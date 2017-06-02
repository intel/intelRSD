/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016-2017 Intel Corporation
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

#include "netlink/utils.hpp"
#include "netlink/fdb_info_message.hpp"

using namespace netlink_base;
using namespace std;

string netlink_base::utils::get_neighbor_mac_address(const string& port_identifier) {
    FdbInfoMessage msg{port_identifier};
    string address{};

    msg.send();
    if (!msg.get_entries().empty()) {
        address = msg.get_entries()[0].get_mac();
    }
    return address;
}
