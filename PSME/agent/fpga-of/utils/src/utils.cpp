/*!
 * @brief Defines utils for FPGAoF agent
 *
 * @copyright Copyright (c) 2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file utils.cpp
 * */



#include "utils.hpp"
#include "agent-framework/eventing/utils.hpp"
#include "agent-framework/database/database_entities.hpp"
#include "agent-framework/database/database_keys.hpp"

#include "net/network_interface.hpp"
#include "sysfs/sysfs_interface.hpp"

#include <libudev.h>



using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework::eventing;
using namespace agent_framework::database;
using namespace agent::fpgaof;
using namespace agent::fpgaof::utils;

const constexpr char SYSFS_NET_PATH[] = "/sys/class/net/";
const constexpr char SYSFS_INFINIBAND_PATH[] = "/sys/class/infiniband/";

namespace {

void update_processor_status(const std::string& processor_uuid, const attribute::Status& status) {

    auto processor = get_manager<Processor>().get_entry_reference(processor_uuid);

    if (processor->get_is_being_erased()) {
        // we should not overwrite this states
        return;
    }

    attribute::Status prev_status = processor->get_status();
    if (!processor->get_is_in_warning_state() &&
        (prev_status.get_health() != status.get_health() || prev_status.get_state() != status.get_state())) {

        log_debug("agent", "Processor (" << processor_uuid << ") status changed: "
                                         << status.get_health() << ", " << status.get_state());
        processor->set_status(status);
        send_event(processor_uuid, enums::Component::Processor, enums::Notification::Update,
                   processor->get_parent_uuid());
    }
}


void update_processor_erased_db(const std::string& processor_uuid, bool erased) {
    ProcessorEntity(processor_uuid).put(PROCESSOR_ERASED_PROPERTY, std::to_string(static_cast<unsigned>(erased)));
}

}

namespace agent {
namespace fpgaof {
namespace utils {

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#endif

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunsafe-loop-optimizations"
#endif


void partition_target_and_initiator_endpoints(const std::vector<Uuid>& input_endpoints,
                                              std::vector<Uuid>& target_endpoints,
                                              std::vector<Uuid>& initiator_endpoints) {
    for (const auto& endpoint_uuid : input_endpoints) {
        if (Endpoint::is_target(endpoint_uuid)) {
            target_endpoints.push_back(endpoint_uuid);
        }
        else {
            initiator_endpoints.push_back(endpoint_uuid);
        }
    }
}


#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#ifdef __clang__
#pragma clang diagnostic pop
#endif


void set_processor_status(const std::string& processor_uuid,
                          const agent_framework::model::attribute::Status& status) {
    update_processor_status(processor_uuid, status);
}


void set_processor_erased(Processor& processor, bool erased) {
    auto fpga = processor.get_fpga();
    fpga.set_erased(erased);
    processor.set_fpga(fpga);
}


void set_processor_erased_in_model(const Uuid& processor_uuid, bool erased) {
    auto fpga = get_manager<Processor>().get_entry_reference(processor_uuid)->get_fpga();
    fpga.set_erased(erased);
    get_manager<Processor>().get_entry_reference(processor_uuid)->set_fpga(fpga);
}


void update_processor_erased(const std::string& processor_uuid, bool erased) {
    set_processor_erased_in_model(processor_uuid, erased);
    update_processor_erased_db(processor_uuid, erased);
}


void set_processor_is_in_warning_state(const std::string& processor_uuid, bool is_in_warning_state) {
    get_manager<Processor>().get_entry_reference(processor_uuid)->set_is_in_warning_state(is_in_warning_state);
}


void set_processor_is_being_erased(const std::string& processor_uuid, bool is_being_erased) {
    get_manager<Processor>().get_entry_reference(processor_uuid)->set_is_being_erased(is_being_erased);
}


void sync_processors_with_db(const Uuid& system_uuid, std::vector<agent_framework::model::Processor>& processors) {

    auto db_uuids = SystemEntity(system_uuid).get_multiple_values(PROCESSORS_PROPERTY);

    for (auto& processor : processors) {
        auto processor_in_db = std::find_if(std::begin(db_uuids), std::end(db_uuids),
                                            [&processor](const auto& db_uuid) {
                                                return db_uuid == processor.get_uuid();
                                            }) != std::end(db_uuids);
        if (processor_in_db) {
            bool erased = ProcessorEntity(processor.get_uuid()).get(PROCESSOR_ERASED_PROPERTY) == "1";
            set_processor_erased(processor, erased);
        }
        else {
            ProcessorEntity(processor.get_uuid()).put(PROCESSOR_ERASED_PROPERTY, std::to_string(
                static_cast<unsigned>(processor.get_fpga().get_erased())));
            SystemEntity(system_uuid).append(PROCESSORS_PROPERTY, processor.get_uuid());
        }
    }
}


OptionalField<Uuid> get_ethernet_interface_uuid_from_ip_address(const attribute::Ipv4Address& address_from_request) {
    auto interfaces = get_manager<NetworkInterface>()
        .get_keys([&address_from_request](const NetworkInterface& interface) {
            for (const auto& ipv4 : interface.get_ipv4_addresses()) {
                if (ipv4.get_address().has_value() && ipv4.get_address() == address_from_request.get_address()) {
                    return true;
                }
            }
            return false;
        });

    return interfaces.empty() ?
           OptionalField<Uuid>{} :
           OptionalField<Uuid>{interfaces.front()};
}


bool is_rdma_supported_for_address(const std::string& ipv4_address) {

    sysfs::SysfsInterface sysfs_interface;
    auto udev_ctx = udev_new();
    udev_device* udev_ifa_device{nullptr};
    udev_device* udev_ifa_parent_device{nullptr};
    bool result = false;

    /*
     * This code finds device of network interface based on IPv4 address using net, then it's parent device using udev.
     * Then it checks if any of the devices supporting RDMA have the same parent as network interface.
     * If parents have the same sysfs path, we assume it's the same parent device and RDMA is supported.
     *
     * There also some check in-between like having a driver and having a parent device.
     */
    try {
        // Get network interface based on IP address
        auto net_ip_address = net::IpAddress::from_string(ipv4_address);
        auto ifa = net::NetworkInterface::for_address(net_ip_address);

        // Get sysfs path of network interface
        auto ifa_syspath = SYSFS_NET_PATH + ifa.get_name();

        // Get parent device of network interface
        udev_ifa_device = udev_device_new_from_syspath(udev_ctx, ifa_syspath.c_str());
        udev_ifa_parent_device = udev_device_get_parent(udev_ifa_device);

        if (udev_ifa_parent_device == nullptr) {
            if (udev_ifa_device != nullptr) {
                udev_device_unref(udev_ifa_device);
            }
            udev_unref(udev_ctx);
            return false;
        }

        // Get sysfs path of parent device of network interface
        std::string udev_ifa_parent_syspath = udev_device_get_syspath(udev_ifa_parent_device);


        // Enumerate RDMA devices. They are NOT the same as network interfaces, but share their parents.
        if (sysfs_interface.dir_exists(SYSFS_INFINIBAND_PATH)) {
            auto infiniband_dir = sysfs_interface.get_dir(SYSFS_INFINIBAND_PATH);

            for (const auto& infiniband_device: infiniband_dir.links) {
                // Get sysfs path of RDMA device
                auto dev_path = infiniband_device.to_string();

                // Use sysfs path of RDMA device to find its parent and its sysfs path
                auto udev_rdma_device = udev_device_new_from_syspath(udev_ctx, dev_path.c_str());

                auto udev_rdma_parent_device = udev_device_get_parent(udev_rdma_device);

                // No parent, no RDMA support (probably loopback interface)
                if (udev_rdma_parent_device == nullptr) {
                    udev_device_unref(udev_rdma_device);
                    continue;
                }

                // No driver in parent, no RDMA support.
                if (udev_device_get_driver(udev_rdma_parent_device) == nullptr) {
                    continue;
                }

                std::string udev_rdma_parent_syspath = udev_device_get_syspath(udev_rdma_parent_device);

                // Cleanup.
                udev_device_unref(udev_rdma_device);
                udev_device_unref(udev_rdma_parent_device);


                // Check if parent of network interface is the same as parent of RDMA device
                if (udev_ifa_parent_syspath == udev_rdma_parent_syspath) {
                    result = true;
                    break;
                }
            }
        }
    }
    catch (const std::exception& ex) {
        log_error("interface-discoverer", "Error when trying to check RDMA support for IP address'" << ipv4_address
                                                                                                    << ". Assuming lack of support. ': "
                                                                                                    << ex.what());
        result = false;
    }

    // Cleanup
    if (udev_ifa_device != nullptr) {
        udev_device_unref(udev_ifa_device);
    }
    if (udev_ifa_device != nullptr) {
        udev_device_unref(udev_ifa_parent_device);
    }

    udev_unref(udev_ctx);
    return result;
}

}
}
}
