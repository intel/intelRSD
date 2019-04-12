/*!
 * @brief Get OOB Target Boot Options Command Implementation.
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
 * @file get_oob_target_boot_option.cpp
 */

#include "ipmi/command/sdv/iscsi_oob_boot/get_oob_target_boot_options.hpp"



using namespace ipmi;
using namespace ipmi::command::sdv;


request::GetOobTargetBootOptions::GetOobTargetBootOptions(uint8_t index) :
    request::GetOobBootOptions(request::GetOobBootOptions::ParameterType::TARGET, index) {}


request::GetOobTargetBootOptions::~GetOobTargetBootOptions() {}


void request::GetOobTargetBootOptions::pack(std::vector<uint8_t>& data) const {

    data.push_back(get_parameter_id());
    data.push_back(get_parameter_index());
}


response::GetOobTargetBootOptions::GetOobTargetBootOptions() :
    response::GetOobBootOptions() {}


response::GetOobTargetBootOptions::~GetOobTargetBootOptions() {}


void response::GetOobTargetBootOptions::unpack(const std::vector<uint8_t>& data) {
    if (!valid_basic_parameter(data)) {
        return;
    }

    set_target_name_offset(pull_uint16(data, OFFSET_TARGET_NAME));
    set_target_name_length(pull_uint16(data, OFFSET_TARGET_NAME_LEN));
    set_dhcp_enabled(0 != data[OFFSET_DHCP_ENABLED]);
    set_address(pull_ip(data, OFFSET_IP_ADDRESS));
    set_port(pull_uint16(data, OFFSET_PORT));
    set_lun(pull_uint64(data, OFFSET_LUN));
    set_chap_type(ChapType(data[OFFSET_CHAP_TYPE]));
    set_chapname_offset(pull_uint16(data, OFFSET_CHAPNAME));
    set_chapname_length(pull_uint16(data, OFFSET_CHAPNAME_LEN));
    set_chapsecret_offset(pull_uint16(data, OFFSET_CHAPSECRET));
    set_chapsecret_length(pull_uint16(data, OFFSET_CHAPSECRET_LEN));
    set_mutual_chapname_offset(pull_uint16(data, OFFSET_MUTUAL_CHAPNAME));
    set_mutual_chapname_length(pull_uint16(data, OFFSET_MUTUAL_CHAPNAME_LEN));
    set_mutual_chapsecret_offset(pull_uint16(data, OFFSET_MUTUAL_CHAPSECRET));
    set_mutual_chapsecret_length(pull_uint16(data, OFFSET_MUTUAL_CHAPSECRET_LEN));
}
