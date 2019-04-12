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
 * @file nl_message.cpp
 *
 * @brief Netlink message
 * */

#include "netlink/nl_message.hpp"
#include "netlink/nl_exception.hpp"
#include "netlink/nl_exception_invalid_input.hpp"

#include <netlink/msg.h>

#include <memory>

namespace {
    struct NlMsgDeleter {
        void operator()(struct nl_msg* msg) const {
            nlmsg_free(msg);
        }
    };
    using NlMsg = std::unique_ptr<struct nl_msg, NlMsgDeleter>;
}

using namespace netlink_base;

NlMessage::NlMessage(NlSocket::Protocol proto) : m_socket{proto} {
    /* setup valid message callback */
    if (0 != nl_socket_modify_cb(m_socket.get_sock(), NL_CB_VALID, NL_CB_CUSTOM,
                        NlMessage::valid_message_handler, this)) {
        throw NlException("Failed to setup the valid message cb");
    }
    /* setup finish message callback */
    if (0 != nl_socket_modify_cb(m_socket.get_sock(), NL_CB_FINISH, NL_CB_CUSTOM,
                        NlMessage::finish_message_handler, nullptr)) {
        throw NlException("Failed to setup the finish message cb");
    }
    /* setup error message callback */
    if (0 != nl_socket_modify_err_cb(m_socket.get_sock(), NL_CB_CUSTOM,
                        NlMessage::error_message_handler, this)) {
        throw NlException("Failed to setup the error message cb");
    }
}

int NlMessage::valid_message_handler(struct nl_msg* msg, void* arg) {
    NlMessage* message = static_cast<NlMessage*>(arg);
    try {
        /* handle the message */
        message->process_message(msg);
    }
    catch (...) {
        /* Netlink callbacks must always return error code, so
         * lets store the exception, return the value and
         * notify the caller about it by negative (NLE_MAX + 1) value */
        message->m_handler_exception = std::current_exception();
        return -(NLE_MAX + 1);
    }
    /* return success */
    return NL_OK;
}

int NlMessage::finish_message_handler(struct nl_msg*, void*) {
    /* skip Netlink DONE message */
    return NL_SKIP;
}

int NlMessage::error_message_handler(struct sockaddr_nl*,
        struct nlmsgerr*, void*) {
    return NL_STOP;
}

void NlMessage::send() {
    int nl_error{};
    /* allocate new message */
    NlMsg msg{static_cast<NlMsg::pointer>(nlmsg_alloc_simple(m_type, m_flags))};
    if (!msg) {
        throw NlException("Failed to allocate Netlink message");
    }
    /* prepare and send the message */
    prepare_message(msg.get());
    if (0 > (nl_error = nl_send_auto(m_socket.get_sock(), (msg.get())))) {
        throw NlException(nl_geterror(nl_error));
    }
    nl_error = nl_recvmsgs_default(m_socket.get_sock());
    if (NLE_MAX < abs(nl_error)) {
        /* throw process handler exception to caller */
        std::rethrow_exception(m_handler_exception);
    }
    else if (0 > nl_error) {
        /* Netlink error message reported back from kernel */
        switch (abs(nl_error)) {
            case NLE_INVAL:
                /* thru invalid input exception for caller */
                throw NlExceptionInvalidInput();
            default:
                /* thru default netlink exception */
                throw NlException(nl_geterror(nl_error));
        }
    }
}

NlMessage::~NlMessage() { }
