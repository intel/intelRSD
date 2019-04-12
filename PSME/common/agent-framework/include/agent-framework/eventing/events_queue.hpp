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
 * @file evens_queue.hpp
 * */

#pragma once

#include "generic/threadsafe_queue.hpp"
#include "agent-framework/generic/singleton.hpp"
#include "agent-framework/module/requests/psme/component_notification.hpp"

namespace agent_framework {
namespace eventing {

class EventsQueue
    : public ::generic::ThreadSafeQueue<agent_framework::model::requests::ComponentNotification>,
        public agent_framework::generic::Singleton<EventsQueue> {

public:
    using ComponentNotification = agent_framework::model::requests::ComponentNotification;
    using EventData = agent_framework::model::attribute::EventData;
    using EventDataVector = agent_framework::model::attribute::EventData::Vector;

    virtual ~EventsQueue();

    void push_back(ComponentNotification notification) {
        ::generic::ThreadSafeQueue<agent_framework::model::requests::ComponentNotification>::push_back(std::move(notification));
    }

    void push_back(EventDataVector events) {
        ComponentNotification notification{};
        notification.set_notifications(std::move(events));
        ::generic::ThreadSafeQueue<ComponentNotification>::push_back(std::move(notification));
    }

    void push_back(EventData event) {
        ComponentNotification notification{};
        notification.set_notifications({std::move(event)});
        ::generic::ThreadSafeQueue<ComponentNotification>::push_back(std::move(notification));
    }

};

}
}

