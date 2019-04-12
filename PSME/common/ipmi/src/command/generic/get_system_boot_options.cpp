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
 * @file get_system_boot_options.cpp
 *
 * @brief IPMI Get Chassis Boot Options Command.
 * */

#include "ipmi/command/generic/get_system_boot_options.hpp"
#include "ipmi/command/generic/enums.hpp"

#include <exception>



using namespace ipmi;
using namespace ipmi::command::generic;

constexpr std::uint8_t request::GetSystemBootOptions::REQ_PARAM_SEL;
constexpr std::uint8_t request::GetSystemBootOptions::REQ_SET_SEL;
constexpr std::uint8_t request::GetSystemBootOptions::REQ_BLOCK_SEL;


request::GetSystemBootOptions::GetSystemBootOptions() :
    Request(generic::NetFn::CHASSIS, generic::Cmd::GET_BOOT_ORDER) {}


request::GetSystemBootOptions::~GetSystemBootOptions() {}


void request::GetSystemBootOptions::pack(std::vector<std::uint8_t>& data) const {
    data.push_back(REQ_PARAM_SEL);
    data.push_back(REQ_SET_SEL);
    data.push_back(REQ_SET_SEL);
}


response::GetSystemBootOptions::GetSystemBootOptions() :
    Response(generic::NetFn::CHASSIS, generic::Cmd::GET_BOOT_ORDER, RESPONSE_SIZE) {}


response::GetSystemBootOptions::~GetSystemBootOptions() {}


void response::GetSystemBootOptions::unpack(const std::vector<std::uint8_t>& data) {
    std::size_t data_size = data.size();

    // return in case of wrong data size
    if (RESPONSE_SIZE > data_size) {
        throw std::runtime_error("Invalid response length after getSystemBootOptions IPMI command");
    }

    bool is_valid = (MASK_VALID & data[OFFSET_PERSISTENT]) != 0u;
    bool is_persistent = (MASK_PERSISTENT & data[OFFSET_PERSISTENT]) != 0u;
    bool is_uefi = (MASK_BOOT_MODE & data[OFFSET_PERSISTENT]) != 0u;
    std::uint8_t device_bits = (MASK_DEVICE & data[OFFSET_DEVICE]);
    bool is_none = device_bits == DEVICE_NONE;
    bool is_hdd = device_bits == DEVICE_HDD;
    bool is_pxe = device_bits == DEVICE_PXE;

    // if valid bit is not set or device is set to none -> no boot overrides
    if (!is_valid || is_none) {
        m_boot_override = BootOverride::Disabled;
        m_boot_override_target = BootOverrideTarget::None;
    }
        // since valid bit and one of the device bits are set -> read boot options
    else {
        m_boot_override = is_persistent ? BootOverride::Continuous :
                          BootOverride::Once;

        m_boot_override_target = is_pxe ? BootOverrideTarget::Pxe :
                                 is_hdd ? BootOverrideTarget::Hdd :
                                 BootOverrideTarget::Other;
    }

    m_boot_mode = is_uefi ? BootMode::UEFI : BootMode::Legacy;
}
