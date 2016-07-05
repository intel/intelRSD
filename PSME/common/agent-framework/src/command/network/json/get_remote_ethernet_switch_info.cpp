/*!
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file command/network/json/get_remote_switch_info.cpp
 * @brief JSON command get remote ethernet switch info information
 * implementation
 * */

#include "agent-framework/command/network/json/get_remote_ethernet_switch_info.hpp"
#include "agent-framework/module-ref/constants/network.hpp"

using namespace agent_framework::command;
using namespace agent_framework::command::network::json;
using namespace agent_framework::model::literals;

GetRemoteEthernetSwitchInfo::GetRemoteEthernetSwitchInfo() :
    CommandJson(Procedure(TAG,
                jsonrpc::PARAMS_BY_NAME,
                jsonrpc::JSON_OBJECT,
                RemoteSwitch::SWITCH, jsonrpc::JSON_STRING,
                nullptr)) { }

void GetRemoteEthernetSwitchInfo::method(const Json::Value& params, Json::Value& result) {
    Command* command = get_command();

    network::GetRemoteEthernetSwitchInfo::Request request{};
    network::GetRemoteEthernetSwitchInfo::Response response{};

    request.m_switch = params[RemoteSwitch::SWITCH].asString();

    command->execute(request, response);

    result = response.m_switch.to_json();
}

void GetRemoteEthernetSwitchInfo::notification(const Json::Value&) { }

static CommandJson::Register<GetRemoteEthernetSwitchInfo> g;
