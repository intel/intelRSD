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
 * @file command/chassis/json/get_component_info.cpp
 *
 * @brief JSON command get component info implementation
 * */

#include "agent-framework/command/chassis/json/get_fan_info.hpp"
#include "agent-framework/module-ref/constants/chassis.hpp"
#include "agent-framework/logger_ext.hpp"

using namespace agent_framework::command::chassis::json;
using namespace agent_framework::model::literals;

GetFanInfo::GetFanInfo() :
    CommandJson(Procedure(TAG,
                jsonrpc::PARAMS_BY_NAME,
                jsonrpc::JSON_STRING,
                Fan::FAN, jsonrpc::JSON_STRING,
                nullptr)) {}

void GetFanInfo::method(const Json::Value& params, Json::Value& result) {
        Command* command = get_command();

        chassis::GetFanInfo::Request request{};
        chassis::GetFanInfo::Response response{};

        request.m_component = params[Fan::FAN].asString();

        command->execute(request, response);

        result = response.m_fan.to_json();
}

void GetFanInfo::notification(const Json::Value&) { }

static CommandJson::Register<GetFanInfo> g;
