/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * */

#include "agent-framework/module/port.hpp"
#include "agent-framework/logger_ext.hpp"

#include "json/json.hpp"

#include <algorithm>

using namespace agent_framework::generic;

Port::~Port(){}

void Port::read_configuration(const json::Value& port_configuration) {
    try {
        log_debug(GET_LOGGER("module"), "Reading ports configuration.");
        set_id(port_configuration["id"].as_uint());

        if (port_configuration.is_member("link_state")) {
            set_link_state(m_state_string_to_enum(
                           port_configuration["link_state"].as_string()));
        }

        // Optional parameters
        if (port_configuration.is_member("ethmod")) {
            set_iface_mode(port_configuration["ethmod"].as_string());
        }
        else {
            set_iface_mode("default");
        }

        if (port_configuration.is_member("type")) {
            set_type(port_configuration["type"].as_string());
        }

        if (port_configuration.is_member("autoneg")) {
            set_autoneg_mode(port_configuration["autoneg"].as_string());
        }
        else {
            set_autoneg_mode("default");
        }
     }
    catch (const json::Value::Exception& e) {
        log_warning(GET_LOGGER("module"), "Invalid/missing submodule configuration member: "
                  << e.what());
    }
    catch (...) {
        log_alert(GET_LOGGER("module"), "Unknown error in submodule section");
    }
}

Port::LinkState Port::m_state_string_to_enum(const std::string& state_str) {
    if (m_state_string_to_enum_map.find(state_str) !=
            m_state_string_to_enum_map.end()) {
        return m_state_string_to_enum_map[state_str];
    }
    else {
        log_warning(GET_LOGGER("module"), "Unknown port state provided.");
        return LinkState::UNKNOWN;
    }
}

