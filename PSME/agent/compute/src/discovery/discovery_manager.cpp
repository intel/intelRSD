/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 *
 * @section DESCRIPTION
 *
 * @file compute/src/discovery/discovery_manager.cpp
 *
 * */

#include "ipmi/manager/ipmitool/management_controller.hpp"
#include "discovery/discovery_manager.hpp"
#include "agent-framework/module-ref/compute_manager.hpp"

#include "ipmi/command/generic/get_device_id.hpp"
#include "ipmi/command/generic/get_system_guid.hpp"
#include "ipmi/command/generic/get_device_guid.hpp"
#include "ipmi/command/sdv/get_bios_version.hpp"
#include "ipmi/command/sdv/get_processor_info.hpp"
#include "ipmi/command/sdv/get_dimm_info.hpp"
#include "ipmi/command/sdv/get_system_mac_address_haswell.hpp"
#include "ipmi/command/sdv/get_slot_id.hpp"
#include "ipmi/command/sdv/enums.hpp"
#include "netlink/utils.hpp"
#include "port_mapper.hpp"

using namespace std;
using namespace ipmi::manager::ipmitool;
using namespace ipmi::command;
using namespace agent::compute::discovery;
using namespace agent::compute;
using namespace agent_framework::model;
using namespace agent_framework::generic;

using ManagementControllerUnique = unique_ptr<ipmi::ManagementController>;
using ComputeComponents = agent_framework::module::ComputeManager;
using COMPLETION_CODE = generic::response::GetDeviceId::COMPLETION_CODE;

using ManagerRef = ObjReference<Manager, std::recursive_mutex>;
using ChassisRef = ObjReference<Chassis, std::recursive_mutex>;
using SystemRef = ObjReference<System, std::recursive_mutex>;
using attribute::FruInfo;

namespace {
    System build_system(const string& manager_uuid,
                        const string& chassis_uuid ) {
        System system{manager_uuid};
        system.set_chassis(chassis_uuid);
        system.add_collection(attribute::Collection(
            enums::CollectionName::Processors,
            enums::CollectionType::Processors,
            ""
        ));

        system.add_collection(attribute::Collection(
            enums::CollectionName::Memories,
            enums::CollectionType::Dimms,
            ""
        ));
        system.add_collection(attribute::Collection(
            enums::CollectionName::StorageControllers,
            enums::CollectionType::StorageControllers,
            ""
        ));
        system.add_collection(attribute::Collection(
            enums::CollectionName::NetworkInterfaces,
            enums::CollectionType::NetworkInterfaces,
            ""
        ));
        system.add_collection(attribute::Collection(
            enums::CollectionName::MemoryChunks,
            enums::CollectionType::MemoryChunks,
            ""
        ));

        attribute::Status system_status;
        system_status.set_health(enums::Health::OK);
        system_status.set_state(enums::State::Enabled);

        system.set_status(system_status);

        agent_framework::model::System::BootOverrideSupported boot_supported;
        boot_supported.add_entry(enums::BootOverrideSupported::Hdd);
        boot_supported.add_entry(enums::BootOverrideSupported::Pxe);
        system.set_boot_override_supported(boot_supported);

        return system;
    }

    constexpr const uint8_t MEMORY_BANK_SIZE = 8;
    string get_socket_name(uint8_t dimm_index) {
        if (dimm_index < MEMORY_BANK_SIZE) {
            log_debug(GET_LOGGER("agent"), "Dimm index: "
                                           << unsigned(dimm_index)
                                           << "Translated to:"
                                           << "A" + to_string(unsigned(dimm_index)));

            return string("A" + to_string(unsigned(dimm_index)));
        }
        else {
            log_debug(GET_LOGGER("agent"), "Dimm index: "
                                           << unsigned(dimm_index)
                                           << "Translated to:"
                                           << "B" + to_string(dimm_index % MEMORY_BANK_SIZE));

            return string("B" + to_string(dimm_index % MEMORY_BANK_SIZE));
        }

    }

    void log_debug_mc(ManagementControllerUnique& mc) {
        log_debug(GET_LOGGER("agent"), "ManagementController:"
                                       << "IP:" << mc->get_ip() << endl
                                       << "PORT:" << mc->get_port() << endl);
    }
    /* Wrapper for logging and runtime_error exception */
    void log_bad_completion_code(const string& uuid, COMPLETION_CODE cc) {
        log_error(GET_LOGGER("agent"), "Bad completion code for: "
                                       << uuid
                                       << " IPMI completion code: "
                                       << uint32_t(cc));
        string msg = "Bad completion code in Get Device Id response."
                   + to_string(uint32_t(cc));
    }

    void set_fru_info_for_each_chassis(vector<ChassisRef>& vec, FruInfo fru) {
        for (auto& chassis_ref : vec) {
            // Copy of fru info on purpose.
            chassis_ref->set_fru_info(fru);
        }
    }

    ManagerRef get_manager(const string& manager_uuid) {
        auto& module_manager = ComputeComponents::get_instance()->
                               get_module_manager();

        if (!module_manager.entry_exists(manager_uuid)) {
            log_error(GET_LOGGER("agent"), "Manager does not exist: "
                      << manager_uuid)
            throw runtime_error("Manager does not exist:" + manager_uuid);
        }
        return module_manager.get_entry_reference(manager_uuid);
    }

    vector<ChassisRef> get_chassis_refs(const string& parent_manager_uuid) {
        auto& chassis_manager = ComputeComponents::get_instance()->
                                get_chassis_manager();

        vector<ChassisRef> chassis_ref_vec{};
        auto chassis_uuids_vec = chassis_manager.get_keys(parent_manager_uuid);

        for (const auto& chassis_uuid : chassis_uuids_vec) {
            auto chassis = chassis_manager.get_entry_reference(chassis_uuid);
            chassis_ref_vec.push_back(move(chassis));
        }
        return chassis_ref_vec;

    }

    vector<SystemRef> get_all_systems(const string& parent_manager_uuid) {
        auto& system_manager = ComputeComponents::get_instance()->
                                get_system_manager();

        vector<SystemRef> system_ref_vec{};
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

    void discover_manager_guid(ManagementControllerUnique& mc,
                               ManagerRef& manager) {
        generic::request::GetDeviceGuid request{};
        generic::response::GetDeviceGuid response{};

        try {
            mc->send(request, response);
        }
        catch (const runtime_error& e) {
            auto status = manager->get_status();
            status.set_health(enums::Health::Warning);
            manager->set_status(status);
            log_error(GET_LOGGER("agent"), "Cannot send command GetDeviceGuid: "
                                           << e.what()
                                           << " for System: "
                                           << manager->get_uuid())
            return;
        }

        const auto cc = response.get_completion_code();

        if (COMPLETION_CODE::COMPLETION_CODE_NORMAL != cc) {
            auto status = manager->get_status();
            status.set_health(enums::Health::Warning);
            manager->set_status(status);
            log_bad_completion_code(manager->get_uuid(), cc);
        }

        manager->set_guid(response.get_guid());

        log_debug(GET_LOGGER("agent"), "Manager: " <<  manager->get_uuid() << " GUID is " << manager->get_guid());

        log_debug(GET_LOGGER("agent"), "GetDeviceGuid successful.");
    }

    void discover_manager_and_all_chassis(ManagementControllerUnique& mc,
                                          ManagerRef& manager) {
        log_debug(GET_LOGGER("agent"), "Send GetDeviceId");

        generic::request::GetDeviceId request{};
        generic::response::GetDeviceId response{};

        try {
            mc->send(request, response);
        } catch (const runtime_error& e) {
            log_error(GET_LOGGER("agent"), "GetDeviceId error:"
                                           << " error: "
                                           << e.what());
            auto status = manager->get_status();
            status.set_health(enums::Health::Critical);
            manager->set_status(status);
            return;
        }

        auto cc = response.get_completion_code();

        if (COMPLETION_CODE::COMPLETION_CODE_NORMAL != cc) {
            log_bad_completion_code(manager->get_uuid(), cc);
            auto status = manager->get_status();
            status.set_health(enums::Health::Critical);
            manager->set_status(status);
            return;
        }

        manager->set_firmware_version(response.get_firmware_version());

        discover_manager_guid(mc, manager);

        auto chassis_ref_vec = get_chassis_refs(manager->get_uuid());

        if (chassis_ref_vec.empty()) {
            log_error(GET_LOGGER("agent"), "Manager does not have chassis!");
        }

        auto chassis_uuid = chassis_ref_vec.front()->get_uuid();

        auto system = build_system(manager->get_uuid(), chassis_uuid);
        ComputeComponents::get_instance()->
        get_system_manager().add_entry(system);

        FruInfo fru_info{};
        fru_info.set_manufacturer(response.get_manufacturer_name());
        fru_info.set_model_number(response.get_product_name());

        set_fru_info_for_each_chassis(chassis_ref_vec, move(fru_info));

        sdv::request::GetSlotId req{};
        sdv::response::GetSlotId res{};

        req.set_network_function(uint8_t(sdv::NetFn::RACKSCALE));

        try {
            mc->send(req, res);
        } catch (const runtime_error& e) {
            log_error(GET_LOGGER("agent"), "GetSlotId error:"
                                           << " error: "
                                           << e.what());
            auto status = manager->get_status();
            status.set_health(enums::Health::Critical);
            manager->set_status(status);
            return;
        }

        cc = response.get_completion_code();

        if (COMPLETION_CODE::COMPLETION_CODE_NORMAL != cc) {
            log_bad_completion_code(manager->get_uuid(), cc);
            auto status = manager->get_status();
            status.set_health(enums::Health::Critical);
            manager->set_status(status);
            return;
        }

        for (auto& chassis_ref : chassis_ref_vec) {
            log_debug(GET_LOGGER("agent"), "Set Slot ID: "
                                           << to_string(unsigned(res.get_slot_id()))
                                           << " for Chassis: "
                                           << chassis_ref->get_uuid());
            chassis_ref->set_location_offset(res.get_slot_id());
        }

        log_debug(GET_LOGGER("agent"),
                  "Produce name: " << response.get_product_name() <<
                  "Manufacturer name: " << response.get_manufacturer_name() <<
                  "Firmware version: " << response.get_firmware_version());
    }

    void discover_bios(ManagementControllerUnique& mc, SystemRef& system) {
        log_debug(GET_LOGGER("agent"), "Send GetBiosVersion");
        sdv::request::GetBiosVersion request{};
        sdv::response::GetBiosVersion response{};

        try {
            mc->send(request, response);
        }
        catch (const runtime_error& e) {
             log_error(GET_LOGGER("agent"), "Cannot send command GetBiosVersion: "
                                       << e.what()
                                       << " for System: "
                                       << system->get_uuid());
             auto status = system->get_status();
             status.set_health(enums::Health::Warning);
             system->set_status(status);
             return;
        }

        auto cc = response.get_completion_code();

        if (COMPLETION_CODE::COMPLETION_CODE_NORMAL != cc) {
            auto status = system->get_status();
            status.set_health(enums::Health::Warning);
            system->set_status(status);
            log_bad_completion_code(system->get_uuid(), cc);
            return;
        }
        system->set_bios_version(response.get_version());

        log_debug(GET_LOGGER("agent"), "GetBiosVersion successful.");
    }

    constexpr const uint8_t HASSWEL_CPU_NO = 2;
    void discover_processors(ManagementControllerUnique& mc, SystemRef& system) {
        sdv::request::GetProcessorInfo request{};
        sdv::response::GetProcessorInfo response{};

        for (uint8_t cpu_index = 0; cpu_index < HASSWEL_CPU_NO ; cpu_index++) {
            log_debug(GET_LOGGER("agent"), "Send GetProcessorInfo");
            request.set_id(cpu_index);

            try {
                mc->send(request, response);
            }
            catch (const runtime_error& e) {
                auto status = system->get_status();
                status.set_health(enums::Health::Warning);
                system->set_status(status);
                log_error(GET_LOGGER("agent"), "Cannot send command GetProcessorInfo: "
                                              << e.what()
                                              << " for System: "
                                              << system->get_uuid());
               continue;
            }

            const auto cc = response.get_completion_code();

            if (COMPLETION_CODE::COMPLETION_CODE_NORMAL != cc) {
                auto status = system->get_status();
                status.set_health(enums::Health::Warning);
                system->set_status(status);
                log_bad_completion_code(system->get_uuid(), cc);
                continue;
            }

            if (!response.is_present()) {
                continue;
            }

            Processor cpu{system->get_uuid()};
            cpu.set_max_speed_mhz(response.get_cpu_frequency());
            cpu.set_processor_type(enums::ProcessorType::CPU);
            cpu.set_model_name(response.get_cpu_type_name());
            cpu.set_socket(to_string(unsigned(cpu_index)));

            attribute::Status status;
            status.set_state(enums::State::Enabled);
            cpu.set_status(status);

            ComputeComponents::get_instance()->
            get_processor_manager().add_entry(move(cpu));

            log_debug(GET_LOGGER("agent"), "GetProcessorInfo successful.");
        }

    }

    constexpr const uint8_t HASSWEL_MEMORY_NO = 16;
    void discover_dimm_modules(ManagementControllerUnique& mc,
                                 SystemRef& system) {
        sdv::request::GetDimmInfo request{};
        sdv::response::GetDimmInfo response{};

        // Dimm modules in HSW are indexed from 1.
        for (uint8_t index = 1 ; index <= HASSWEL_MEMORY_NO; index++) {
            log_debug(GET_LOGGER("agent"), "Send GetDimmInfo");
            request.set_dimm_index(index);

            try {
                mc->send(request, response);
            }
            catch (const runtime_error& e) {
                auto status = system->get_status();
                status.set_health(enums::Health::Warning);
                system->set_status(status);
                log_error(GET_LOGGER("agent"), "Cannot send command GetDimmInfo: "
                                               << e.what()
                                               << " for System: "
                                               << system->get_uuid());
                continue;
            }

            const auto cc = response.get_completion_code();

            if (COMPLETION_CODE::COMPLETION_CODE_NORMAL != cc) {
                auto status = system->get_status();
                status.set_health(enums::Health::Warning);
                system->set_status(status);
                log_bad_completion_code(system->get_uuid(), cc);
                continue;
            }

            if (!response.is_present()) {
                continue;
            }

            Dimm dimm{system->get_uuid()};

            const auto capacity_mb = response.get_dimm_size_mbytes();
            dimm.set_capacity_mb(capacity_mb);
            dimm.set_operating_speed_mhz(response.get_dimm_speed_mhz());
            dimm.set_max_voltage_volt(response.get_dimm_voltage());
            dimm.set_min_voltage_volt(response.get_dimm_voltage());
            dimm.set_memory_type(enums::MemoryType::DRAM);
            dimm.add_dimm_media(enums::DimmMedia::DRAM);

            /*
             * Deep Discovery numbering starts from 0. IPMI from 1.
             * Need to translate IPMI index into LSWH (used in Deep Discovery)
             * socket name (literal + numbering from 0);
             */
            dimm.set_device_locator(get_socket_name(uint8_t(index - 1)));

            attribute::Status status;
            status.set_state(enums::State::Enabled);
            dimm.set_status(status);

            attribute::Region region;
            region.set_region_id("0");
            region.set_offset_mb(0);
            region.set_size_mb(capacity_mb);
            dimm.add_region(std::move(region));

            ComputeComponents::get_instance()->
                get_dimm_manager().add_entry(move(dimm));

            log_debug(GET_LOGGER("agent"), "GetDimmInfo successful.");
        }
    }

    void discover_rrc_network_interface(SystemRef& system) {
        NetworkInterface interface{system->get_uuid()};
        attribute::Status status;
        try {
            const auto& port_identifier = PortMapper::get_instance()->
                get_switch_port_identifier(system->get_parent_uuid());
            auto mac_address = netlink_base::utils::
                get_neighbor_mac_address(port_identifier);
            status.set_state(enums::State::Enabled);
            interface.set_factory_mac_address(mac_address);
            interface.set_mac_address(mac_address);
        }
        catch (const std::runtime_error& e) {
            log_error(GET_LOGGER("agent"),
                      "Cannot read MAC address from Netlink for system "
                      << system->get_uuid() << " : " << e.what());
            status.set_state(enums::State::Disabled);
        }

        interface.set_status(status);
        ComputeComponents::get_instance()->
            get_network_interface_manager().add_entry(move(interface));
    }

    void discover_network_interfaces(ManagementControllerUnique& mc,
                                      SystemRef& system) {
        log_debug(GET_LOGGER("agent"), "Send GetSystemMacAddress");

        sdv::request::GetSystemMacAddressHaswell  request{};
        sdv::response::GetSystemMacAddressHaswell response{};

        /*
         * This is workaround for BDC-RRC switch issue.
         * BDC-RRC has two network interfaces, but in legacy mode it is possible
         * to retrieve information only about one.
         * This code retrieve information only about first one to avoid setting
         * System::State::Health to Warning because this would prevent assemble
         * from working correctly.
         */
        const uint8_t nics_ids[] = {1} ;

        for (const uint8_t nic_index : nics_ids) {
            request.set_interface_id(nic_index);

             try {
                mc->send(request, response);
            }
            catch (const runtime_error& e) {
                auto status = system->get_status();
                status.set_health(enums::Health::Warning);
                system->set_status(status);

                log_error(GET_LOGGER("agent"), "Cannot send command GetSystemMacAddress: "
                                               << e.what()
                                               << " for System: "
                                               << system->get_uuid());
                continue;
            }

            const auto cc = response.get_completion_code();

            if (COMPLETION_CODE::COMPLETION_CODE_NORMAL != cc) {
                auto status = system->get_status();
                status.set_health(enums::Health::Warning);
                system->set_status(status);
                log_bad_completion_code(system->get_uuid(), cc);
                continue;
            }

            NetworkInterface interface{system->get_uuid()};

            interface.set_mac_address(response.get_system_mac_address());
            interface.set_factory_mac_address(response.get_system_mac_address());

            attribute::Status status;
            status.set_state(enums::State::Enabled);
            interface.set_status(status);

            ComputeComponents::get_instance()->
            get_network_interface_manager().add_entry(move(interface));

            log_debug(GET_LOGGER("agent"), "GetSystemMacAddress successful.");
        }

        /* retrieve information about factory MAC address on RRC interface
         * using Netlink API */
        discover_rrc_network_interface(system);
    }

    void discover_system_guid(ManagementControllerUnique& mc,
                                 SystemRef& system) {
        generic::request::GetSystemGuid request{};
        generic::response::GetSystemGuid response{};

        try {
            mc->send(request, response);
        }
        catch (const runtime_error& e) {
            auto status = system->get_status();
            status.set_health(enums::Health::Warning);
            system->set_status(status);
            log_error(GET_LOGGER("agent"), "Cannot send command GetSystemGuid: "
                                           << e.what()
                                           << " for System: "
                                           << system->get_uuid())
            return;
        }

        const auto cc = response.get_completion_code();

        if (COMPLETION_CODE::COMPLETION_CODE_NORMAL != cc) {
            auto status = system->get_status();
            status.set_health(enums::Health::Warning);
            system->set_status(status);
            log_bad_completion_code(system->get_uuid(), cc);
        }

        system->set_guid(response.get_guid());

        log_debug(GET_LOGGER("agent"), "System: " <<  system->get_uuid() << " GUID is " << system->get_guid());

        log_debug(GET_LOGGER("agent"), "GetSystemGuid successful.");
    }

    void set_all_systems_to_critical_health(ManagerRef& manager) {
        auto systems = get_all_systems(manager->get_uuid());

        for (auto it = systems.begin(); it != systems.end(); ++it) {
            auto status = (*it)->get_status();
            status.set_health(enums::Health::Critical);
            (*it)->set_status(status);
        }
    }

    void discover_all_systems(ManagementControllerUnique& mc,
                              ManagerRef& manager) {
        auto systems = get_all_systems(manager->get_uuid());

        for (auto& system: systems) {
            discover_bios(mc, system);
            discover_processors(mc, system);
            discover_dimm_modules(mc, system);
            discover_network_interfaces(mc, system);
            discover_system_guid(mc, system);
        }
    }

    void create_memory_chunks(const string& manager_uuid) {

        auto systems =
            ComputeComponents::get_instance()->
                get_system_manager().get_keys(manager_uuid);

        for (const auto& sys : systems) {
            MemoryChunk chunk{sys};
            std::uint32_t total_size{0};
            auto memories =
                ComputeComponents::get_instance()->
                    get_dimm_manager().get_keys(sys);

            for (const auto& memory_uuid : memories) {
                auto memory = ComputeComponents::get_instance()->
                    get_dimm_manager().get_entry_reference(memory_uuid);
                for (const auto& region: memory->get_regions()) {
                    total_size += region.get_size_mb();
                    chunk.add_interleave_set({memory_uuid, region.get_region_id()});
                }
            }

            chunk.set_chunk_size_mb(total_size);
            chunk.set_type(agent_framework::model::enums::MemoryChunkType::Volatile);
            chunk.set_status(true);
            log_debug(GET_LOGGER("agent"), "Creating memory chunk: "
                                          << chunk.get_uuid());
            ComputeComponents::get_instance()->
                get_memory_chunk_manager().add_entry(std::move(chunk));
        }
    }
}

DiscoveryManager::DiscoveryManager(): m_mc{new ManagementController()}
{}

DiscoveryManager::DiscoveryManager(ManagementControllerUnique& mc):
m_mc(move(mc)){}

DiscoveryManager::~DiscoveryManager() {}

void DiscoveryManager::discovery(const string& manager_uuid) {
    log_info(GET_LOGGER("agent"), "Starting Discovery for Manager:"
                                  << manager_uuid);
    auto manager = get_manager(manager_uuid);
    auto connection_data = manager->get_connection_data();

    set_ipmi_connection_data(m_mc, connection_data);
    log_debug_mc(m_mc);

    discover_manager_and_all_chassis(m_mc, manager);

    const auto manager_health = manager->get_status().get_health();

    if (enums::Health::Critical == manager_health) {
        set_all_systems_to_critical_health(manager);
    }
    else {
        discover_all_systems(m_mc, manager);
    }

    create_memory_chunks(manager_uuid);

    log_info(GET_LOGGER("agent"), "Discovery done for Manager:"
                                  << manager_uuid);
}
