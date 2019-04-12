/*!
 * @brief Set OOB Control Boot Options Command Implementation.
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
 * @file set_oob_control_boot_option.cpp
 */

#include "ipmi/command/sdv/iscsi_oob_boot/set_oob_control_boot_options.hpp"



using namespace ipmi;
using namespace ipmi::command::sdv;

constexpr std::uint8_t request::SetOobControlBootOptions::CONTROL_PARAMETER_INDEX;
constexpr std::uint8_t request::SetOobControlBootOptions::ISCSI_DISABLED_IN_BIOS[request::SetOobControlBootOptions::BIOS_BOOT_FLAG_BYTES];
constexpr std::uint8_t request::SetOobControlBootOptions::ISCSI_ENABLED_IN_BIOS[request::SetOobControlBootOptions::BIOS_BOOT_FLAG_BYTES];


request::SetOobControlBootOptions::SetOobControlBootOptions() :
    request::SetOobBootOptions(request::SetOobBootOptions::ParameterType::CONTROL) {}


request::SetOobControlBootOptions::~SetOobControlBootOptions() {}


void request::SetOobControlBootOptions::pack(std::vector<uint8_t>& data) const {

    data.push_back(get_parameter_id());
    data.push_back(PARAMETER_VERSION);
    data.push_back(CONTROL_PARAMETER_INDEX);
    for (auto byte : (m_bios_enabled ? ISCSI_ENABLED_IN_BIOS : ISCSI_DISABLED_IN_BIOS)) {
        data.push_back(byte);
    }
}


response::SetOobControlBootOptions::SetOobControlBootOptions() :
    response::SetOobBootOptions() {}


response::SetOobControlBootOptions::~SetOobControlBootOptions() {}


void response::SetOobControlBootOptions::unpack(const std::vector<uint8_t>&) { }
