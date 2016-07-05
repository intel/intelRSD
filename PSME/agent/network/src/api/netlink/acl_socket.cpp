/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016 Intel Corporation
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
 *
 * @file acl_socket.cpp
 *
 * @brief ACL netlink socket implementation.
 * */

#include "api/netlink/acl_socket.hpp"

extern "C" {
#include <acl_nl_cmds.h>
}

using namespace agent::network::api::netlink;
using namespace netlink_base;
using namespace std;

void AclSocket::connect() {
    if (!m_sock) {
        throw runtime_error("ACL: null socket");
    }

    int status = nl_connect(m_sock.get(), NETLINK_USERSOCK);
    if (status) {
        throw runtime_error(string("ACL: nl_connect failed ") +
                            to_string(status));
    }
    nl_socket_disable_auto_ack(m_sock.get());
    nl_socket_disable_seq_check(m_sock.get());
    nl_socket_set_peer_port(m_sock.get(), ACL_TOOL_SOCKET_PORT);
    status = nl_socket_set_msg_buf_size(m_sock.get(), ACL_NL_MSG_SIZE_BIG);
    if (status) {
        throw runtime_error(string("ACL: nl_socket_set_msg_buf_size failed ") +
                           to_string(status));
    }
    status = nl_socket_set_buffer_size(m_sock.get(), ACL_NL_MSG_SIZE_BIG,
                                       ACL_NL_MSG_SIZE_BIG);
    if (status) {
        throw runtime_error(string("ACL: nl_socket_set_buffer_size failed ") +
                           to_string(status));
    }
}
