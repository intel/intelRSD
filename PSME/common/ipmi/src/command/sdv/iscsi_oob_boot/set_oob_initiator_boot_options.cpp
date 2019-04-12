/*!
 * @brief Set OOB Initiator Boot Options Command Implementation.
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
 * @file set_oob_initiator_boot_option.cpp
 */

#include "ipmi/command/sdv/iscsi_oob_boot/set_oob_initiator_boot_options.hpp"



using namespace ipmi;
using namespace ipmi::command::sdv;

constexpr std::uint8_t request::SetOobInitiatorBootOptions::INITIATOR_PARAMETER_INDEX;


request::SetOobInitiatorBootOptions::SetOobInitiatorBootOptions() :
    request::SetOobBootOptions(request::SetOobBootOptions::ParameterType::INITIATOR) {

    set_wait_time(GRANTLEY_DEFAULT_WAIT_TIME);
    set_retry_count(DEFAULT_RETRY_COUNT);
}


request::SetOobInitiatorBootOptions::~SetOobInitiatorBootOptions() {}


void request::SetOobInitiatorBootOptions::pack(std::vector<uint8_t>& data) const {

    data.push_back(uint8_t(get_parameter_id()));
    data.push_back(PARAMETER_VERSION);
    data.push_back(INITIATOR_PARAMETER_INDEX);
    push_uint16(data, m_offset);
    push_uint16(data, m_length);
    push_uint16(data, m_wait_time);
    push_uint16(data, m_retry_count);
}


response::SetOobInitiatorBootOptions::SetOobInitiatorBootOptions() :
    response::SetOobBootOptions() {}


response::SetOobInitiatorBootOptions::~SetOobInitiatorBootOptions() {}


void response::SetOobInitiatorBootOptions::unpack(const std::vector<uint8_t>&) { }
