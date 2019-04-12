/*!
 * @brief Set switch LLDP enabled command class definition.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file set_switch_lldp_enabled.cpp
 */

#include "hal/eos_eapi/set_switch_lldp_enabled.hpp"

using namespace agent::network::hal;


std::vector<std::string> SetSwitchLldpEnabled::serialize() const {
    std::vector<std::string> commands;
    commands.emplace_back("configure");
    commands.push_back((m_state ? std::string("") : std::string("no ")) + "lldp run");
    return commands;
}

SetSwitchLldpEnabled::~SetSwitchLldpEnabled() { }