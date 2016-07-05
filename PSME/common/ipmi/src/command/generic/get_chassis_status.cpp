/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * @file get_chassis_status.cpp
 *
 * @brief IPMI Get Chassis Status Command.
 * */

#include "ipmi/command/generic/get_chassis_status.hpp"
#include "ipmi/command/generic/enums.hpp"

using namespace ipmi;
using namespace ipmi::command::generic;

request::GetChassisStatus::GetChassisStatus():
    Request(uint8_t(NetFn::CHASSIS), uint8_t(Cmd::GET_CHASSIS_STATUS)) {}

request::GetChassisStatus::~GetChassisStatus() {}

void request::GetChassisStatus::pack(vector<uint8_t>& data) const {
    (void)data;
}

response::GetChassisStatus::GetChassisStatus():
    Response(uint8_t(NetFn::CHASSIS), uint8_t(Cmd::GET_CHASSIS_STATUS), RESPONSE_SIZE) {}

response::GetChassisStatus::~GetChassisStatus() {}


bool response::GetChassisStatus::is_response_correct(const std::vector<std::uint8_t>& data) {
    bool retval = true;

    if(ERROR_DATA_SIZE == data.size()) {
        retval = false;
    }
    else if (data.size() < RESPONSE_SIZE) {
        throw std::runtime_error(("Cannot unpack response. Data length too short."
            " Expected: ") + std::to_string(RESPONSE_SIZE)
                                 + " Received: " + std::to_string(data.size()));
    }
    m_completion_code = COMPLETION_CODE(data[OFFSET_COMPLETION_CODE]);
    return retval;
}

void response::GetChassisStatus::unpack(const vector<uint8_t>& data) {
    if(!is_response_correct(data)) {
        return; // Error response, do not unpack values.
    }
    m_power_on = MASK_SYSTEM_POWER & data[OFFSET_POWER_STATUS];
}

