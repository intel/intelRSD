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

#include "agent-framework/command/chassis/json/get_power_zone_info.hpp"
#include "agent-framework/module-ref/constants/chassis.hpp"
#include "agent-framework/logger_ext.hpp"

using namespace agent_framework::command::chassis::json;
using namespace agent_framework::model::literals;

GetPowerZoneInfo::GetPowerZoneInfo() :
    CommandJson(Procedure(TAG,
                jsonrpc::PARAMS_BY_NAME,
                jsonrpc::JSON_STRING,
                PowerZone::ZONE, jsonrpc::JSON_STRING,
                nullptr)) {}

void GetPowerZoneInfo::method(const Json::Value& params, Json::Value& result) {
        Command* command = get_command();

        chassis::GetPowerZoneInfo::Request request{};
        chassis::GetPowerZoneInfo::Response response{};

        request.m_component = params[PowerZone::ZONE].asString();

        command->execute(request, response);

        result = response.m_power_zone.to_json();
}

void GetPowerZoneInfo::notification(const Json::Value&) { }

static CommandJson::Register<GetPowerZoneInfo> g;
