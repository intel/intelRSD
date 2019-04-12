/*!
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
*/
#include "agent-framework/eventing/event_dispatcher.hpp"
#include "agent-framework/eventing/events_queue.hpp"
#include "agent-framework/module/service_uuid.hpp"
#include "agent-framework/module/requests/psme/component_notification.hpp"

#include <string>

using namespace agent_framework::generic;

namespace {
const size_t QUEUE_WAIT_TIME = 1000;
}


EventDispatcher::EventDispatcher() : m_event_sender{new EventSender()} {}

EventDispatcher::~EventDispatcher() {}

void EventDispatcher::enable_send_notifications(const std::string& url) {
    m_event_sender->enable_send(url);
    log_info("eventing", "Sending AMC notifications enabled.");
}

void EventDispatcher::disable_send_notifications() {
    m_event_sender->disable_send();
    log_info("eventing", "Sending AMC notifications disabled.");
}

bool EventDispatcher::send_notifications_enabled() const {
    return m_event_sender->send_enabled();
}

void EventDispatcher::execute() {
    using agent_framework::eventing::EventsQueue;
    log_info("eventing", "Starting EventDispatcher thread...");

    while (is_running()) {
        auto notification = EventsQueue::get_instance()->wait_for_and_pop(std::chrono::milliseconds(QUEUE_WAIT_TIME));

        if (notification) {
            if (notification->get_notifications().empty()) {
                log_debug("eventing", "Discarding empty notification");
                continue;
            }

            m_event_sender->send_notifications(std::move(*notification));
        }
    }
    log_info("eventing", "EventDispatcher thread stopped.");
}
