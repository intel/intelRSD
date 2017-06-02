/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @file switch_vlan.cpp
 *
 * @brief Switch Vlan functionality.
 * */

#include "api/switch_vlan.hpp"
#include "agent-framework/exceptions/exception.hpp"
#include "logger/logger_factory.hpp"
#include <sstream>

using namespace agent::network::api;
using std::stringstream;

void SwitchVlan::add_port(const PortIdentifier& port, bool tagged) {
    try {
        add_switch_port(port, tagged);
    }
    catch(const std::runtime_error& error) {
        stringstream ss{};
        ss << "Unable to add vlan " << get_vlan_id() << " to port " << port;
        log_debug(GET_LOGGER("network-agent"), error.what());
        THROW(agent_framework::exceptions::Fm10000Error,
                "network-agent", ss.str());
    }
}

void SwitchVlan::remove_port(const PortIdentifier& port) {
    try {
        remove_switch_port(port);
    }
    catch(const std::runtime_error& error) {
        stringstream ss{};
        ss << "Unable to remove vlan " << get_vlan_id() << " from port " << port;
        log_debug(GET_LOGGER("network-agent"), error.what());
        THROW(agent_framework::exceptions::Fm10000Error,
                "network-agent", ss.str());
    }
}

void SwitchVlan::get_info(const PortIdentifier& port, VlanPortInfo& info) {
    try {
        get_switch_info(port, info);
    }
    catch(const std::runtime_error& error) {
        stringstream ss{};
        ss << "Unable to get vlan " << get_vlan_id() << " info on port " << port;
        log_debug(GET_LOGGER("network-agent"), error.what());
        THROW(agent_framework::exceptions::Fm10000Error,
                "network-agent", ss.str());
    }
}

void SwitchVlan::add_switch_port(const PortIdentifier&, bool) {
}

void SwitchVlan::remove_switch_port(const PortIdentifier&) {
}

void SwitchVlan::get_switch_info(const PortIdentifier&, VlanPortInfo&) {
}

SwitchVlan::~SwitchVlan() {}
