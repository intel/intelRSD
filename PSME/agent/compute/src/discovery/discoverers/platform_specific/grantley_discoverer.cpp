/*!
 * @brief Compute agent Grantley platform discoverer.
 *
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @header{Filesystem}
 * @file grantley_discoverer.cpp
 */

#include "discovery/discoverers/platform_specific/grantley_discoverer.hpp"
#include "ipmi/command/sdv/get_slot_id.hpp"
#include "ipmi/command/sdv/get_bios_version.hpp"
#include "ipmi/command/sdv/get_cable_id.hpp"
#include "ipmi/command/sdv/get_processor_info.hpp"
#include "ipmi/command/sdv/get_dimm_info.hpp"
#include "ipmi/command/sdv/iscsi_oob_boot/get_oob_control_boot_options.hpp"
#include "ipmi/command/sdv/iscsi_oob_boot/get_oob_initiator_boot_options.hpp"
#include "ipmi/command/sdv/iscsi_oob_boot/get_oob_nic_boot_options.hpp"
#include "ipmi/command/sdv/iscsi_oob_boot/get_oob_target_boot_options.hpp"
#include "ipmi/command/sdv/iscsi_oob_boot/get_oob_heap_boot_options.hpp"
#include "ipmi/command/sdv/iscsi_oob_boot/set_oob_heap_boot_options.hpp"
#include "ipmi/command/sdv/get_system_mac_address_haswell.hpp"

#include "discovery/builders/platform_specific/grantley/grantley_chassis_builder.hpp"
#include "discovery/builders/platform_specific/grantley/grantley_system_builder.hpp"
#include "discovery/builders/platform_specific/grantley/grantley_processor_builder.hpp"
#include "discovery/builders/platform_specific/grantley/grantley_memory_builder.hpp"
#include "discovery/builders/platform_specific/grantley/grantley_network_device_function_builder.hpp"
#include "discovery/builders/platform_specific/grantley/grantley_network_interface_builder.hpp"



using namespace agent::compute::discovery;
using namespace agent_framework::model;
using namespace ipmi::command;

namespace {

static constexpr const uint8_t GRANTLEY_CPU_NUMBER = 2;
static constexpr const uint8_t GRANTLEY_MEMORY_NUMBER = 16;


/*!
 * @brief Read iSCSI OOB heap.
 * @param mc Sled management controller object.
 * @param off Offset of data to read.
 * @param len Length of data to read.
 * @return Read data.
 */
std::string read_from_heap(ipmi::IpmiController& mc, int off, int len) {
    uint16_t offset = uint16_t(off);
    uint16_t length = uint16_t(len);
    ipmi::command::sdv::request::GetOobHeapBootOptions request{offset, length};
    ipmi::command::sdv::response::GetOobHeapBootOptions response{};

    mc.send(request, response);

    std::vector<uint8_t> heap_string = response.get_heap();

    if (heap_string.size() != length) {
        std::string message{
            "Unable to fetch Heap string from OOB Boot Options. Expected " + std::to_string(uint32_t(length))
            + " bytes but received " + std::to_string(heap_string.size()) + " bytes."};
        log_error("agent", message);
        throw std::runtime_error{message};
    }

    return std::string(heap_string.begin(), heap_string.end());
}


/*!
 * @brief Read string from iSCSI OOB heap.
 * @param mc Sled management controller object.
 * @param off Offset of data to read.
 * @param len Length of data to read.
 * @return Read data.
 */
std::string get_string_from_heap(ipmi::IpmiController& mc, uint16_t offset, uint16_t length) {
    int MAX_BLOCK_SIZE = ipmi::command::sdv::request::SetOobHeapBootOptions::MAX_BLOCK_SIZE;
    std::stringstream ss;
    int blocks = length / MAX_BLOCK_SIZE;
    int delta = length % MAX_BLOCK_SIZE;

    for (uint16_t block = 0; block < blocks; ++block) {
        ss << read_from_heap(mc, offset + block * MAX_BLOCK_SIZE, MAX_BLOCK_SIZE);
    }
    if (delta != 0) {
        ss << read_from_heap(mc, offset + blocks * MAX_BLOCK_SIZE, delta);
    }

    return ss.str();
}

}


GrantleyDiscoverer::~GrantleyDiscoverer() {
}


std::uint32_t GrantleyDiscoverer::get_platform_id() const {
    return ipmi::command::generic::ProductId::PRODUCT_ID_INTEL_XEON_BDC_R;
}


bool GrantleyDiscoverer::discover_chassis(Chassis& chassis) {
    GenericDiscoverer::discover_chassis(chassis);

    sdv::request::GetSlotId get_slot_id_request{};
    sdv::response::GetSlotId get_slot_id_response{};

    try {
        log_debug("grantley-discoverer", "Sending " << get_slot_id_request.get_command_name());
        get_management_controller().send(get_slot_id_request, get_slot_id_response);

        GrantleyChassisBuilder::update_slot_id(chassis, get_slot_id_response);
    }
    catch (const ipmi::ResponseError& response_error) {
        log_ipmi_error(response_error);
        return false;
    }
    catch (const std::runtime_error& e) {
        log_discovery_error(e, chassis.get_component().to_string());
        return false;
    }

    return true;
}


bool GrantleyDiscoverer::discover_processors(std::vector<Processor>& processors,
                                             const std::string& parent_uuid) {
    bool generic_discovery_successful = GenericDiscoverer::discover_processors(processors, parent_uuid);

    if (!generic_discovery_successful) {
        processors.clear();

        sdv::request::GetProcessorInfo get_processor_info_request{};
        sdv::response::GetProcessorInfo get_processor_info_response{};

        for (uint8_t cpu_index = 0; cpu_index < GRANTLEY_CPU_NUMBER; cpu_index++) {
            auto processor = GrantleyProcessorBuilder::build_default(parent_uuid);

            try {
                log_debug("grantley-discoverer", "Sending " << get_processor_info_request.get_command_name());
                get_management_controller().send(get_processor_info_request, get_processor_info_response);

                GrantleyProcessorBuilder::update_processor_info(processor, get_processor_info_response);
                GrantleyProcessorBuilder::update_socket_id(processor, cpu_index);
            }
            catch (const ipmi::ResponseError& response_error) {
                log_ipmi_error(response_error);
                return false;
            }
            catch (const std::runtime_error& e) {
                log_discovery_error(e, processor.get_component().to_string());
                return false;
            }

            processors.push_back(std::move(processor));
        }
    }

    return true;
}


bool GrantleyDiscoverer::discover_memory(std::vector<Memory>& memories,
                                         const std::string& parent_uuid) {
    bool generic_discovery_successful = GenericDiscoverer::discover_memory(memories, parent_uuid);

    if (!generic_discovery_successful) {
        memories.clear();

        sdv::request::GetDimmInfo get_dimm_info_request{};
        sdv::response::GetDimmInfo get_dimm_info_response{};

        for (std::uint8_t index = 1; index <= GRANTLEY_MEMORY_NUMBER; index++) {
            auto memory = GrantleyMemoryBuilder::build_default(parent_uuid);

            try {
                log_debug("grantley-discoverer", "Sending " << get_dimm_info_request.get_command_name());
                get_management_controller().send(get_dimm_info_request, get_dimm_info_response);
                GrantleyMemoryBuilder::update_dimm_info(memory, get_dimm_info_response);
                GrantleyMemoryBuilder::update_memory_index(memory, index);
            }
            catch (const ipmi::ResponseError& response_error) {
                log_ipmi_error(response_error);
                return false;
            }
            catch (const std::runtime_error& e) {
                log_discovery_error(e, memory.get_component().to_string());
                return false;
            }

            memories.push_back(memory);
        }
    }

    return true;
}


bool GrantleyDiscoverer::discover_network_interfaces(
    std::vector<NetworkInterface>& network_interfaces, const std::string& parent_uuid) {
    bool generic_discover_successful = GenericDiscoverer::discover_network_interfaces(network_interfaces, parent_uuid);

    if (!generic_discover_successful) {
        network_interfaces.clear();

        sdv::request::GetSystemMacAddressHaswell get_mac_address_request{};
        sdv::response::GetSystemMacAddressHaswell get_mac_address_response{};

        get_mac_address_request.set_interface_id(1);

        auto interface_from_ipmi = GrantleyNetworkInterfaceBuilder::build_default(parent_uuid);
        // @TODO: address this issue!
        // auto interface_from_netlink = GrantleyNetworkInterfaceBuilder::build_default(parent_uuid);

        try {
            log_debug("grantley-discoverer", "Sending " << get_mac_address_request.get_command_name());
            get_management_controller().send(get_mac_address_request, get_mac_address_response);

            GrantleyNetworkInterfaceBuilder::update_mac_address_info(interface_from_ipmi, get_mac_address_response);
        }
        catch (const ipmi::ResponseError& response_error) {
            log_ipmi_error(response_error);
            return false;
        }
        catch (const std::runtime_error& e) {
            log_discovery_error(e, NetworkInterface::get_component().to_string());
            return false;
        }

        network_interfaces.push_back(std::move(interface_from_ipmi));
    }

    return true;
}


bool GrantleyDiscoverer::discover_network_device_function(
    NetworkDeviceFunction& network_device_function) {
    bool generic_discovery_successful = GenericDiscoverer::discover_network_device_function(network_device_function);

    if (!generic_discovery_successful) {
        sdv::request::GetOobControlBootOptions control_request{};
        sdv::response::GetOobControlBootOptions control_response{};
        sdv::request::GetOobInitiatorBootOptions initiator_request{};
        sdv::response::GetOobInitiatorBootOptions initiator_response{};
        sdv::request::GetOobNicBootOptions nic_request{};
        sdv::response::GetOobNicBootOptions nic_response{};
        sdv::request::GetOobTargetBootOptions target_request{};
        sdv::response::GetOobTargetBootOptions target_response{};
        std::string initiator_name{};
        std::string primary_target_name{};
        std::string chap_username{};
        std::string chap_secret{};
        std::string mutual_chap_username{};
        std::string mutual_chap_secret{};

        try {
            log_debug("grantley-discoverer", "Sending " << control_request.get_command_name());
            get_management_controller().send(control_request, control_response);
            log_debug("grantley-discoverer", "Sending " << initiator_request.get_command_name());
            get_management_controller().send(initiator_request, initiator_response);
            log_debug("grantley-discoverer", "Sending " << nic_request.get_command_name());
            get_management_controller().send(nic_request, nic_response);
            log_debug("grantley-discoverer", "Sending " << target_request.get_command_name());
            get_management_controller().send(target_request, target_response);

            if (initiator_response.get_name_length() != 0) {
                initiator_name = get_string_from_heap(get_management_controller(),
                                                      initiator_response.get_name_offset(),
                                                      initiator_response.get_name_length());
            }
        }
        catch (const ipmi::UpdateInProgressError&) {
            log_info("grantley-discoverer",
                     "Parameter update in progress, iSCSI OOB attributes cannot be discovered.");
        }
        catch (const ipmi::ResponseError& response_error) {
            log_error("grantley-discoverer", "Could not discover iSCSI OOB attributes for function "
                << network_device_function.get_uuid() << ". Exception: " << response_error.what());
        }

        GrantleyNetworkDeviceFunctionBuilder::update_oob_control_boot_options(network_device_function,
                                                                              control_response);
        GrantleyNetworkDeviceFunctionBuilder::update_initator_name(network_device_function, initiator_name);
        GrantleyNetworkDeviceFunctionBuilder::update_oob_nic_boot_options(network_device_function, nic_response);

        if (target_response.get_target_name_length() != 0) {
            primary_target_name = get_string_from_heap(get_management_controller(),
                                                       target_response.get_target_name_offset(),
                                                       target_response.get_target_name_length());
        }

        if (target_response.get_chapname_length() != 0) {
            chap_username = get_string_from_heap(get_management_controller(), target_response.get_chapname_offset(),
                                                 target_response.get_chapname_length());
        }

        if (target_response.get_chapsecret_length() != 0) {
            chap_secret = get_string_from_heap(get_management_controller(), target_response.get_chapsecret_offset(),
                                               target_response.get_chapsecret_length());
        }

        if (target_response.get_mutual_chapname_length() != 0) {
            mutual_chap_username = get_string_from_heap(get_management_controller(),
                                                        target_response.get_mutual_chapname_offset(),
                                                        target_response.get_mutual_chapname_length());
        }

        if (target_response.get_mutual_chapsecret_length() != 0) {
            mutual_chap_secret = get_string_from_heap(get_management_controller(),
                                                      target_response.get_mutual_chapsecret_offset(),
                                                      target_response.get_mutual_chapsecret_length());
        }

        GrantleyNetworkDeviceFunctionBuilder::update_oob_target_boot_options(network_device_function,
                                                                             target_response,
                                                                             primary_target_name,
                                                                             chap_username,
                                                                             chap_secret,
                                                                             mutual_chap_username,
                                                                             mutual_chap_secret);
    }

    // enable the NetworkDeviceFunction
    auto status = network_device_function.get_status();
    status.set_state(enums::State::Enabled);
    network_device_function.set_status(status);

    return true;
}


bool GrantleyDiscoverer::discover_cable_id(System& system) {
    bool generic_discovery_successful = GenericDiscoverer::discover_cable_id(system);

    if (!generic_discovery_successful) {
        for (size_t channel = 0; channel < sdv::request::GetCableId::MAX_CHANNEL_NUMBER; channel++) {
            sdv::request::GetCableId get_cable_id_request{};
            sdv::response::GetCableId get_cable_id_response{};

            get_cable_id_request.set_channel(channel);

            try {
                log_debug("grantley-discoverer", "Sending " << get_cable_id_request.get_command_name());
                get_management_controller().send(get_cable_id_request, get_cable_id_response);
                GrantleySystemBuilder::add_cable_id(system, get_cable_id_response);
            }
            catch (const ipmi::ResponseError& response_error) {
                log_ipmi_error(response_error);
            }
            catch (const std::runtime_error& e) {
                log_discovery_error(e, system.get_component().to_string());
            }
        }
    }

    return true;
}


bool GrantleyDiscoverer::discover_bios(System& system) {
    bool generic_discovery_successful = GenericDiscoverer::discover_bios(system);

    if (!generic_discovery_successful) {
        sdv::request::GetBiosVersion get_bios_version_request{};
        sdv::response::GetBiosVersion get_bios_version_response{};

        try {
            log_debug("grantley-discoverer", "Sending " << get_bios_version_request.get_command_name());
            get_management_controller().send(get_bios_version_request, get_bios_version_response);
            GrantleySystemBuilder::update_bios_version(system, get_bios_version_response);
        }
        catch (const ipmi::ResponseError& response_error) {
            log_ipmi_error(response_error);
            return false;
        }
        catch (const std::runtime_error& e) {
            log_discovery_error(e, system.get_component().to_string());
            return false;
        }
    }

    return true;
}
