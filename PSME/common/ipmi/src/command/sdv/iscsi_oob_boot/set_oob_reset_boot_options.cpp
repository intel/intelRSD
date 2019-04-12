/*!
 * @brief Set OOB Reset Boot Options Command Implementation.
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
 * @file set_oob_reset_boot_option.cpp
 */

#include "ipmi/command/sdv/iscsi_oob_boot/set_oob_reset_boot_options.hpp"



using namespace ipmi;
using namespace ipmi::command::sdv;

constexpr std::uint8_t request::SetOobResetBootOptions::CLEAR;


request::SetOobResetBootOptions::SetOobResetBootOptions() :
    request::SetOobBootOptions(request::SetOobBootOptions::ParameterType::RESET) {
}


request::SetOobResetBootOptions::~SetOobResetBootOptions() {}


void request::SetOobResetBootOptions::pack(std::vector<uint8_t>& data) const {

    data.push_back(uint8_t(get_parameter_id()));
    data.push_back(PARAMETER_VERSION);
    data.push_back(CLEAR);
}


response::SetOobResetBootOptions::SetOobResetBootOptions() :
    response::SetOobBootOptions() {}


response::SetOobResetBootOptions::~SetOobResetBootOptions() {}


void response::SetOobResetBootOptions::unpack(const std::vector<uint8_t>&) { }
