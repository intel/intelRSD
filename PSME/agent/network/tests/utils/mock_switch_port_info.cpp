/*!
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
 * */

#include "hal/switch_port_info_impl.hpp"

#include <vector>
#include <string>

using namespace agent::network::hal;
using namespace std;

SwitchPortInfoImpl::~SwitchPortInfoImpl() {}

SwitchPortInfoImpl::SwitchPortInfoImpl(const string& port) :
    m_port_identifier{port} {
    set_is_present(true);
}

SwitchPortInfo::VlanInfoList SwitchPortInfoImpl::get_vlans() const {
    SwitchPortInfo::VlanInfoList list{{3000, false}, {4000, true}};
    return list;
}

bool SwitchPortInfoImpl::is_member() const {
    if (m_port_identifier == "sw0p3") {
        return true;
    }
    return false;
}

SwitchPortInfo::PortList SwitchPortInfoImpl::get_port_members() const {
    SwitchPortInfo::PortList list{"sw0p1"};
    return list;
}

SwitchPortInfo::PortIdentifier SwitchPortInfoImpl::get_master_port() const {
    return "team1";
}

void SwitchPortInfoImpl::get_switch_port_attribute(SwitchPortInfo::PortAttributeType,
                                               SwitchPortInfo::PortAttributeValue& value) {
    value.set(uint32_t(100));
}
