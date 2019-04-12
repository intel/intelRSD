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
#include "agent-framework/eventing/event_msg.hpp"
#include "agent-framework/state_machine/module_state.hpp"
using namespace agent_framework::generic;

/*
 * Mock subscriber. Used to test subscription and Publisher.
 */
class MockSubscriber : public Subscriber {
public:
    virtual void notify(const EventMsg& msg);
};
void MockSubscriber::notify(const EventMsg& msg) {
    (void)(msg);
}

/*
 * Try to subscribe object to Publisher. Test if object is subscribed. It should
 * be.
 */
TEST(SubscribeToPublisher_PositiveIsSubscribed, SubscriptionTest) {

    Publisher * publisher = new EventPublisher();
    Subscriber *  subscriber = new MockSubscriber();

    publisher->subscribe(subscriber);

    ASSERT_TRUE(publisher->is_subscribed(subscriber)) ;

    delete subscriber;
    delete publisher;
}

/*
 * Try to subscribe object to Publisher. Test if object is subscribed. It
 * shouldn't be.
 */
TEST(NotSubscribeToPublisher_NegativeIsSubscribed, SubscriptionTest) {

    Publisher * publisher = new EventPublisher();
    Subscriber *  subscriber = new MockSubscriber();

    ASSERT_FALSE(publisher->is_subscribed(subscriber)) ;

    delete subscriber;
    delete publisher;
}

/*
 * Try to subscribe object to Publisher. Try to unsubscribe it. Test if object
 * is subscribed. It shouldn't be.
 */
TEST(UnsubscribeFromPublisher_NegativeIsSubscribed, SubscriptionTest) {

    Publisher * publisher = new EventPublisher();
    Subscriber * subscriber = new MockSubscriber();

    publisher->subscribe(subscriber);
    publisher->unsubscribe(subscriber);

    ASSERT_FALSE(publisher->is_subscribed(subscriber)) ;

    delete subscriber;
    delete publisher;
}

/*
 * Try to subscribe NULL to Publisher. Try to unsubscribe it. Test if object
 * is subscribed. It shouldn't be.
 */
TEST(SubscribeNull_ThrowException, SubscriptionTest) {

    Publisher * publisher = new EventPublisher();

    EXPECT_NO_THROW({publisher->subscribe(nullptr);});
    ASSERT_FALSE(publisher->is_subscribed(nullptr)) ;

    delete publisher;
}

/*
 * Try to unsubscribe NULL from Publisher. Try to unsubscribe it.
 * Should be ignored: No exceptions.
 */
TEST(UnsubscribeNull_NotThrowException, SubscriptionTest) {

    Publisher * publisher = new EventPublisher();

    EXPECT_NO_THROW({publisher->unsubscribe(nullptr);}) ;

    delete publisher;
}

/*
 * Try to unsubscribe Object from Publisher which isn't subscribed.
 *  Should be ignored: No exceptions.
 */
TEST(UnsubscribeNotSubscribed_NotThrowException, SubscriptionTest) {

    Publisher * publisher = new EventPublisher();
    Subscriber * subscriber = new MockSubscriber();

    EXPECT_NO_THROW({publisher->unsubscribe(subscriber);});
    ASSERT_FALSE(publisher->is_subscribed(subscriber));

    delete subscriber;
    delete publisher;
}

/*
 * Try to subscribe multiply times. Try to unsubscribe. Test if it is still
 * subscribed. Should be posible to subscribe to single Publisher only ince.
 */
TEST(SubscribeManyTimesUnsubscribeOnce_NegativeIsSubscribed, SubscriptionTest) {

    Publisher * publisher = new EventPublisher();
    Subscriber * subscriber = new MockSubscriber();

    // Only first subscription should be saved.
    publisher->subscribe(subscriber);
    publisher->subscribe(subscriber);
    publisher->subscribe(subscriber);
    publisher->subscribe(subscriber);
    publisher->subscribe(subscriber);
    publisher->unsubscribe(subscriber);

    ASSERT_FALSE(publisher->is_subscribed(subscriber));

    delete subscriber;
    delete publisher;
}

/*
 * Checks if nullptr is subscribed. Method should return false, and ignore it.
 * Not exception.
 */
TEST(NullIsSubscribed_NegativeIsSubscribed, SubscriptionTest) {

    Publisher * publisher = new EventPublisher();

    EXPECT_NO_THROW({ASSERT_FALSE(publisher->is_subscribed(nullptr));});

    delete publisher;
}
