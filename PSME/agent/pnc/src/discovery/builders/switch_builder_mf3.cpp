/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file switch_builder_mf3.cpp
 *
 * @brief SwitchBuilder implementation for MF3
 * */

#include "discovery/builders/switch_builder_mf3.hpp"
#include "discovery/discovery_utils.hpp"
#include "agent-framework/module/utils/to_hex_string.hpp"

#include <exception>

using namespace agent::pnc::discovery::builders;
using namespace agent_framework::model;
using namespace agent_framework::model::utils;
using namespace agent::pnc::sysfs;
using namespace agent::pnc::discovery::utils;

SwitchBuilderMf3::ReturnType SwitchBuilderMf3::update_sysfs(const SysfsSwitch& sysfs_switch) {
    m_obj.set_memory_path(sysfs_switch.memory_resource);
    m_obj.set_bridge_path(sysfs_switch.bridge_path);
    m_obj.set_sec_bus_num(sysfs_switch.sec_bus_num);
    m_obj.set_switch_id(std::to_string(static_cast<uint32_t>(sysfs_switch.bridge_id.device_num)));

    // this is read from sysfs instead of SEEPROM
    attribute::FruInfo fru_info{};
    if (sysfs_switch.has_serial_number) {
        fru_info.set_serial_number(to_hex_string<8, false>(sysfs_switch.serial_number));
    }
    fru_info.set_manufacturer(to_hex_string<2>(sysfs_switch.vendor_id));
    fru_info.set_model_number(to_hex_string<2>(sysfs_switch.device_id));
    m_obj.set_fru_info(std::move(fru_info));

    return *this;
}

[[ noreturn ]] SwitchBuilderMf3::ReturnType SwitchBuilderMf3::update_seeprom(const Seeprom&) {
    // this is not implemented for MF3
    assert(false);
    // will be never thrown - needed for compilation
    throw std::runtime_error("Not implemented");
}
