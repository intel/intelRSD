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
 * @file command/heart_beat/json/heart_beat.cpp
 *
 * @brief JSON command HeartBeat implementation
 * */

#include "psme/command/heart_beat/json/heart_beat.hpp"
#include "psme/command/heart_beat/heart_beat.hpp"
#include "logger/logger_factory.hpp"

using namespace std;

using namespace psme;
using namespace psme::command::heart_beat::json;

HeartBeat::HeartBeat() :
    CommandJson(heart_beat::TAG, Procedure(
                heart_beat::HeartBeat::TAG,
                jsonrpc::PARAMS_BY_NAME,
                jsonrpc::JSON_OBJECT,
                "gamiId", jsonrpc::JSON_STRING,
                nullptr)) { }

void HeartBeat::method(const Json::Value& params, Json::Value& result) {
    try {
        Command* command = get_command();

        heart_beat::HeartBeat::Request request{};
        request.m_gami_id = params["gamiId"].asString();
        heart_beat::HeartBeat::Response response{};

        command->execute(request, response);

        result["timestamp"] = Json::UInt64(response.m_time_live);
        result["minDelay"] = Json::UInt64(response.m_minimum_delay.count());
    } catch (const command::exception::NotFound&) {
        log_error(GET_LOGGER("heartBeat"), "-32602 Command not found");
        /* @TODO: Move common exceptions to JSON command server */
        throw jsonrpc::JsonRpcException(-32602, "Command not found");
    } catch (...) {
        log_error(GET_LOGGER("heartBeat"), "-1 JSON command error");
        throw jsonrpc::JsonRpcException(-1, "JSON command error");
    }
}

void HeartBeat::notification(const Json::Value&) { }

static CommandJson::Register<HeartBeat> g;
