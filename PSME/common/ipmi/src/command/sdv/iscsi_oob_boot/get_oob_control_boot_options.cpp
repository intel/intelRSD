/*!
 * @brief Get OOB Control Boot Options Command Implementation.
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
 * @file get_oob_control_boot_option.cpp
 */


#include "ipmi/command/sdv/iscsi_oob_boot/get_oob_control_boot_options.hpp"



using namespace ipmi;
using namespace ipmi::command::sdv;


request::GetOobControlBootOptions::GetOobControlBootOptions(uint8_t index) :
    request::GetOobBootOptions(request::GetOobBootOptions::ParameterType::CONTROL, index) {}


request::GetOobControlBootOptions::~GetOobControlBootOptions() {}


void request::GetOobControlBootOptions::pack(std::vector<uint8_t>& data) const {

    data.push_back(get_parameter_id());
    data.push_back(get_parameter_index());
}


response::GetOobControlBootOptions::GetOobControlBootOptions() :
    response::GetOobBootOptions() {}


response::GetOobControlBootOptions::~GetOobControlBootOptions() {}


void response::GetOobControlBootOptions::unpack(const std::vector<uint8_t>& data) {

    if (!valid_basic_parameter(data)) {
        return;
    }

    set_bios_enabled(0 != (data[OFFSET_BIOS_ENABLED] & BIOS_ENABLED_MASK));
}
