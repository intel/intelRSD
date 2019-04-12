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

#include <thread>

using namespace ssdp;

using testing::Return;
using testing::Throw;
using testing::_;

namespace ssdp {

static const std::string PAYLOAD = "PAYLOAD";

class MessageTest : public ::testing::Test {
protected:
    MessageTest() {}

    ~MessageTest();

    net::MockMulticastSocket create_mcast_socket(size_t payload_size) {
        auto sock_impl = std::unique_ptr<net::MockDatagramSocketImpl>(new net::MockDatagramSocketImpl);
        EXPECT_CALL(*sock_impl, send_to(_, payload_size, sock_address, 0))
                .WillOnce(Return(PAYLOAD.size()));
        return net::MockMulticastSocket(std::move(sock_impl));
    }

    net::MockMulticastSocket create_mcast_socket_send_throws(int error_code) {
        auto sock_impl = std::unique_ptr<net::MockDatagramSocketImpl>(new net::MockDatagramSocketImpl);
        EXPECT_CALL(*sock_impl, send_to(_, PAYLOAD.size(), sock_address, 0))
                .WillOnce(Throw(net::NetException("NetException", error_code)));
        return net::MockMulticastSocket(std::move(sock_impl));
    }

    net::MockMulticastSocket create_mcast_socket_waitready() {
        auto sock_impl = std::unique_ptr<net::MockDatagramSocketImpl>(new net::MockDatagramSocketImpl);
        EXPECT_CALL(*sock_impl, send_to(_, PAYLOAD.size(), sock_address, 0))
                .WillOnce(Throw(net::NetException("NetException", EWOULDBLOCK)))
                .WillOnce(Return(PAYLOAD.size()));
        return net::MockMulticastSocket(std::move(sock_impl));
    }

    std::shared_ptr<const std::string> get_payload() const {
        return std::make_shared<const std::string>(PAYLOAD);
    }

    SocketAddress sock_address{};
    Socket::List writable_list{};
};

}

MessageTest::~MessageTest() {}

TEST_F(MessageTest, Constructor) {
    net::MockMulticastSocket mcast(
        std::unique_ptr<net::MockDatagramSocketImpl>(new net::MockDatagramSocketImpl));
    ScheduledMessage msg(mcast, sock_address, get_payload());
    ASSERT_EQ(ScheduledMessage::State::SCHEDULED, msg.get_state());
    ASSERT_EQ(msg.get_sender().impl(), mcast.impl());
    ASSERT_TRUE(msg.is_scheduled());
    ASSERT_FALSE(msg.is_ready_to_send(writable_list));
}

TEST_F(MessageTest, UpdateState) {
    net::MockMulticastSocket mcast(
        std::unique_ptr<net::MockDatagramSocketImpl>(new net::MockDatagramSocketImpl));
    ScheduledMessage msg(mcast, sock_address, get_payload());
    msg.update_state(ScheduledMessage::State::READY_TO_SEND);
    ASSERT_EQ(ScheduledMessage::State::READY_TO_SEND, msg.get_state());
    ASSERT_FALSE(msg.is_scheduled());
    ASSERT_TRUE(msg.is_ready_to_send(writable_list));
}

TEST_F(MessageTest, TrySendSends) {
    ScheduledMessage msg(create_mcast_socket(PAYLOAD.size()), sock_address, get_payload());
    ASSERT_EQ(PAYLOAD.size(), msg.try_send());
}

TEST_F(MessageTest, TrySendLogWarningSendTruncated) {
    const auto more_payload = std::make_shared<const std::string>(PAYLOAD + "ab");
    ScheduledMessage msg(create_mcast_socket(more_payload->size()), sock_address, more_payload);
    ASSERT_EQ(PAYLOAD.size(), msg.try_send());
}

TEST_F(MessageTest, TrySendPostponedStateSendnow) {
    ScheduledMessage msg(create_mcast_socket_send_throws(EINTR), sock_address, get_payload());
    ASSERT_EQ(0, msg.try_send());
    ASSERT_EQ(ScheduledMessage::State::READY_TO_SEND, msg.get_state());
}

TEST_F(MessageTest, TrySendPostponedStateWaitReady) {
    ScheduledMessage msg(create_mcast_socket_send_throws(EWOULDBLOCK), sock_address, get_payload());
    ASSERT_EQ(0, msg.try_send());
    ASSERT_EQ(ScheduledMessage::State::WAIT_READY, msg.get_state());

    ScheduledMessage msg2(create_mcast_socket_send_throws(EAGAIN), sock_address, get_payload());
    ASSERT_EQ(0, msg2.try_send());
    ASSERT_EQ(ScheduledMessage::State::WAIT_READY, msg.get_state());
}

TEST_F(MessageTest, TrySendUnknownError) {
    ScheduledMessage msg(create_mcast_socket_send_throws(0), sock_address, get_payload());
    ASSERT_GT(0, msg.try_send());
    ASSERT_EQ(ScheduledMessage::State::SCHEDULED, msg.get_state());
}

TEST_F(MessageTest, SenderWriteReady) {
    auto sock = create_mcast_socket_waitready();
    ScheduledMessage msg(sock, sock_address, get_payload());
    ASSERT_EQ(0, msg.try_send());
    ASSERT_EQ(ScheduledMessage::State::WAIT_READY, msg.get_state());
    writable_list.emplace_back(sock);
    ASSERT_TRUE(msg.is_ready_to_send(writable_list));
    ASSERT_EQ(PAYLOAD.size(), msg.try_send());
    ASSERT_EQ(ScheduledMessage::State::WAIT_READY, msg.get_state());
}
