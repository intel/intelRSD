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
 * @file command/network/json/delete_vlan.cpp
 *
 * @brief JSON command DeleteVlan implementation
 * */

#include "agent-framework/command/network/json/delete_vlan.hpp"
#include "agent-framework/module-ref/constants/network.hpp"
#include "agent-framework/module-ref/constants/common.hpp"

using namespace agent_framework::command;
using namespace agent_framework::command::network::json;
using namespace agent_framework::model::literals;

DeleteVlan::DeleteVlan() :
    CommandJson(Procedure(TAG,
                jsonrpc::PARAMS_BY_NAME,
                jsonrpc::JSON_OBJECT,
                Vlan::VLAN, jsonrpc::JSON_STRING,
                Oem::OEM, jsonrpc::JSON_OBJECT,
                nullptr)) { }

void DeleteVlan::method(const Json::Value& params, Json::Value& result) {
    Command* command = get_command();

    network::DeleteVlan::Request request{};
    network::DeleteVlan::Response response{};

    request.m_vlan = params[Vlan::VLAN].asString();

    command->execute(request, response);

    result[Oem::OEM] = response.m_oem.to_json();
}

void DeleteVlan::notification(const Json::Value&) { }

static CommandJson::Register<DeleteVlan> g;
