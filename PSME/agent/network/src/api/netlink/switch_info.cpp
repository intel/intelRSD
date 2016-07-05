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
 * @file switch_info.cpp
 *
 * @brief Switch parameters
 * */
#include "api/netlink/switch_info.hpp"
#include "netlink/socket.hpp"
#include "api/netlink/add_lag_message.hpp"
#include "api/netlink/del_lag_message.hpp"
#include "api/netlink/sysfs.hpp"
#include "agent-framework/exceptions/exception.hpp"
#include "logger/logger_factory.hpp"

#include <net/if.h>

#include <sstream>

using namespace netlink_base;
using namespace agent::network::api::netlink;
using std::stringstream;

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
    Socket socket{};
    AddLagMessage lag_msg(port);
    try {
        socket.connect();
        if (PortMode::LINK_AGGREGATION_STATIC != mode) {
            THROW(agent_framework::exceptions::InvalidParameters,
                  "network-agent", "Invalid link aggregation mode");
        }
        socket.send_message(lag_msg);
        socket.receive_message(lag_msg);
    }
    catch (const std::runtime_error& error) {
        THROW(agent_framework::exceptions::Fm10000Error,
                "network-agent", "Unable to add new port " + port + ". Error message: " + error.what());
    }
}

void SwitchInfo::delete_switch_port(const PortIdentifier& port)
{
    Socket socket{};
    DelLagMessage lag_msg(port);
    try {
        socket.connect();
        socket.send_message(lag_msg);
        socket.receive_message(lag_msg);
    }
    catch (const std::runtime_error&) {
        THROW(agent_framework::exceptions::Fm10000Error,
                "network-agent", "Unable to remove port " + port);
    }
}
