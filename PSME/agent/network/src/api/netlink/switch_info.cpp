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
 * @file switch_info.cpp
 *
 * @brief Switch parameters
 * */
#include "api/netlink/switch_info.hpp"
#include "api/netlink/add_lag_message.hpp"
#include "api/netlink/del_lag_message.hpp"
#include "agent-framework/exceptions/exception.hpp"
#include "netlink/nl_exception.hpp"

#include <net/if.h>

using namespace netlink_base;
using namespace agent::network::api::netlink;

SwitchInfo::SwitchInfo(const std::string& mgmt_port) {
    set_is_enabled(0 != if_nametoindex(CPU_PORT));
    set_management_port(mgmt_port);
}

SwitchInfo::~SwitchInfo() {}

void SwitchInfo::read_switch_info() {
    read_mgmt_mac_address();
}

void SwitchInfo::read_switch_port_list() {
}

void SwitchInfo::add_switch_port(const PortIdentifier& port, PortMode mode)
{
    if (PortMode::LINK_AGGREGATION_STATIC != mode) {
        THROW(agent_framework::exceptions::InvalidValue,
              "network-agent", "Invalid link aggregation mode.");
    }
    try {
        AddLagMessage lag_msg(port);
        lag_msg.send();
    }
    catch (const NlException& error) {
        THROW(agent_framework::exceptions::Fm10000Error, "network-agent",
              "Unable to add new port " + port + ". Error message: " +
              error.what());
    }
}

void SwitchInfo::delete_switch_port(const PortIdentifier& port)
{
    try {
        DelLagMessage lag_msg(port);
        lag_msg.send();
    }
    catch (const NlException& error) {
        THROW(agent_framework::exceptions::Fm10000Error, "network-agent",
              "Unable to remove port " + port + ". Error message: " +
              error.what());
    }
}
