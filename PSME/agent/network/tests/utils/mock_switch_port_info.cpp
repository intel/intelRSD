/*!
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
 * */

#include "api/netlink/switch_port_info.hpp"

#include <vector>
#include <string>

using namespace agent::network::api::netlink;
using namespace std;

SwitchPortInfo::~SwitchPortInfo() {}

SwitchPortInfo::SwitchPortInfo(const string& port) {
    m_port_identifier = port;
}

SwitchPortInfo::VlanInfoList SwitchPortInfo::get_vlans() const {
    SwitchPortInfo::VlanInfoList list{{3000, false}, {4000, true}};
    return list;
}

bool SwitchPortInfo::is_member() const {
    if (m_port_identifier == "sw0p3") {
        return true;
    }
    return false;
}

SwitchPortInfo::PortList SwitchPortInfo::get_port_members() const {
    SwitchPortInfo::PortList list{"sw0p1"};
    return list;
}

SwitchPortInfo::PortIdentifier SwitchPortInfo::get_master_port() const {
    return "team1";
}

void SwitchPortInfo::get_switch_port_attribute(SwitchPortInfo::PortAttributeType,
                                               SwitchPortInfo::PortAttributeValue& value) {
    value.set(uint32_t(100));
}
