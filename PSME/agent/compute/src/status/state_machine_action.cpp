/*!
 * @brief Implementation of StateMachineAction class
 *
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
 *
 * @file state_machine_action.cpp
*/
#include "status/state_machine_action.hpp"
#include "discovery/discovery_manager.hpp"
#include "tree_stability/compute_tree_stabilizer.hpp"
#include "agent-framework/module/compute_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/eventing/events_queue.hpp"



using namespace agent::compute;
using namespace agent_framework;
using namespace agent_framework::state_machine;
using namespace agent_framework::module;

using agent_framework::state_machine::enums::State;
using agent_framework::model::enums::Component;
using agent_framework::state_machine::enums::Transition;
using agent_framework::eventing::Notification;
using agent_framework::module::CommonComponents;
using agent_framework::module::ComputeComponents;

namespace {

bool is_notification_required(const Transition trans) {
    switch (trans) {
        case Transition::CAME_UP:
        case Transition::WENT_DOWN:
        case Transition::STARTED:
        case Transition::DISCOVERY_UP:
        case Transition::EXTRACTION:
        case Transition::INSERTION:
            return true;
        case Transition::IDLE:
        case Transition::DISCOVERY_MISSING:
        case Transition::DISCOVERY_DOWN:
        case Transition::DID_NOT_START:
        case Transition::SLOW_INSERTION:
        default:
            return false;
    }
}


Notification select_notification_for_came_up(bool was_discovered) {
    if (was_discovered) {
        return Notification::Update;
    }
    else {
        return Notification::Add;
    }
}


Notification select_notification(const Transition trans, bool was_discovered) {
    switch (trans) {
        case Transition::CAME_UP:
            return select_notification_for_came_up(was_discovered);
        case Transition::WENT_DOWN:
            return Notification::Update;
        case Transition::STARTED:
        case Transition::DISCOVERY_UP:
        case Transition::INSERTION:
            return Notification::Add;
        case Transition::EXTRACTION:
            return Notification::Remove;
        case Transition::IDLE:
        case Transition::DISCOVERY_MISSING:
        case Transition::DISCOVERY_DOWN:
        case Transition::DID_NOT_START:
        case Transition::SLOW_INSERTION:
        default:
            // this should never happen
            assert(false);
            return Notification::Update;
    }
}


void send_event(const std::string& module, const std::string& parent, Component type, Notification notification) {
    ::agent_framework::eventing::EventData edat;
    edat.set_parent(parent);
    edat.set_component(module);
    edat.set_type(type);
    edat.set_notification(notification);
    ::agent_framework::eventing::EventsQueue::get_instance()->push_back(edat);
    log_info(GET_LOGGER("agent"), "Sending an event for component " << edat.get_component() << "," << "("
                                  << edat.get_type() << ")" << ", notification type: " << edat.get_notification());
}


void remove_processors(const string& system_uuid) {
    auto& pm = ComputeComponents::get_instance()->get_processor_manager();
    pm.remove_by_parent(system_uuid);
}


void remove_memory_modules(const string& system_uuid) {
    auto& mm = ComputeComponents::get_instance()->get_memory_manager();
    mm.remove_by_parent(system_uuid);
}


void remove_network_interfaces(const string& system_uuid) {
    auto& nicm = ComputeComponents::get_instance()->
        get_network_interface_manager();
    nicm.remove_by_parent(system_uuid);
}


void remove_storage_subsystems(const string& system_uuid) {
    auto& ssm = CommonComponents::get_instance()->
        get_storage_subsystem_manager();
    auto& scm = ComputeComponents::get_instance()->
        get_storage_controller_manager();
    for (const auto& subsystem_uuid : ssm.get_keys(system_uuid)) {
        scm.remove_by_parent(subsystem_uuid);
    }
    ssm.remove_by_parent(system_uuid);
}

void remove_network_devices(const string& system_uuid) {
    auto& ndm = ComputeComponents::get_instance()->
        get_network_device_manager();
    auto& ndfm = ComputeComponents::get_instance()->
        get_network_device_function_manager();
    for (const auto& device_uuid : ndm.get_keys(system_uuid)) {
        ndfm.remove_by_parent(device_uuid);
    }
    ndm.remove_by_parent(system_uuid);
}


void remove_systems(const string& manager_uuid) {
    auto& sm = CommonComponents::get_instance()->get_system_manager();
    auto system_keys = sm.get_keys(manager_uuid);
    for (const auto& key: system_keys) {
        remove_processors(key);
        remove_memory_modules(key);
        remove_network_interfaces(key);
        remove_storage_subsystems(key);
        remove_network_devices(key);
    }
    sm.remove_by_parent(manager_uuid);
    for (const auto& key: system_keys) {
        // sending delete events for systems
        send_event(key, manager_uuid, Component::System, Notification::Remove);
    }
}


void update_status(const string& manager_uuid, model::enums::State state, bool was_discovered) {
    auto& chm = CommonComponents::get_instance()->get_chassis_manager();
    auto chassis_keys = chm.get_keys(manager_uuid);

    for (const auto& key : chassis_keys) {
        auto chassis = chm.get_entry_reference(key);
        auto status = chassis->get_status();
        status.set_state(state);
        chassis->set_status(status);
        // sending update events for chassis

        if (was_discovered) {
            send_event(key, manager_uuid, Component::Chassis, Notification::Update);
        }
        else {
            send_event(key, manager_uuid, Component::Chassis, Notification::Add);
        }

    }

    auto& mm = CommonComponents::get_instance()->get_module_manager();
    auto manager = mm.get_entry_reference(manager_uuid);

    auto status = manager->get_status();
    status.set_state(state);
    manager->set_status(status);
}


void generate_systems_add_event(const std::string& module) {
    auto& sm = CommonComponents::get_instance()->get_system_manager();
    auto system_keys = sm.get_keys(module);

    // Sending add events for systems
    for (const auto& key : system_keys) {
        send_event(key, module, Component::System, Notification::Add);
    }
}


const std::string randomize_module_uuid(const std::string& module_uuid) {
    auto& chassis_manager = CommonComponents::get_instance()->get_chassis_manager();
    auto& module_manager = CommonComponents::get_instance()->get_module_manager();
    const auto& chassis_keys = chassis_manager.get_keys(module_uuid);

    if (chassis_keys.empty()) {
        throw std::runtime_error("No chassis found for module " + module_uuid);
    }

    const std::string module_random_uuid = module_manager.get_entry_reference(module_uuid)->make_random_uuid();
    {
        auto chassis = chassis_manager.get_entry_reference(chassis_keys.front());
        chassis->set_parent_uuid(module_random_uuid);
        chassis->make_random_uuid();
    }

    return module_random_uuid;
}

}

using agent_framework::discovery::Discovery;
using DiscoveryUnique = std::unique_ptr<Discovery>;


void StateMachineAction::execute(StateThreadEntrySharedPtr entry) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto module = entry->get_module();
    auto state = entry->get_state();
    auto trans = entry->get_transition();
    bool was_discovered = entry->was_discovered();

    log_debug(GET_LOGGER("agent"), "State machine action in module (UUID=" + module + "): after event "
                                   + trans.to_string() + ", current state is " + state.to_string());

    {
        bool presence = (state == State::ENABLED || (state == State::UNAVAILABLE_OFFLINE && was_discovered));
        CommonComponents::get_instance()->get_module_manager().get_entry_reference(module)->set_presence(presence);
    }

    switch (state) {
        case State::ENABLED:
            log_debug(GET_LOGGER("agent"), "\tAction: Discovery");

            try {
                discovery::DiscoveryManager().discovery(module);
                module = ComputeTreeStabilizer().stabilize(module);
                entry->update_module(module);
            }
            catch (const std::runtime_error& e) {
                log_error(GET_LOGGER("agent"), e.what());
            }
            update_status(module, model::enums::State::Enabled, was_discovered);
            generate_systems_add_event(module);
            entry->set_discovered(true);
            break;

        case State::ABSENT:
        case State::UNAVAILABLE_OFFLINE:
            if (enums::Transition::EXTRACTION == trans || enums::Transition::WENT_DOWN == trans) {
                log_debug(GET_LOGGER("agent"), "\tAction: Removing systems for manager");

                update_status(module, model::enums::State::UnavailableOffline, was_discovered);
                remove_systems(module);
            }
            if (State::ABSENT == state) {
                entry->set_discovered(false);
                entry->update_module(randomize_module_uuid(module));
            }
            break;

        case State::UNKNOWN:
        case State::STARTING:
        default:
            break;
    }

    // sending events for manager
    if (is_notification_required(trans)) {
        send_event(module, {}, Component::Manager, select_notification(trans, was_discovered));
    }
}
