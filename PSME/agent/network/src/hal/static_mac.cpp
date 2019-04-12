/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 */

#include "hal/static_mac.hpp"
#include "tools/netlink/static_mac_message.hpp"
#include "netlink/fdb_info_message.hpp"
#include "logger/logger_factory.hpp"

extern "C" {
#include <netinet/ether.h>
}

using namespace agent::network::hal;
using namespace agent::network::tools::netlink;
using namespace netlink_base;
using namespace std;

StaticMac::StaticMac() {}
StaticMac::~StaticMac() {}

void StaticMac::add(const string& port, const string& address,
                    uint16_t vlan_id) {
    StaticMacMessage msg(true, port, address, vlan_id);
    msg.send();
}

void StaticMac::remove(const string& port, const string& address,
                       uint16_t vlan_id) {
    StaticMacMessage msg(false, port, address, vlan_id);
    msg.send();
}

void StaticMac::get_macs(const string& port, Table& macs) {
    FdbInfoMessage msg{port};
    msg.send();
    for (const auto& entry : msg.get_entries()) {
        if (FdbInfoMessage::EntryType::STATIC == entry.get_type()) {
            macs.insert(make_pair(entry.get_mac(), entry.get_vlan()));
        }
    }
}
