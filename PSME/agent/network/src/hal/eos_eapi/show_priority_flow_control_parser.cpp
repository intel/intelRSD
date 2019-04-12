/*!
 * @brief Eapi show priority flow control command parser class definition.
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
 * @file show_priority_flow_control_parser.cpp
 */

#include "hal/eos_eapi/show_priority_flow_control_parser.hpp"
#include <regex>

using namespace agent::network::hal::eapi;

namespace {

constexpr char SWITCH_PFC_ENABLED[] = "Global PFC : Enabled";
constexpr char SWITCH_PFC_DISABLED[] = "Global PFC : Disabled";

}


void ShowPriorityFlowControlParser::parse(const std::string& output) {
    std::regex switch_pfc_enabled(SWITCH_PFC_ENABLED);
    std::regex switch_pfc_disabled(SWITCH_PFC_DISABLED);
    if (regex_search(output, switch_pfc_enabled)) {
        m_pfc_enabled = true;
    }
}