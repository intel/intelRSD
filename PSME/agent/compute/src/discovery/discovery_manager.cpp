/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * */

#include "agent-framework/module/compute_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "ipmi/manager/ipmitool/management_controller.hpp"
#include "ipmi/command/sdv/get_mdr_data_region_status.hpp"
#include "ipmi/command/sdv/mdr_region_read.hpp"
#include "smbios/smbios_parser.hpp"

#include "discovery/common.hpp"
#include "discovery/discovery_manager.hpp"
#include "discovery/basic/network_device_and_function.hpp"
#include "discovery/basic/manager_and_chassis.hpp"
#include "discovery/basic/system.hpp"
#include "discovery/smbios/manager_and_chassis.hpp"
#include "discovery/smbios/system.hpp"
#include "discovery/basic/bios.hpp"
#include "discovery/smbios/bios.hpp"
#include "discovery/basic/cable_id.hpp"
#include "discovery/basic/processor.hpp"
#include "discovery/smbios/processor.hpp"
#include "discovery/basic/memory.hpp"
#include "discovery/smbios/memory.hpp"
#include "discovery/basic/network.hpp"
#include "discovery/smbios/network_interface.hpp"
#include "discovery/smbios/pci_device.hpp"
#include "discovery/smbios/storage_controller_and_drives.hpp"

#include "make_unique.hpp"

using namespace ipmi::manager::ipmitool;
using namespace ipmi::command;
using namespace agent::compute::discovery;
using namespace agent::compute;
using namespace agent_framework::model;
using namespace agent_framework::generic;
using namespace ::smbios::parser;

using agent_framework::module::ComputeComponents;
using agent_framework::module::CommonComponents;

using ManagementControllerUnique = std::unique_ptr<ipmi::ManagementController>;
using COMPLETION_CODE = ipmi::Response::COMPLETION_CODE;

using ManagerRef = ObjReference<Manager, std::recursive_mutex>;
using SystemRef = ObjReference<System, std::recursive_mutex>;

namespace {

System build_system(const string& manager_uuid,
                    const string& chassis_uuid) {
    System system{manager_uuid};
    system.set_chassis(chassis_uuid);
    system.add_collection(attribute::Collection(
        enums::CollectionName::Processors,
        enums::CollectionType::Processors,
        ""
    ));

    system.add_collection(attribute::Collection(
        enums::CollectionName::Memory,
        enums::CollectionType::Memory,
        ""
    ));
    system.add_collection(attribute::Collection(
        enums::CollectionName::StorageSubsystems,
        enums::CollectionType::StorageSubsystems,
        ""
    ));
    system.add_collection(attribute::Collection(
        enums::CollectionName::NetworkInterfaces,
        enums::CollectionType::NetworkInterfaces,
        ""
    ));
    system.add_collection(attribute::Collection(
        enums::CollectionName::NetworkDevices,
        enums::CollectionType::NetworkDevices,
        ""
    ));

    attribute::Status system_status;
    system_status.set_health(enums::Health::OK);
    system_status.set_state(enums::State::Enabled);

    system.set_status(system_status);

    agent_framework::model::System::BootOverrideSupported boot_supported;
    boot_supported.add_entry(enums::BootOverrideTarget::Hdd);
    boot_supported.add_entry(enums::BootOverrideTarget::Pxe);
    boot_supported.add_entry(enums::BootOverrideTarget::None);
    boot_supported.add_entry(enums::BootOverrideTarget::RemoteDrive);
    system.set_boot_override_supported(boot_supported);

    return system;
}


void discover_all_systems(ipmi::ManagementController& mc,
                          ::smbios::parser::SmbiosParser::Ptr smbios,
                          std::vector<System::Reference>& systems,
                          const std::string& chassis_uuid) {

    bool status{false};
    enums::Health health = enums::Health::OK;

    for (auto& system_ref : systems) {
        auto& system = system_ref.get_raw_ref();
        log_info(GET_LOGGER("compute-discovery"), "Starting Discovery for System: " << system.get_uuid());

        log_info(GET_LOGGER("smbios-discovery"), "Discovering BIOS from SMBIOS table.");
        std::tie(status, health) = discovery::smbios::discover_bios(smbios, system);
        if (!status) {
            log_warning(GET_LOGGER("smbios-discovery"), "BIOS discovery failed. Fallback to legacy mode.");
            std::tie(status, health) = basic::discover_bios(mc, system);
        }
        update_health(system, health);

        log_info(GET_LOGGER("smbios-discovery"), "Discovering System from SMBIOS table.");
        std::tie(status, health) = discovery::smbios::discover_system(smbios, system);
        if (!status) {
            log_warning(GET_LOGGER("smbios-discovery"), "System discovery failed. Fallback to legacy mode.");
            std::tie(status, health) = basic::discover_system(mc, system);
        }
        update_health(system, health);

        log_info(GET_LOGGER("basic-discovery"), "Discovering cable Ids...");
        std::tie(status, health) = basic::discover_cable_id(mc, system);
        update_health(system, health);

        log_info(GET_LOGGER("basic-discovery"), "Discovering network device and function...");
        std::tie(status, health) = basic::discover_network_device(mc, system);
        update_health(system, health);


        log_info(GET_LOGGER("smbios-discovery"), "Discovering Processors from SMBIOS table.");
        std::tie(status, health) = discovery::smbios::discover_processor(smbios, system);
        if (!status) {
            log_warning(GET_LOGGER("smbios-discovery"), "Processor discovery failed. Fallback to legacy mode.");
            std::tie(status, health) = basic::discover_processors(mc, system);
        }
        update_health(system, health);

        log_info(GET_LOGGER("smbios-discovery"), "Discovering Memory Modules from SMBIOS table.");
        std::tie(status, health) = discovery::smbios::discover_memory_modules(smbios, system);
        if (!status) {
            log_warning(GET_LOGGER("smbios-discovery"), "Memory Modules discovery failed. Fallback to legacy mode.");
            std::tie(status, health) = basic::discover_memory_modules(mc, system);
        }
        update_health(system, health);

        log_info(GET_LOGGER("smbios-discovery"), "Discovering Network Interfaces from SMBIOS table.");
        std::tie(status, health) = discovery::smbios::discover_network_interfaces(smbios, system);
        if (!status) {
            log_warning(GET_LOGGER("smbios-discovery"), "Network Interfaces discovery failed. Fallback to legacy mode.");
            std::tie(status, health) = basic::discover_network_interfaces(mc, system);
        }
        update_health(system, health);

        log_info(GET_LOGGER("smbios-discovery"), "Discovering PCI Devices from SMBIOS table.");
        std::tie(status, health) = discovery::smbios::discover_pci_devices(smbios, system);
        update_health(system, health);

        log_info(GET_LOGGER("smbios-discovery"), "Discovering Drives from SMBIOS table.");
        std::tie(status, health) = discovery::smbios::discover_drives(smbios, system, chassis_uuid);
        update_health(system, health);
    }
}


void log_debug_mc(ManagementControllerUnique& mc) {
    log_debug(GET_LOGGER("compute-discovery"), "ManagementController = " <<
                                           "IP: " << mc->get_ip() << ", " <<
                                           "PORT: " << mc->get_port() << std::endl);
}


ManagerRef get_manager(const string& manager_uuid) {
    auto& module_manager = CommonComponents::get_instance()->get_module_manager();

    if (!module_manager.entry_exists(manager_uuid)) {
        THROW(agent_framework::exceptions::InvalidUuid, "compute-discovery",
              "Manager does not exist: '" + manager_uuid + "'.");
    }
    return module_manager.get_entry_reference(manager_uuid);
}


std::vector<SystemRef> get_all_systems(const string& parent_manager_uuid) {
    auto& system_manager = CommonComponents::get_instance()->
        get_system_manager();

    std::vector<SystemRef> system_ref_vec{};
    auto systems_uuids_vec = system_manager.get_keys(parent_manager_uuid);

    for (const auto& system_uuid : systems_uuids_vec) {
        auto system = system_manager.get_entry_reference(system_uuid);
        system_ref_vec.push_back(move(system));
    }
    return system_ref_vec;

}


void set_ipmi_connection_data(ManagementControllerUnique& mc,
                              const attribute::ConnectionData& data) {
    mc->set_ip(data.get_ip_address());
    mc->set_port(data.get_port());
    mc->set_username(data.get_username());
    mc->set_password(data.get_password());
}


void set_all_systems_to_critical_health(ManagerRef& manager) {
    auto systems = get_all_systems(manager->get_uuid());

    for (auto it = systems.begin(); it != systems.end(); ++it) {
        auto status = (*it)->get_status();
        status.set_health(enums::Health::Critical);
        (*it)->set_status(status);
    }
}

}


DiscoveryManager::DiscoveryManager() :
    m_mc{future::make_unique<ManagementController>()} { }


DiscoveryManager::DiscoveryManager(ManagementControllerUnique& mc) :
    m_mc(move(mc)) { }


DiscoveryManager::~DiscoveryManager() { }


void DiscoveryManager::discovery(const string& manager_uuid) {
    auto manager = get_manager(manager_uuid);
    auto connection_data = manager->get_connection_data();
    std::string chassis_uuid;
    attribute::FruInfo fruInfo;

    set_ipmi_connection_data(m_mc, connection_data);
    log_debug_mc(m_mc);

    std::shared_ptr<SmbiosParser> smbios{nullptr};
    {
        std::uint16_t bytes_to_read{};

        bool status{false};
        /* Starting discovery based on data read from SMBIOS */
        log_info(GET_LOGGER("smbios-discovery"), "Trying to read MDR data region [" << connection_data.get_ip_address() << "].");
        std::tie(status, bytes_to_read) = get_mdr_data_region();
        if (status) {
            std::vector<std::uint8_t> mdr_region_data{};
            std::tie(status, mdr_region_data) = read_mdr_data(bytes_to_read);

            try {
                smbios.reset(new SmbiosParser(mdr_region_data.data(), mdr_region_data.size()));
            }
            catch (const SmbiosParser::Exception& exception) {
                smbios.reset();
                log_error(GET_LOGGER("smbios-discovery"),
                          "Error while parsing SMBIOS structures. "
                              << "Fallback to legacy discovery. Message: " << exception.what());
            }
        }
        else {
            log_warning(GET_LOGGER("smbios-discovery"), "Getting MDR data region failed. Fallback to legacy discovery.");
        }
    }

    {
        bool status{false};
        enums::Health health = enums::Health::OK;

        log_info(GET_LOGGER("compute-discovery"), "Starting Discovery for Manager: " << manager_uuid);
        if (smbios) {
            log_info(GET_LOGGER("smbios-discovery"), "Discovering Manager from SMBIOS table [" << connection_data.get_ip_address() << "].");
            status = smbios::discover_manager(smbios, manager.get_raw_ref(), fruInfo);
        }
        if (!status) {
            log_warning(GET_LOGGER("smbios-discovery"), "Manager discovery failed. Fallback to legacy mode.");
            std::tie(status, health) = basic::discover_manager(*m_mc.get(), manager.get_raw_ref(), fruInfo);
            update_health(manager.get_raw_ref(), health);
        }
        if (!status) {
            log_error(GET_LOGGER("compute-discovery"), "Error while discovering manager. "
                                                   << "Manager UUID: " << manager->get_uuid());
        }
    }

    {
        bool status{false};
        enums::Health health = enums::Health::OK;

        log_info(GET_LOGGER("compute-discovery"), "Starting Discovery for Chassis: " << chassis_uuid);
        if (smbios) {
            log_info(GET_LOGGER("smbios-discovery"), "Discovering Chassis from SMBIOS table [" << connection_data.get_ip_address() << "].");
            status = smbios::discover_all_chassis(smbios, manager.get_raw_ref(), chassis_uuid);
        }
        if (!status) {
            log_warning(GET_LOGGER("smbios-discovery"), "Chassis discovery failed. Fallback to legacy mode.");
            std::tie(status, health) = basic::discover_all_chassis(
                *m_mc.get(), manager.get_raw_ref(), fruInfo, chassis_uuid);
            update_health(manager.get_raw_ref(), health);
        }
        if (!status) {
            log_error(GET_LOGGER("compute-discovery"), "Error while discovering manager and chassis. "
                                                   << "Manager UUID: " << manager->get_uuid());
        }
    }

    {
        bool status{false};
        enums::Health health = enums::Health::OK;

        log_info(GET_LOGGER("compute-discovery"), "Starting Discovery for Manager GUID: " << manager->get_uuid());
        if (smbios) {
            log_info(GET_LOGGER("smbios-discovery"), "Discovering Manager GUID from SMBIOS table [" << connection_data.get_ip_address() << "].");
            status = smbios::discover_manager_guid(smbios, manager.get_raw_ref());
        }
        if (!status) {
            log_warning(GET_LOGGER("smbios-discovery"), "Manager GUID discovery failed. Fallback to legacy mode.");
            std::tie(status, health) = basic::discover_manager_guid(*m_mc.get(), manager.get_raw_ref());
            update_health(manager.get_raw_ref(), health);
        }
        if (!status) {
            log_error(GET_LOGGER("compute-discovery"), "Error while discovering manager GUID. "
                                                   << "Manager UUID: " << manager->get_uuid());
        }
    }

    {
        bool status{false};
        enums::Health health = enums::Health::OK;

        log_info(GET_LOGGER("compute-discovery"), "Starting Discovery for Chassis location for Manager: " << manager->get_uuid());
        if (smbios) {
            log_info(GET_LOGGER("smbios-discovery"), "Discovering Chassis location from SMBIOS table [" << connection_data.get_ip_address() << "].");
            status = smbios::discover_chassis_location(smbios, manager.get_raw_ref());
        }
        if (!status) {
            log_warning(GET_LOGGER("smbios-discovery"), "Chassis location discovery failed. Fallback to legacy mode.");
            std::tie(status, health) = basic::discover_chassis_location(*m_mc.get(), manager.get_raw_ref());
            update_health(manager.get_raw_ref(), health);
        }
        if (!status) {
            log_error(GET_LOGGER("compute-discovery"), "Error while discovering chassis location. "
                                                   << "Manager UUID: " << manager->get_uuid());
        }
    }

    /* Create system regardless of discovery result */
    auto system = build_system(manager->get_uuid(), chassis_uuid);
    CommonComponents::get_instance()->get_system_manager().add_entry(system);

    const auto manager_health = manager->get_status().get_health();
    if (enums::Health::Critical == manager_health) {
        set_all_systems_to_critical_health(manager);
    }
    else {
        auto systems = get_all_systems(manager->get_uuid());
        log_info(GET_LOGGER("compute-discovery"), "Starting Discovery for all Systems Resources under Manager: " << manager->get_uuid());
        discover_all_systems(*m_mc.get(), smbios, systems, chassis_uuid);
    }
    log_info(GET_LOGGER("compute-discovery"), "Discovery completed for Manager: " << manager_uuid);
}


std::tuple<bool, std::uint16_t> DiscoveryManager::get_mdr_data_region() {
    const auto error = std::make_tuple(false, 0);
    log_debug(GET_LOGGER("smbios-discovery"), "Send GetMdrDataRegionStatus.");
    sdv::request::GetMdrDataRegionStatus request{};
    sdv::response::GetMdrDataRegionStatus response{};

    request.set_data_region_id(sdv::DataRegionId::SMBIOS_TABLE);
    try {
        m_mc->send(request, response);
    }
    catch (const std::runtime_error& e) {
        log_error(GET_LOGGER("smbios-discovery"), "Cannot send command GetMdrDataRegionStatus: " << e.what());
        return error;
    }

    auto cc = response.get_completion_code();
    if (COMPLETION_CODE::COMPLETION_CODE_NORMAL != cc) {
        log_error(GET_LOGGER("smbios-discovery"), "GetMdrDataRegionStatus command error.");
        log_bad_completion_code(cc, "GetMdrDataRegionStatus");
        return error;
    }

    /* Make sure it isn't locked */
    if (response.get_lock_status() != sdv::response::GetMdrDataRegionStatus::LockStatus::UNLOCKED) {
        log_warning(GET_LOGGER("smbios-discovery"), "MDR Region locked!");
        return error;
    }
        /* Make sure the data there is valid */
    else if (response.get_data_validation() != sdv::response::GetMdrDataRegionStatus::DataValidation::VALID) {
        log_warning(GET_LOGGER("smbios-discovery"), "MDR Region not valid!");
        return error;
    }
        /* Make sure there is data to read */
    else if (response.get_region_size_used() == 0) {
        log_warning(GET_LOGGER("smbios-discovery"), "MDR Region is empty!");
        return error;
    }

    log_debug(GET_LOGGER("smbios-discovery"), "GetMdrRegionStatus successful.");
    return std::make_tuple(true, response.get_region_size_used());
}


std::tuple<bool, std::vector<std::uint8_t>> DiscoveryManager::read_mdr_data(std::uint16_t bytes_to_read) {
    const auto error = make_tuple(false, std::vector<std::uint8_t>());

    std::uint16_t wrote{};
    std::uint16_t offset{};
    std::uint16_t to_fetch{};
    std::vector<std::uint8_t> mdr_region_data{};

    /* Loop until everything is written */
    while (wrote < bytes_to_read) {

        /* Update where to start the read/write */
        offset = wrote;
        /* How much is left to be read/written */
        to_fetch = std::uint16_t(bytes_to_read - wrote);

        /* There is a limit to the maximum number of bytes we can read */
        if (to_fetch > sdv::request::MdrRegionRead::MAX_DATA_COUNT) {
            /* Cap it */
            to_fetch = sdv::request::MdrRegionRead::MAX_DATA_COUNT;
        }

        log_debug(GET_LOGGER("smbios-discovery"), "Send GetMdrDataRegionStatus.");
        sdv::request::MdrRegionRead request{};
        sdv::response::MdrRegionRead response{std::uint8_t(to_fetch)};

        request.set_data_region_id(sdv::DataRegionId::SMBIOS_TABLE);
        request.set_data_length(std::uint8_t(to_fetch));
        request.set_offset(offset);
        try {
            m_mc->send(request, response);
        }
        catch (const std::runtime_error& e) {
            log_error(GET_LOGGER("smbios-discovery"), "Cannot send command MdrRegionRead: " << e.what());
            return error;
        }

        auto cc = response.get_completion_code();
        if (COMPLETION_CODE::COMPLETION_CODE_NORMAL != cc) {
            log_error(GET_LOGGER("smbios-discovery"), "MdrRegionRead command error.");
            log_bad_completion_code(cc, "MdrRegionRead");
            return error;
        }

        /* Read a block */
        const auto& response_data = response.get_data();
        mdr_region_data.insert(mdr_region_data.end(), response_data.cbegin(), response_data.cend());

        if (0 == response_data.size()) {
            log_warning(GET_LOGGER("smbios-discovery"), "MdrRegionRead returns 0 size data!");
            return error;
        }

        /* Update our counter and loop */
        wrote = std::uint16_t(wrote + response_data.size());
        log_debug(GET_LOGGER("smbios-discovery"), "MdrRegionRead successful.");
    }

    return make_tuple(true, mdr_region_data);
}
