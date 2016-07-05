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
*/

#include "logger/logger_factory.hpp"
#include "status/state_machine_action.hpp"
#include "discovery/discovery_manager.hpp"
#include "agent-framework/state_machine/module_state.hpp"
#include "agent-framework/module-ref/compute_manager.hpp"
#include "agent-framework/eventing/events_queue.hpp"

using namespace agent::compute;
using namespace agent_framework;
using namespace agent_framework::state_machine;
using namespace agent_framework::module;

using ComputeComponents = agent_framework::module::ComputeManager;
using State = agent_framework::state_machine::enums::State;
using Transition = agent_framework::state_machine::enums::Transition;

namespace {

    bool is_notification_required(const Transition trans) {
        switch(trans) {
            case Transition::CAME_UP:
            case Transition::WENT_DOWN:
            case Transition::INSERTION:
            case Transition::DISCOVERY_DOWN:
            case Transition::DISCOVERY_UP:
            case Transition::EXTRACTION:
                return true;
            case Transition::IDLE:
            case Transition::DISCOVERY_MISSING:
            default:
                return false;
        }
    }

    ::agent_framework::eventing::Notification select_notification(
            const Transition trans) {
        switch(trans) {
            case Transition::CAME_UP:
            case Transition::WENT_DOWN:
                return ::agent_framework::eventing::Notification::Update;
            case Transition::INSERTION:
            case Transition::DISCOVERY_DOWN:
            case Transition::DISCOVERY_UP:
                return ::agent_framework::eventing::Notification::Add;
            case Transition::EXTRACTION:
                return ::agent_framework::eventing::Notification::Remove;
            case Transition::IDLE:
            case Transition::DISCOVERY_MISSING:
            default:
                // this should never happen
                assert(false);
                return ::agent_framework::eventing::Notification::Update;
        }
    }

    void send_event(const std::string& module, const std::string& parent,
                    ::agent_framework::model::enums::Component type,
                    ::agent_framework::eventing::Notification notification) {
        ::agent_framework::eventing::EventData edat;
        edat.set_parent(parent);
        edat.set_component(module);
        edat.set_type(type);
        edat.set_notification(notification);
        ::agent_framework::eventing::EventsQueue::get_instance()->push_back(edat);
        log_info(GET_LOGGER("agent"), "Sending an event for component "
                << edat.get_component() << ","
                << "(" << edat.get_type() << ")"
                << ", notification type: " << edat.get_notification());
    }

    void remove_processors(const string& system_uuid) {
        auto& pm = ComputeComponents::get_instance()->get_processor_manager();
        pm.remove_by_parent(system_uuid);
    }

    void remove_dimms(const string& system_uuid) {
        auto& mm = ComputeComponents::get_instance()->get_dimm_manager();
        mm.remove_by_parent(system_uuid);
    }

    void remove_network_interfaces(const string& system_uuid) {
        auto& nicm = ComputeComponents::get_instance()->
                     get_network_interface_manager();
        nicm.remove_by_parent(system_uuid);
    }

    void remove_systems(const string& manager_uuid) {
        auto& sm = ComputeComponents::get_instance()->get_system_manager();
        auto system_keys = sm.get_keys(manager_uuid);
        for (const auto& key: system_keys) {
            remove_processors(key);
            remove_dimms(key);
            remove_network_interfaces(key);
        }
        sm.remove_by_parent(manager_uuid);
        for (const auto& key: system_keys) {
            // sending delete events for systems
            send_event(key, manager_uuid, ::agent_framework::model::enums::Component::System,
                ::agent_framework::eventing::Notification::Remove);
        }
    }

    void update_status(const string& manager_uuid, model::enums::State state) {
        auto& chm =  ComputeComponents::get_instance()->get_chassis_manager();
        auto chassis_keys = chm.get_keys(manager_uuid);

        for (const auto& key : chassis_keys) {
            auto chassis = chm.get_entry_reference(key);
            auto status = chassis->get_status();
            status.set_state(state);
            chassis->set_status(status);
            // sending update events for chassis
            send_event(key, manager_uuid, ::agent_framework::model::enums::Component::Chassis,
                ::agent_framework::eventing::Notification::Update);
        }

        auto& mm =  ComputeComponents::get_instance()->get_module_manager();
        auto manager = mm.get_entry_reference(manager_uuid);

        auto status = manager->get_status();
        status.set_state(state);
        manager->set_status(status);
    }

    void generate_systems_add_event(const std::string& module) {
        auto& sm = ComputeComponents::get_instance()->get_system_manager();
        auto system_keys = sm.get_keys(module);

        // Sending add events for systems
        for (const auto& key : system_keys) {
            send_event(key, module, ::agent_framework::model::enums::Component::System,
                ::agent_framework::eventing::Notification::Add);
        }
    }

}

using agent_framework::discovery::Discovery;
using DiscoveryUnique = std::unique_ptr<Discovery>;

void StateMachineAction::execute(const std::string& module,
                                 State state, Transition trans) {

    log_debug(GET_LOGGER("agent"), "State machine action in module (UUID=" + module
        + "): after event " + trans.to_string() + ", current state is " + state.to_string());

    bool presence = (state == State::STARTING
                  || state == State::ENABLED
                  || state == State::OFFLINE);

    {
        ComputeComponents::get_instance()->
            get_module_manager().get_entry_reference(module)->set_presence(presence);
    }

    switch(state) {
        case State::STARTING:
            log_debug(GET_LOGGER("agent"), "\tAction: Updating status");
            update_status(module, model::enums::State::Starting);
            break;
        case State::ENABLED:
            log_debug(GET_LOGGER("agent"), "\tAction: Discovery");
            try {
                discovery::DiscoveryManager().discovery(module);
            }
            catch(const std::runtime_error& e) {
                log_error(GET_LOGGER("agent"), e.what());
            }
            update_status(module,model::enums::State::Enabled);
            generate_systems_add_event(module);

            break;

        case State::ABSENT:
        case State::OFFLINE:
            log_debug(GET_LOGGER("agent"), "\tAction: Removing systems for manager");
            update_status(module, model::enums::State::Offline);
            remove_systems(module);
            break;

        case State::UNKNOWN:
        default:
            break;
    }

    // sending events for manager
    if (is_notification_required(trans)) {
        send_event(module, {}, ::agent_framework::model::enums::Component::Manager,
            select_notification(trans));
    }

}
