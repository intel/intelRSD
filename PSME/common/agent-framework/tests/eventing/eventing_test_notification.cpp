/*!
 * @section LICENSE
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
 * @section DESCRIPTION
 * */

#include <exception>
#include "gtest/gtest.h"
#include "agent-framework/eventing/event_publisher.hpp"
#include "agent-framework/eventing/subscriber.hpp"
#include "agent-framework/eventing/event_msg.hpp"
#include "agent-framework/state_machine/module_state.hpp"
#include "agent-framework/state_machine/state_machine_transition.hpp"

using namespace agent_framework::state_machine;
using namespace agent_framework::generic;

#define UNUSED(x) (void)(x)
/*
 * Mock subscriber. Used to test notification and Publisher.
 */
class MockSubscriber : public Subscriber {
private:
    int * value;
public:
    MockSubscriber(): value(nullptr) {}
    MockSubscriber(const MockSubscriber& other) : value(nullptr) { UNUSED(other);  }
    MockSubscriber& operator=(const MockSubscriber& other){
        UNUSED(other);
        return *this;
    }

    void set_value(int * pvalue) { value = pvalue; }
    virtual void notify(const EventMsg& msg);
};
void MockSubscriber::notify(const EventMsg& msg) {
    UNUSED(msg);
    if(nullptr != value) {
        *value = *value + 1;
    }
}

TEST(NotifyWithOneSubscriber_ValueEqualToOne, NotificationTest) {

    int value = 0;
    auto publisher  = new EventPublisher();
    auto subscriber =  new MockSubscriber();

    // Set pointer to value on which it will do calculations.
    subscriber->set_value(&value);

    publisher->subscribe(subscriber);
    publisher->notify_all(EventMsg{"test",
                                    ModuleState::State::ENABLED,
                                    StateMachineTransition::Transition::IDLE});
    // After notify all value should be incremented.
    ASSERT_EQ(value, 1);

    delete subscriber;
    delete publisher;
}

TEST(NotifyWithZeroSubscriber_ValueEqualToZero, NotificationTest) {

    int value = 0;
    auto publisher  = new EventPublisher();
    auto subscriber =  new MockSubscriber();

    // Set pointer to value on which it will do calculations.
    subscriber->set_value(&value);
    // DO NOT ADD Subscriber.
    publisher->notify_all(EventMsg{"test",
                                    ModuleState::State::ENABLED,
                                    StateMachineTransition::Transition::IDLE});
    // After notify all value should be still 0.
    ASSERT_EQ(value, 0);

    delete subscriber;
    delete publisher;
}

TEST(SubscribeNotifyAllUnsubscribeNotifyAll_ValueEqualToOne, NotificationTest) {

    int value = 0;
    auto publisher  = new EventPublisher();
    auto subscriber = new MockSubscriber();

    // Set pointer to value on which it will do calculations.
    subscriber->set_value(&value);

    publisher->subscribe(subscriber);
    publisher->notify_all(EventMsg{"test",
                                    ModuleState::State::ENABLED,
                                    StateMachineTransition::Transition::IDLE});
    publisher->unsubscribe(subscriber);
    publisher->notify_all(EventMsg{"test",
                                    ModuleState::State::ENABLED,
                                    StateMachineTransition::Transition::IDLE});
    // After notify all value should be incremented only once.
    ASSERT_EQ(value, 1);

    delete subscriber;
    delete publisher;
}

TEST(SubscribeThreeNotifyAll_ValueEqualToThree, NotificationTest) {

    int value = 0;
    auto publisher  = new EventPublisher();
    auto subscriber1 =  new MockSubscriber();
    auto subscriber2 =  new MockSubscriber();
    auto subscriber3 =  new MockSubscriber();

    // Set pointer to value on which it will do calculations.
    subscriber1->set_value(&value);
    subscriber2->set_value(&value);
    subscriber3->set_value(&value);

    publisher->subscribe(subscriber1);
    publisher->subscribe(subscriber2);
    publisher->subscribe(subscriber3);
    publisher->notify_all(EventMsg{"test",
                                    ModuleState::State::ENABLED,
                                    StateMachineTransition::Transition::IDLE});

    // After notify all value should be incremented only once.
    ASSERT_EQ(value, 3);

    delete subscriber3;
    delete subscriber2;
    delete subscriber1;
    delete publisher;
}
