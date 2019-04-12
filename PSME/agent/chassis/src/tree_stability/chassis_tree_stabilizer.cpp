/*!
 * @brief Provides implementation of ChassisTreeStabilizer class
 *
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * @file chassis_tree_stabilizer.cpp
 * */



#include "tree_stability/chassis_tree_stabilizer.hpp"
#include "agent-framework/module/managers/generic_manager.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/eventing/events_queue.hpp"

#include <string>
#include <iostream>



using namespace agent_framework::eventing;
using namespace agent::chassis;
using namespace agent_framework::module;

namespace {

/*! Auxiliary namespace to hold prefixes for unique key construction, needed to maintain data integrity */
namespace literals {
constexpr const char MODULE_KEY_BASE[] = "Module_";
constexpr const char SLED_MANAGER_KEY_BASE[] = "SledModule_";
constexpr const char DRAWER_KEY_BASE[] = "Drawer_";
}

std::mutex manager_mutex{};


/*!
 * Check if drawer manager has already been assigned a persistent UUID.
 * This function is needed in order for the resource tree manager
 * (i.e. drawer manager) and the underlying chassis to be assigned
 * persistent UUIDs only once.
 *
 * @return True if it has already been stabilized, false otherwise
 * */
bool is_drawer_manager_stabilized() {
    const auto drawer_manager_uuid = get_manager<agent_framework::model::Manager>().get_keys("").front();
    const auto drawer_manager = get_manager<agent_framework::model::Manager>().get_entry(drawer_manager_uuid);

    return drawer_manager.get_unique_key().has_value();
}


/*!
 * Generate persistent UUIDs for drawer manager and its chassis.
 * The action of this function is transparent to the rest of the
 * application as all resources holding references to the updated
 * components are also updated.
 * */
void stabilize_drawer_and_manager() {
    std::lock_guard<std::mutex> lock{manager_mutex};

    // not necessary to stabilize drawer/manager once again
    if (is_drawer_manager_stabilized()) {
        return;
    }

    std::string module_persistent_uuid{};
    std::string drawer_persistent_uuid{};
    const auto module_uuid = get_manager<agent_framework::model::Manager>().get_keys("").front();
    const auto sled_modules_uuids = get_manager<agent_framework::model::Manager>().get_keys(module_uuid);
    const auto chassis_uuid = get_manager<agent_framework::model::Chassis>().get_keys(module_uuid).front();

    log_info(LOGUSR, "Generating persistent UUID for resources for Manager " << module_uuid);

    /*! Make persistent UUID for drawer manager */
    {
        auto module = get_manager<agent_framework::model::Manager>().get_entry_reference(module_uuid);
        module->set_unique_key(literals::MODULE_KEY_BASE);
        module->make_persistent_uuid();
        module_persistent_uuid = module->get_uuid();

        /*! Update chassis and generate persistent UUID */
        {
            auto drawer = get_manager<agent_framework::model::Chassis>().get_entry_reference(chassis_uuid);
            drawer->set_parent_uuid(module_persistent_uuid);
            drawer->set_unique_key(literals::DRAWER_KEY_BASE);
            drawer->make_persistent_uuid();
            drawer_persistent_uuid = drawer->get_uuid();
        }

        module->set_location(drawer_persistent_uuid);
    }

    /*! Update underlying sled managers */
    for (const auto& sled_module_uuid : sled_modules_uuids) {
        get_manager<agent_framework::model::Manager>().get_entry_reference(sled_module_uuid)->
            set_parent_uuid(module_persistent_uuid);
    }

    // This event needs to be sent here for a moment to ensure that no false events are
    // passed to the REST server.
    agent_framework::model::attribute::EventData edat{};
    edat.set_parent("");
    edat.set_component(module_persistent_uuid);
    edat.set_type(agent_framework::model::enums::Component::Manager);
    edat.set_notification(agent_framework::model::enums::Notification::Add);
    EventsQueue::get_instance()->push_back(edat);

    log_debug("tree-stability",
              "Persistent UUID for Module " << module_uuid << " generated: " << module_persistent_uuid);
    log_debug("tree-stability",
              "Persistent UUID for Drawer " << chassis_uuid << " generated: " << drawer_persistent_uuid);
}

}


ChassisTreeStabilizer::~ChassisTreeStabilizer() {}


const std::string ChassisTreeStabilizer::stabilize(const std::string& module_uuid) {
    // must be in synchronized block, is called from multiple threads at once!
    stabilize_drawer_and_manager();

    std::string module_persistent_uuid{};
    std::string module_unique_key{literals::SLED_MANAGER_KEY_BASE};
    const std::string& module_ip = get_manager<agent_framework::model::Manager>()
        .get_entry(module_uuid).get_connection_data().get_ip_address();

    const auto keys = get_manager<agent_framework::model::Chassis>().get_keys(module_uuid);
    // it is just a sanity check.. with synchronization it never executes!
    if (keys.empty()) {
        log_error(LOGUSR, "Blade " << module_ip << "/" << module_uuid << " not in the chassis_manager!");
        THROW(::agent_framework::exceptions::InvalidUuid, "agent",
              "Blade [UUID = " + module_uuid + "] not in the chassis_manager");
    }
    const auto blade_chassis_uuid = keys.front();

    module_unique_key.append(module_ip);

    {
        auto sled_module = get_manager<agent_framework::model::Manager>().get_entry_reference(module_uuid);
        sled_module->set_unique_key(module_unique_key);
        sled_module->make_persistent_uuid();
        module_persistent_uuid = sled_module->get_uuid();
    }

    get_manager<agent_framework::model::Chassis>()
        .get_entry_reference(blade_chassis_uuid)->set_parent_uuid(module_persistent_uuid);

    log_debug(LOGUSR,
              "Persistent UUID for Blade " << module_ip << "/" << module_uuid << " generated: " << module_persistent_uuid);

    return module_persistent_uuid;
}
