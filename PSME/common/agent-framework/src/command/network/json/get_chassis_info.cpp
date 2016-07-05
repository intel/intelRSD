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
 * @file command/network/json/get_chassis_info.cpp
 *
 * @brief JSON command get chassis information implementation
 * */

#include "agent-framework/command/network/json/get_chassis_info.hpp"
#include "agent-framework/module-ref/constants/chassis.hpp"

using namespace agent_framework::command;
using namespace agent_framework::command::network::json;
using namespace agent_framework::model::literals;

GetChassisInfo::GetChassisInfo() :
    CommandJson(Procedure(TAG,
                jsonrpc::PARAMS_BY_NAME,
                jsonrpc::JSON_OBJECT,
                Chassis::CHASSIS, jsonrpc::JSON_STRING,
                nullptr)) { }

void GetChassisInfo::method(const Json::Value& params, Json::Value& result) {
        Command* command = get_command();

        network::GetChassisInfo::Request request{};
        network::GetChassisInfo::Response response{};

        request.m_chassis = params[Chassis::CHASSIS].asString();

        command->execute(request, response);

        result = response.m_chassis.to_json();
}

void GetChassisInfo::notification(const Json::Value&) { }

static CommandJson::Register<GetChassisInfo> g;
