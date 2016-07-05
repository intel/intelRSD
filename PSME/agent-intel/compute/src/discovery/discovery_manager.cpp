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
#include "agent-framework/module-ref/model/processor.hpp"
#include "agent-framework/module-ref/model/attributes/status.hpp"

#include "ipmi/command/generic/get_device_id.hpp"

using namespace std;
using namespace ipmi::command::generic;
using namespace agent::compute::discovery;
using namespace agent_framework::model;
using namespace agent_framework::generic;

using ManagementControllerUnique = unique_ptr<ipmi::ManagementController>;
using ComputeComponents = agent_framework::module::ComputeManager;
using COMPLETION_CODE = response::GetDeviceId::COMPLETION_CODE;

using ManagerRef = ObjReference<Manager, std::recursive_mutex>;
using ChassisRef = ObjReference<Chassis, std::recursive_mutex>;
using SystemRef = ObjReference<System, std::recursive_mutex>;
using attribute::FruInfo;

namespace {

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

    void discover_manager_and_all_chassis(ManagementControllerUnique& mc,
                                          ManagerRef& manager) {
        log_debug(GET_LOGGER("agent"), "Send GetDeviceId");

        request::GetDeviceId request{};
        response::GetDeviceId response{};

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

        auto chassis_ref_vec = get_chassis_refs(manager->get_uuid());

        FruInfo fru_info{};
        fru_info.set_manufacturer(response.get_manufacturer_name());
        fru_info.set_model_number(response.get_product_name());

        set_fru_info_for_each_chassis(chassis_ref_vec, move(fru_info));

        log_debug(GET_LOGGER("agent"),
                  "Produce name: " << response.get_product_name() <<
                  "Manufacturer name: " << response.get_manufacturer_name() <<
                  "Firmware version: " << response.get_firmware_version());
    }

    void discover_bios(ManagementControllerUnique&, SystemRef&) {
    }

    void discover_processors(ManagementControllerUnique& , SystemRef&) {
    }

    void discover_dimm_modules(ManagementControllerUnique&, SystemRef&) {
    }

    void discover_memory_chunks(ManagementControllerUnique&, SystemRef&) {
    }

    void discover_netowork_interfaces(ManagementControllerUnique&, SystemRef&) {
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
            discover_memory_chunks(mc, system);
            discover_netowork_interfaces(mc, system);
        }
    }

}

DiscoveryManager::DiscoveryManager(): m_mc{new ipmi::manager::ipmitool::ManagementController()}
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

    log_info(GET_LOGGER("agent"), "Discovery done for Manager:"
                                  << manager_uuid);
}
