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
 * @file utils.cpp
 *
 * @brief Netlink utilities.
 * */

#include "netlink/socket.hpp"
#include "netlink/utils.hpp"

using namespace netlink_base;

std::string netlink_base::utils::get_neighbor_mac_address(const std::string& port_identifier) {
    Socket socket{};
    Message message{port_identifier};
    /* connect to the netlink */
    socket.connect();
    message.set_type(RTM_GETNEIGH);
    message.set_flags(NLM_F_DUMP | NLM_F_REQUEST);
    socket.modify_callback(Message::read_neighbour, &message);
    socket.send_rtnl_message(message);
    socket.receive_to_callback(message);
    return message.get_address();
}
