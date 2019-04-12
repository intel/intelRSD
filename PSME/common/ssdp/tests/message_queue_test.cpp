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

#include "ssdp/message_queue.hpp"
#include "multicast_socket_mock.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace ssdp;

using testing::Return;
using testing::_;
using testing::Throw;

namespace ssdp {

static const std::string PAYLOAD = "PAYLOAD";

template<class M>
class MessageQueueTest : public ::testing::Test {
protected:
    using Ms = typename MessageQueue<M>::Ms;
    using Timepoint = typename MessageQueue<M>::Timepoint;

    MessageQueueTest();

    ~MessageQueueTest();

    void SetUp() override {
        now = std::chrono::time_point_cast<Ms>(std::chrono::steady_clock::now());
    }

    typename MessageQueue<M>::Queue& internal_queue() {
        return queue.m_scheduled_msgs;
    }

    MessageQueue<M> queue;
    Socket::List writable_list{};
    Timepoint now;
    SocketAddress sock_address{};
};

template<class M>
MessageQueueTest<M>::MessageQueueTest() {}

template<class M>
MessageQueueTest<M>::~MessageQueueTest() {}

class MessageQueueScheduledMessageTest : public MessageQueueTest<ScheduledMessage> {
protected:
    MessageQueueScheduledMessageTest() {}

    ~MessageQueueScheduledMessageTest();

    net::MockMulticastSocket create_mcast_socket(size_t payload_size) {
        auto sock_impl = std::unique_ptr<net::MockDatagramSocketImpl>(new net::MockDatagramSocketImpl);
        EXPECT_CALL(*sock_impl, send_to(_, payload_size, sock_address, 0))
                .WillOnce(Return(PAYLOAD.size()))
                .WillOnce(Return(PAYLOAD.size()));
        return net::MockMulticastSocket(std::move(sock_impl));
    }

    net::MockMulticastSocket create_mcast_socket_waitready() {
        auto sock_impl = std::unique_ptr<net::MockDatagramSocketImpl>(new net::MockDatagramSocketImpl);
        EXPECT_CALL(*sock_impl, send_to(_, PAYLOAD.size(), sock_address, 0))
                .WillOnce(Throw(net::NetException("NetException", EWOULDBLOCK)))
                .WillOnce(Return(PAYLOAD.size()))
                .WillOnce(Return(PAYLOAD.size()));
        return net::MockMulticastSocket(std::move(sock_impl));
    }
};

MessageQueueScheduledMessageTest::~MessageQueueScheduledMessageTest() {}

}

TEST_F(MessageQueueScheduledMessageTest, SendReadyMessages) {
    ASSERT_TRUE(internal_queue().empty());
    auto mcast = create_mcast_socket(PAYLOAD.size());
    ScheduledMessage msg1(mcast, sock_address, std::make_shared<const std::string>(PAYLOAD));
    auto mcast_waitready = create_mcast_socket_waitready();
    ScheduledMessage msg2(mcast_waitready, sock_address, std::make_shared<const std::string>(PAYLOAD));
    // enqueue message twice with different dispatch time (usual use case)
    queue.send_or_enqueue(msg1, Ms(1));
    queue.send_or_enqueue(msg1, Ms(10));

    // enqueue message for which first send interrupts, second succeeds
    queue.send_or_enqueue(msg2, Ms(3));
    queue.send_or_enqueue(msg2, Ms(13));

    ASSERT_EQ(4, internal_queue().size());
    // set now to be after two first messages in queue
    now = internal_queue().cbegin()->first + Ms(5);
    auto timeout = Ms(10000);
    queue.select_ready_messages(writable_list, now, timeout);
    // zero timeout indicates we have message(s) to be send now
    ASSERT_EQ(Timepoint::duration::zero(), timeout);

    // send messages
    queue.send_ready_messages(writable_list);
    ASSERT_EQ(3, internal_queue().size());
    auto it = internal_queue().cbegin();
    ASSERT_EQ(ScheduledMessage::State::WAIT_READY, it->second.get_state());
    ++it;
    ASSERT_EQ(ScheduledMessage::State::SCHEDULED, it->second.get_state());
    ++it;
    ASSERT_EQ(ScheduledMessage::State::SCHEDULED, it->second.get_state());

    // repeat select, WAIT_READY message should be in writable_list
    queue.select_ready_messages(writable_list, now, timeout);
    ASSERT_EQ(1, writable_list.size());
    ASSERT_EQ(mcast_waitready, *writable_list.begin());

    // repeat send
    queue.send_ready_messages(writable_list);
    ASSERT_EQ(2, internal_queue().size());
    it = internal_queue().cbegin();
    ASSERT_EQ(ScheduledMessage::State::SCHEDULED, it->second.get_state());
    ++it;
    ASSERT_EQ(ScheduledMessage::State::SCHEDULED, it->second.get_state());

    // send remaining messages
    now = internal_queue().cbegin()->first + Ms(5);
    queue.select_ready_messages(writable_list, now, timeout);
    queue.send_ready_messages(writable_list);
    ASSERT_TRUE(internal_queue().empty());
}
