/*!
 * @brief Delete QoS priority to priority group mapping command class definition.
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
 * @file delete_switch_qos_priority_group_mapping.cpp
 */

#include "hal/eos_eapi/delete_switch_qos_priority_group_mapping.hpp"

using namespace agent::network::hal;


std::vector<std::string> DeleteSwitchQosPriorityGroupMapping::serialize() const {
    std::vector<std::string> commands;
    commands.emplace_back("configure");
    commands.push_back("no dcbx ets qos map cos " + std::to_string(m_priority) +
                       " traffic-class " + std::to_string(m_priority_group));
    return commands;
}

DeleteSwitchQosPriorityGroupMapping::~DeleteSwitchQosPriorityGroupMapping() { }