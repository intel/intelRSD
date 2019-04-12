/*!
 * @brief Set OOB NIC Boot Options Command Implementation.
 *
 * @copyright Copyright (c) 2016-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file set_oob_nic_boot_option.cpp
 */

#include "ipmi/command/sdv/iscsi_oob_boot/set_oob_nic_boot_options.hpp"



using namespace ipmi;
using namespace ipmi::command::sdv;

constexpr std::uint8_t request::SetOobNicBootOptions::NIC_PARAMETER_INDEX;


request::SetOobNicBootOptions::SetOobNicBootOptions() :
    request::SetOobBootOptions(request::SetOobBootOptions::ParameterType::NIC) {
}


request::SetOobNicBootOptions::~SetOobNicBootOptions() {}


void request::SetOobNicBootOptions::pack(std::vector<uint8_t>& data) const {

    data.push_back(uint8_t(get_parameter_id()));
    data.push_back(PARAMETER_VERSION);
    data.push_back(NIC_PARAMETER_INDEX);
    data.push_back(m_dhcp_enabled ? 1 : 0);
    data.push_back(uint8_t(m_dhcp_ip_version));
    push_ip(data, m_ip_address);
    push_ip(data, m_subnet_mask);
    push_ip(data, m_gateway);
    push_ip(data, m_primary_dns);
    push_ip(data, m_secondary_dns);
    push_uint16(data, m_name_offset);
    push_uint16(data, m_name_length);
    data.push_back(m_vlan_enabled ? 1 : 0);
    push_uint16(data, m_vlan);
    push_mac(data, m_mac);
}


response::SetOobNicBootOptions::SetOobNicBootOptions() :
    response::SetOobBootOptions() {}


response::SetOobNicBootOptions::~SetOobNicBootOptions() {}


void response::SetOobNicBootOptions::unpack(const std::vector<uint8_t>&) { }
