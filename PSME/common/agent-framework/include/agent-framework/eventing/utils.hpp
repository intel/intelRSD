/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file eventing/utils.hpp
 */

#pragma once



#include "agent-framework/eventing/events_queue.hpp"
#include "agent-framework/module/managers/utils/manager_utils.hpp"



namespace agent_framework {
namespace eventing {

void send_event(const Uuid& component_uuid,
                const agent_framework::model::enums::Component& component,
                const agent_framework::model::enums::Notification& notification,
                const Uuid& parent_uuid = "");


template<typename M>
void send_add_notifications_for_each() {
    model::attribute::EventData::Vector notifications{};
    for (const auto& entry : agent_framework::module::get_manager<M>().get_entries()) {
        model::attribute::EventData edat{};
        edat.set_component(entry.get_uuid());
        edat.set_type(M::get_component());
        edat.set_parent(entry.get_parent_uuid());
        edat.set_notification(agent_framework::model::enums::Notification::Add);
        notifications.push_back(edat);
    }
    agent_framework::eventing::EventsQueue::get_instance()->push_back(notifications);
}


template<typename M>
void send_update(const M& model) {
    send_event(model.get_uuid(),
               M::get_component(),
               agent_framework::model::enums::Notification::Update,
               model.get_parent_uuid());
}

}
}
