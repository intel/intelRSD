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
 * @file message_queue.cpp
 *
 * @brief MessageQueue implementation.
 * */

#include "ssdp/message_queue.hpp"

#include "logger/logger_factory.hpp"

#include <algorithm>

using namespace ssdp;

bool ScheduledMessage::is_ready_to_send(const Socket::List& writable) const {
    return ScheduledMessage::State::READY_TO_SEND == m_state
            ||
            (ScheduledMessage::State::WAIT_READY == m_state
             && writable.cend() != std::find(writable.cbegin(),
                                             writable.cend(), m_sender));
}

long ScheduledMessage::try_send() {
    log_debug("ssdp", "try_send to:" << m_receiver
                                  << " len:" << m_payload->size());
    long rc{0};
    try {
        rc = m_sender.send_to(m_payload->data(),
                              m_payload->size(), m_receiver);

        if (m_payload->size() != size_t(rc)) {
            log_warning("ssdp", "send to:" << m_receiver
                << " send " << rc << " bytes out of: " << m_payload->size());
        }
        else {
            log_debug("ssdp", "send OK");
        }
    }
    catch(const net::NetException& e) {
        auto error_code = e.get_code();
        if (EWOULDBLOCK == error_code || EAGAIN == error_code) {
            m_state = ScheduledMessage::State::WAIT_READY;
            rc = 0;
            log_debug("ssdp", "send postponed");
        }
        else if (EINTR == error_code) {
            m_state = ScheduledMessage::State::READY_TO_SEND;
            rc = 0;
            log_debug("ssdp", "send postponed");
        }
        else {
            rc = -1;
            log_error("ssdp", "send to:" << m_receiver
                << " failed: " << e.what());
        }
    }
    return rc;
}
