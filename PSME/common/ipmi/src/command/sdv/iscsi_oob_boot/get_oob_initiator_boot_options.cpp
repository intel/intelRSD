/*!
 * @brief Get OOB Initiator Boot Options Command Implementation.
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
 * @file get_oob_initiator_boot_option.cpp
 */

#include "ipmi/command/sdv/iscsi_oob_boot/get_oob_initiator_boot_options.hpp"



using namespace ipmi;
using namespace ipmi::command::sdv;


request::GetOobInitiatorBootOptions::GetOobInitiatorBootOptions(uint8_t index) :
    request::GetOobBootOptions(request::GetOobBootOptions::ParameterType::INITIATOR, index) {}


request::GetOobInitiatorBootOptions::~GetOobInitiatorBootOptions() {}


void request::GetOobInitiatorBootOptions::pack(std::vector<uint8_t>& data) const {

    data.push_back(get_parameter_id());
    data.push_back(get_parameter_index());
}


response::GetOobInitiatorBootOptions::GetOobInitiatorBootOptions() :
    response::GetOobBootOptions() {}


response::GetOobInitiatorBootOptions::~GetOobInitiatorBootOptions() {}


void response::GetOobInitiatorBootOptions::unpack(const std::vector<uint8_t>& data) {

    if (!valid_basic_parameter(data)) {
        return;
    }

    set_name_offset(pull_uint16(data, OFFSET_NAME));
    set_name_length(pull_uint16(data, OFFSET_NAME_LEN));
    set_wait_time(pull_uint16(data, OFFSET_WAIT_TIME));
    set_retry_count(pull_uint16(data, OFFSET_RETRY_COUNT));
}
