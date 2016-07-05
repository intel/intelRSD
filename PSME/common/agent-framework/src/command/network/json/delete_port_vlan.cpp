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
 * @file command/network/json/delete_port_vlan.cpp
 *
 * @brief JSON command DeletePortVlan implementation
 * */

#include "agent-framework/command/network/json/delete_port_vlan.hpp"
#include "agent-framework/module-ref/constants/network.hpp"
#include "agent-framework/module-ref/constants/common.hpp"

using namespace agent_framework::command;
using namespace agent_framework::command::network::json;
using namespace agent_framework::model::literals;

DeletePortVlan::DeletePortVlan() :
    CommandJson(Procedure(TAG,
                jsonrpc::PARAMS_BY_NAME,
                jsonrpc::JSON_OBJECT,
                PortVlan::PORT_VLAN, jsonrpc::JSON_STRING,
                Oem::OEM, jsonrpc::JSON_OBJECT,
                nullptr)) { }

void DeletePortVlan::method(const Json::Value& params, Json::Value& result) {
    Command* command = get_command();

    network::DeletePortVlan::Request request{};
    network::DeletePortVlan::Response response{};

    request.m_port_vlan = params[PortVlan::PORT_VLAN].asString();

    command->execute(request, response);

    result[Oem::OEM] = response.m_oem.to_json();
}

void DeletePortVlan::notification(const Json::Value&) { }

static CommandJson::Register<DeletePortVlan> g;
