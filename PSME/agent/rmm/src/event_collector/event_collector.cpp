/*!
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
 * @file rmm/event_collector/event_collector.cpp
 */

#include "event_collector/event_collector.hpp"
#include "agent-framework/eventing/events_queue.hpp"

using namespace agent::rmm::event_collector;
using namespace agent_framework::eventing;

void EventCollector::poll_add_event(const enums::Component component, const std::string& resource_uuid,
    const std::string& parent_uuid) {

    poll_event(component, resource_uuid, parent_uuid, Notification::Add);
}

void EventCollector::poll_remove_event(const enums::Component component, const std::string& resource_uuid,
    const std::string& parent_uuid) {

    poll_event(component, resource_uuid, parent_uuid, Notification::Remove);
}


void EventCollector::poll_update_event(const enums::Component component, const std::string& resource_uuid,
    const std::string& parent_uuid) {

    poll_event(component, resource_uuid, parent_uuid, Notification::Update);
}

void EventCollector::poll_event(const enums::Component component, const std::string& resource_uuid,
    const std::string& parent_uuid, Notification event_type) {

    /* Any squashing should be done here */

    EventData event{};
    event.set_component(resource_uuid);
    event.set_parent(parent_uuid);
    event.set_type(component);
    event.set_notification(event_type);
    m_event_pool.emplace_back(std::move(event));

}

void EventCollector::send() {
    EventDataVec notifications(m_event_pool.begin(), m_event_pool.end());
    EventsQueue::get_instance()->push_back(notifications);
    clear();
}

void EventCollector::clear() {

    m_event_pool.clear();
}
