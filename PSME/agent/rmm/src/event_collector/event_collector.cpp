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
 * @file rmm/event_collector/event_collector.cpp
 * */

#include "event_collector/event_collector.hpp"
#include "agent-framework/eventing/events_queue.hpp"

using namespace agent::rmm::event_collector;

void EventCollector::poll_add_event(const agent_framework::model::enums::Component component,
    const std::string& resource_uuid,
    const std::string& parent_uuid) {

    poll_event(component, resource_uuid, parent_uuid, agent_framework::model::enums::Notification::Add);
}

void EventCollector::poll_remove_event(const agent_framework::model::enums::Component component,
    const std::string& resource_uuid,
    const std::string& parent_uuid) {

    poll_event(component, resource_uuid, parent_uuid, agent_framework::model::enums::Notification::Remove);
}


void EventCollector::poll_update_event(const agent_framework::model::enums::Component component,
    const std::string& resource_uuid,
    const std::string& parent_uuid) {

    poll_event(component, resource_uuid, parent_uuid, agent_framework::model::enums::Notification::Update);
}

void EventCollector::poll_event(const agent_framework::model::enums::Component component,
    const std::string& resource_uuid,
    const std::string& parent_uuid,
    agent_framework::model::enums::Notification event_type) {

    /* Any squashing should be done here */

    agent_framework::model::attribute::EventData event{};
    event.set_component(resource_uuid);
    event.set_parent(parent_uuid);
    event.set_type(component);
    event.set_notification(event_type);
    m_event_pool.emplace_back(std::move(event));

}

void EventCollector::send() {
    agent_framework::model::attribute::EventData::Vector notifications(m_event_pool.begin(), m_event_pool.end());
    agent_framework::eventing::EventsQueue::get_instance()->push_back(notifications);
    clear();
}

void EventCollector::clear() {
    m_event_pool.clear();
}
