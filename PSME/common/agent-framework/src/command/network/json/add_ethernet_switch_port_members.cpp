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
 * @file command/network/json/add_ethernet_switch_port_members.cpp
 *
 * @brief JSON command AddEthernetSwitchPortMembers implementation
 * */

#include \
    "agent-framework/command/network/json/add_ethernet_switch_port_members.hpp"
#include "agent-framework/module-ref/constants/network.hpp"
#include "agent-framework/module-ref/utils/utils.hpp"

using namespace agent_framework::command::network::json;
using namespace agent_framework::model::literals;
using namespace agent_framework::model::utils;
using namespace agent_framework::model;

AddEthernetSwitchPortMembers::AddEthernetSwitchPortMembers() :
    CommandJson(Procedure(TAG,
                jsonrpc::PARAMS_BY_NAME,
                jsonrpc::JSON_OBJECT,
                SwitchPortMembers::PORT, jsonrpc::JSON_STRING,
                SwitchPortMembers::MEMBERS, jsonrpc::JSON_ARRAY,
                SwitchPortMembers::OEM, jsonrpc::JSON_OBJECT,
                nullptr)) { }

void AddEthernetSwitchPortMembers::method(const Json::Value& params,
    Json::Value& result) {

    Command* command = get_command();

    network::AddEthernetSwitchPortMembers::Request request{};
    network::AddEthernetSwitchPortMembers::Response response{};

    request.m_port = params[SwitchPortMembers::PORT].asString();
    request.m_members = json_to_string_vector(
        params[SwitchPortMembers::MEMBERS]);
    request.m_oem = attribute::Oem::from_json(
        params[SwitchPortMembers::OEM]);

    command->execute(request, response);

    result[SwitchPortMembers::OEM] = response.m_oem.to_json();
}

void AddEthernetSwitchPortMembers::notification(const Json::Value&) { }

static CommandJson::Register<AddEthernetSwitchPortMembers> g;
