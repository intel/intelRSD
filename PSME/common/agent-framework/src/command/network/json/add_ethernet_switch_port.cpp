/*!
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
 * @file command/network/json/add_ethernet_switch_port.cpp
 *
 * @brief JSON command AddEthernetSwitchPort implementation
 * */

#include "agent-framework/command/network/json/add_ethernet_switch_port.hpp"
#include "agent-framework/module-ref/constants/network.hpp"
#include "agent-framework/module-ref/constants/common.hpp"
#include "agent-framework/module-ref/utils/utils.hpp"

using namespace agent_framework::command;
using namespace agent_framework::command::network::json;
using namespace agent_framework::model::literals;
using namespace agent_framework::model::utils;
using namespace agent_framework::model;

AddEthernetSwitchPort::AddEthernetSwitchPort() :
    CommandJson(Procedure(TAG,
                jsonrpc::PARAMS_BY_NAME,
                jsonrpc::JSON_OBJECT,
                SwitchPort::SWITCH, jsonrpc::JSON_STRING,
                SwitchPort::PORT_IDENTIFIER, jsonrpc::JSON_STRING,
                SwitchPort::MODE, jsonrpc::JSON_STRING,
                SwitchPort::MEMBERS, jsonrpc::JSON_ARRAY,
                SwitchPort::OEM, jsonrpc::JSON_OBJECT,
                nullptr)) { }

void AddEthernetSwitchPort::method(const Json::Value& params,
    Json::Value& result) {
    Command* command = get_command();

    network::AddEthernetSwitchPort::Request request{};
    network::AddEthernetSwitchPort::Response response{};

    request.m_switch = params[SwitchPort::SWITCH].asString();
    request.m_port_identifier = params[SwitchPort::PORT_IDENTIFIER].asString();
    request.m_mode =
        enums::PortMode::from_string(params[SwitchPort::MODE].asString());
    request.m_members = json_to_string_vector(params[SwitchPort::MEMBERS]);

    command->execute(request, response);

    result[SwitchPort::PORT] = response.m_port;
    result[SwitchPort::OEM] = response.m_oem.to_json();
}

void AddEthernetSwitchPort::notification(const Json::Value&) { }

static CommandJson::Register<AddEthernetSwitchPort> g;
