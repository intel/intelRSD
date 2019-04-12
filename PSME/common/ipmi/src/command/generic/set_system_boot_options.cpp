/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * @file set_system_boot_options.cpp
 *
 * @brief Set System Boot Option Command Implementation.
 * */

#include "ipmi/command/generic/set_system_boot_options.hpp"
#include "ipmi/command/generic/enums.hpp"



using namespace ipmi;
using namespace ipmi::command::generic;

constexpr std::uint8_t request::SetSystemBootOptions::REQUEST_BYTE_3_DEFAULT;
constexpr std::uint8_t request::SetSystemBootOptions::REQUEST_BYTE_4_DEFAULT;
constexpr std::uint8_t request::SetSystemBootOptions::REQUEST_BYTE_5_DEFAULT;


request::SetSystemBootOptions::SetSystemBootOptions(BootOverride boot_override, BootMode boot_mode,
                                                    BootOverrideTarget boot_override_target) :
    Request(generic::NetFn::CHASSIS, generic::Cmd::SET_BOOT_ORDER), m_boot_override{boot_override},
    m_boot_mode{boot_mode}, m_boot_override_target{boot_override_target} {}


request::SetSystemBootOptions::~SetSystemBootOptions() {}


void request::SetSystemBootOptions::pack(std::vector<std::uint8_t>& data) const {
    std::uint8_t param_validity = std::uint8_t(ParameterValidity::VALID);
    std::uint8_t param_selector = std::uint8_t(ParameterSelector::BOOT_FLAGS);

    std::uint8_t param_options = std::uint8_t(param_validity << 7 | param_selector << 0);

    data.push_back(param_options);

    std::uint8_t boot_options = MASK_VALID;
    if (m_boot_override == BootOverride::Continuous) {
        boot_options |= MASK_PERSISTENT;
    }
    if (m_boot_mode == BootMode::UEFI) {
        boot_options |= MASK_BOOT_MODE;
    }

    std::uint8_t boot_target = DEVICE_NONE;
    if (m_boot_override_target == BootOverrideTarget::Hdd) {
        boot_target = DEVICE_HDD;
    }
    else if (m_boot_override_target == BootOverrideTarget::Pxe) {
        boot_target |= DEVICE_PXE;
    }
    else if (m_boot_override_target == BootOverrideTarget::RemoteDrive) {
        boot_target |= DEVICE_REMOTE_HDD;
    }

    // 5 bytes of the options for BOOT FLAGS.
    data.push_back(boot_options);
    data.push_back(boot_target);
    data.push_back(REQUEST_BYTE_3_DEFAULT);
    data.push_back(REQUEST_BYTE_4_DEFAULT);
    data.push_back(REQUEST_BYTE_5_DEFAULT);
}


response::SetSystemBootOptions::SetSystemBootOptions() :
    Response(generic::NetFn::CHASSIS, generic::Cmd::SET_BOOT_ORDER, RESPONSE_SIZE) {}


response::SetSystemBootOptions::~SetSystemBootOptions() {}


void response::SetSystemBootOptions::unpack(const std::vector<std::uint8_t>&) {
}
