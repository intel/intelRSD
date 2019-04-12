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
 * */

#include "agent-framework/eventing/events_queue.hpp"
#include "agent-framework/module/model/attributes/event_data.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace agent_framework::eventing;

/* Positive. */

TEST(EventsQueueTest, PositiveEmptyQueue) {
    auto msg = EventsQueue::get_instance()->try_pop();
    ASSERT_EQ(msg.get(), nullptr);
}

TEST(EventsQueueTest, PositiveQueuePushPop) {
    agent_framework::model::attribute::EventData event_data{};
    event_data.set_component("TestComponentUUID");
    event_data.set_parent("TestParentUUID");
    event_data.set_type(agent_framework::model::enums::Component::Chassis);
    event_data.set_notification(agent_framework::model::enums::Notification::Update);

    EventsQueue::get_instance()->push_back(event_data);

    auto out_msg = EventsQueue::get_instance()->try_pop();

    ASSERT_NE(out_msg.get(), nullptr);
    ASSERT_EQ(out_msg->get_notifications().at(0).get_component(), "TestComponentUUID");
}
