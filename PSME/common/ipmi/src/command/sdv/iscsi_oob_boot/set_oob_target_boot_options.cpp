/*!
 * @brief Set OOB Target Boot Options Command Implementation.
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
 * @file set_oob_target_boot_option.cpp
 */

#include "ipmi/command/sdv/iscsi_oob_boot/set_oob_target_boot_options.hpp"



using namespace ipmi;
using namespace ipmi::command::sdv;

constexpr std::uint8_t request::SetOobTargetBootOptions::TARGET_PARAMETER_INDEX;


request::SetOobTargetBootOptions::SetOobTargetBootOptions() :
    request::SetOobBootOptions(request::SetOobBootOptions::ParameterType::TARGET) {
}


request::SetOobTargetBootOptions::~SetOobTargetBootOptions() {}


void request::SetOobTargetBootOptions::pack(std::vector<uint8_t>& data) const {

    data.push_back(uint8_t(get_parameter_id()));
    data.push_back(PARAMETER_VERSION);
    data.push_back(TARGET_PARAMETER_INDEX);
    push_uint16(data, m_name_offset);
    push_uint16(data, m_name_length);
    data.push_back(m_dhcp_enabled ? 1 : 0);
    push_ip(data, m_ip_address);
    push_uint16(data, m_port);
    push_uint64(data, m_lun);
    data.push_back(uint8_t(m_chap_type));
    push_uint16(data, m_chap_name_offset);
    push_uint16(data, m_chap_name_length);
    push_uint16(data, m_chap_secret_offset);
    push_uint16(data, m_chap_secret_length);
    push_uint16(data, m_mutual_chap_name_offset);
    push_uint16(data, m_mutual_chap_name_length);
    push_uint16(data, m_mutual_chap_secret_offset);
    push_uint16(data, m_mutual_chap_secret_length);
}


response::SetOobTargetBootOptions::SetOobTargetBootOptions() :
    response::SetOobBootOptions() {}


response::SetOobTargetBootOptions::~SetOobTargetBootOptions() {}


void response::SetOobTargetBootOptions::unpack(const std::vector<uint8_t>&) { }
