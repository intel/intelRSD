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
 *
 * @file message_queue.hpp
 *
 * @brief MessageQueue declaration
 * */

#pragma once

#include "net/multicast_socket.hpp"
#include "net/socket_address.hpp"

#include <memory>
#include <chrono>
#include <map>

namespace ssdp {

using net::Socket;
using net::MulticastSocket;
using net::SocketAddress;

/*!
 * Message to be queued.
 */
class ScheduledMessage {
public:
    /*!
     * State of message.
     */
    enum class State {
        READY_TO_SEND = 0,
        WAIT_READY,
        SCHEDULED
    };

    /*!
     * Constructor.
     * param[in] sender Socket used to send message
     * param[in] receiver Message destination
     * param[in] payload Message payload
     */
    ScheduledMessage(const MulticastSocket& sender,
                 const SocketAddress& receiver,
                 std::shared_ptr<const std::string> payload)
            : m_sender(sender), m_receiver(receiver), m_payload(payload) {}

    /*!
     * Message State getter.
     * @return Current message State.
     */
    State get_state() const {
        return m_state;
    }

    /*!
     * Updates message State.
     * @param[in] state State to be set.
     */
    void update_state(const State& state) {
        m_state = state;
    }

    /*!
     * Sending socket getter
     * @return Socket used to send message
     */
    const MulticastSocket& get_sender() const {
        return m_sender;
    }

    /*!
     * Tests if message is ready to be send.
     * @param[in] writable Collection of sockets ready for write.
     * @return true if message is ready to be send, false otherwise.
     */
    bool is_ready_to_send(const Socket::List& writable) const;

    /*!
     * Tests SCHEDULED state of message.
     * @return true if message is in SCHEDULED state, false otherwise.
     */
    bool is_scheduled() const {
        return ScheduledMessage::State::SCHEDULED == m_state;
    }

    /*
     * Tries to send a message.
     * @return > 0 - message send (returns number of send bytes)
     *           0 - send postponed
     *          -1 - some error occurred
     */
    long try_send();

private:
    State m_state{State::SCHEDULED};
    MulticastSocket m_sender;
    SocketAddress m_receiver;
    std::shared_ptr<const std::string> m_payload{};
};

/*!
 * Queue containing messages scheduled for dispatch.
 * Messages are sorted by time of dispatch.
 * At beginning are messages to be send first.
 */
template<class Message>
class MessageQueue {
public:
    using Ms = std::chrono::milliseconds;
    using Timepoint = std::chrono::time_point<std::chrono::steady_clock, Ms>;

    /*
     * Sends or enqueues (if delay > 0) a message.
     * @param[in] msg Message to be send or enqueued.
     * @param[in] delay Dispatch delay value.
     */
    void send_or_enqueue(Message msg,
                    Timepoint::duration delay = Timepoint::duration::zero());

    /*!
     * Sends all messages ready to be send.
     * Message is ready if it's time to send arrived (is in READY_TO_SEND state)
     * or its connected socket will not block (is in WAIT_READY state).
     * @param[in] writable List of sockets ready for write.
     */
    void send_ready_messages(const Socket::List& writable);

    /*!
     * Updates state of messages ready to be send. Updates timeout value.
     * If time to send a message came, message state is updated from SCHEDULED to READY_TO_SEND.
     * Message in WAIT_READY state adds sending socket to writable list.
     * @param[in,out] writable List of sockets waiting for write ready
     * @param[in] now Current time
     * @param[in,out] timeout Updated to std::min(std::max(zero, (first_scheduled_timepoint - now)), timeout)
     */
    void select_ready_messages(Socket::List& writable,
                               const Timepoint& now,
                               Timepoint::duration& timeout);
private:
    template<class T>
    friend class MessageQueueTest;
    using Queue = std::multimap<Timepoint, Message>;
    Queue m_scheduled_msgs{};
};

template<class Message>
void MessageQueue<Message>::send_or_enqueue(Message msg, Timepoint::duration delay) {
    if (delay == Timepoint::duration::zero()) {
        if (msg.try_send()) {
            // message send or error logged
            return;
        }
    }
    auto timepoint = std::chrono::time_point_cast<Ms>(std::chrono::steady_clock::now())
                     + delay;
    m_scheduled_msgs.emplace(timepoint, std::move(msg));
}

template<class Message>
void MessageQueue<Message>::select_ready_messages(Socket::List& writable,
                                    const Timepoint& now,
                                    Timepoint::duration& timeout) {
    if (m_scheduled_msgs.empty()) {
        return;
    }

    auto it = m_scheduled_msgs.begin();
    auto end = m_scheduled_msgs.upper_bound(now);

    // timeout = std::min(std::max(zero, (first_scheduled_timepoint - now)), timeout)
    if (it != end) {
        // we have messages to send now
        timeout = Timepoint::duration::zero();
    }
    else {
        auto diff = end->first - now;
        if (timeout > diff) {
            timeout = diff;
        }
    }

    // update messages state
    for (; it != end; ++it) {
        // previous send resulted in EAGAIN/EWOULDBLOCK
        if (Message::State::WAIT_READY == it->second.get_state()) {
            writable.emplace_back(it->second.get_sender());
        }
        else {
            it->second.update_state(Message::State::READY_TO_SEND);
        }
    }
}

template<class Message>
void MessageQueue<Message>::send_ready_messages(const Socket::List& writable) {
    auto it = m_scheduled_msgs.begin();
    auto end = m_scheduled_msgs.end();

    while (it != end) {
        auto& msg = it->second;
        if (msg.is_ready_to_send(writable)) {
            if (msg.try_send()) {
                it = m_scheduled_msgs.erase(it);
            }
            else {
                // send postponed, skip message and proceed with next one
                ++it;
            }
        }
        else if (msg.is_scheduled()) {
            // messages are sorted by time, when we encounter
            // one in SCHEDULED state, we can stop iteration
            break;
        }
        else {
            ++it;
        }
    }
}

}
