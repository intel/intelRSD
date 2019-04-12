/*!
 * @brief Eapi command parser class implementation.
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
 * @file agent/network/src/hal/eos_eapi/port_active_configuration.cpp
 */

#include "hal/eos_eapi/port_running_configuration_parser.hpp"
#include "agent-framework/exceptions/exception.hpp"
#include <regex>

using namespace agent::network::hal::eapi;

namespace {

constexpr char PORT_PFC_ENABLED[] = "priority-flow-control\\smode\\son";

constexpr char PORT_PFC_PRIORITY[] = "(priority-flow-control\\spriority\\s)([0-7]+)(\\sno-drop)";
// index of above regex group in brackets that defines priority number (1 - based)
constexpr int PRIORITY_REGEX_GROUP_INDEX = 2;

constexpr char DCBX_MODE_CEE[] = "dcbx\\smode\\scee";
constexpr char DCBX_MODE_IEEE[] = "dcbx\\smode\\sieee";

constexpr char LLDP_DISABLED[] = "no\\slldp\\sreceive";
}

void PortRunningConfigurationParser::parse(const std::string& output) {
    read_dcbx_state(output);
    read_pfc(output);
    read_lldp(output);

}

void PortRunningConfigurationParser::read_dcbx_state(const std::string& output) {
    try {
        std::regex dcbx_mode_cee(DCBX_MODE_CEE);
        std::regex dcbx_mode_ieee(DCBX_MODE_IEEE);
        if (regex_search(output, dcbx_mode_cee)) {
            m_dcbx_state = agent_framework::model::enums::DcbxState::EnabledCEE;
        }
        else if (regex_search(output, dcbx_mode_ieee)) {
            m_dcbx_state = agent_framework::model::enums::DcbxState::EnabledIEEE;
        }
    }
    catch (std::regex_error& e) {
        std::string message = e.what();
        THROW(agent_framework::exceptions::NetworkError, "eapi-parser", std::string("DCBX regex error: " + message));
    }
}

void PortRunningConfigurationParser::read_pfc(const std::string& output) {
    try {
        std::regex pfc_enabled(PORT_PFC_ENABLED);
        std::regex pfc_priority(PORT_PFC_PRIORITY);
        m_pfc_enabled = regex_search(output, pfc_enabled);
        std::sregex_iterator next(output.begin(), output.end(), pfc_priority);
        std::sregex_iterator end;
        while (next != end) {
            std::smatch match = *next;
            try {
                auto priority = std::stoi(match.str(PRIORITY_REGEX_GROUP_INDEX));
                m_pfc_priorities.push_back(static_cast<uint32_t>(priority));
            }
            catch (std::invalid_argument&) {
                THROW(agent_framework::exceptions::NetworkError, "eapi-parser", "Invalid priority value.");
            }
            next++;
        }
    }
    catch (std::regex_error& e) {
        std::string message = e.what();
        THROW(agent_framework::exceptions::NetworkError, "eapi-parser", std::string("PFC regex error: " + message));
    }
}

void PortRunningConfigurationParser::read_lldp(const std::string& output) {
    std::regex lldp_regex(LLDP_DISABLED);
    if (regex_search(output, lldp_regex)) {
        m_lldp_enabled = false;
    }
    else {
        m_lldp_enabled = true;
    }
}