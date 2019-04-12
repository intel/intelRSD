/*!
 * @brief Get OOB NIC Boot Options Command Implementation.
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
 * @file get_oob_nic_boot_option.cpp
 */

#include "ipmi/command/sdv/iscsi_oob_boot/get_oob_nic_boot_options.hpp"



using namespace ipmi;
using namespace ipmi::command::sdv;


request::GetOobNicBootOptions::GetOobNicBootOptions(uint8_t index) :
    request::GetOobBootOptions(request::GetOobBootOptions::ParameterType::NIC, index) {}


request::GetOobNicBootOptions::~GetOobNicBootOptions() {}


void request::GetOobNicBootOptions::pack(std::vector<uint8_t>& data) const {

    data.push_back(get_parameter_id());
    data.push_back(get_parameter_index());
}


response::GetOobNicBootOptions::GetOobNicBootOptions() :
    response::GetOobBootOptions() {}


response::GetOobNicBootOptions::~GetOobNicBootOptions() {}


void response::GetOobNicBootOptions::unpack(const std::vector<uint8_t>& data) {

    if (!valid_basic_parameter(data)) {
        return;
    }

    set_dhcp_enabled(0 != data[OFFSET_DHCP_ENABLED]);
    set_dhcp_version(IPVersion(data[OFFSET_DHCP_VERSION]));
    set_address(pull_ip(data, OFFSET_IP_ADDRESS));
    set_subnet(pull_ip(data, OFFSET_SUBNET));
    set_gateway(pull_ip(data, OFFSET_GATEWAY));
    set_secondary_dns(pull_ip(data, OFFSET_SECONDARY_DNS));
    set_hostname_offset(pull_uint16(data, OFFSET_HOSTNAME));
    set_hostname_length(pull_uint16(data, OFFSET_HOSTNAME_LEN));
    set_vlan_enabled(0 != data[OFFSET_VLAN_ENABLED]);
    set_vlan_id(pull_uint16(data, OFFSET_VLAN_ID));
    set_mac_address(pull_mac(data, OFFSET_MAC_ADDRESS));
}
