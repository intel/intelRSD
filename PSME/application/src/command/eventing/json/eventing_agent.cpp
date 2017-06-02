/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @file command/eventing/json/eventing_agent.cpp
 *
 * @brief JSON command EventingAgent implementation
 * */

#include "psme/command/eventing/json/eventing_agent.hpp"
#include "psme/command/eventing/eventing_agent.hpp"

using namespace psme;
using namespace psme::command::eventing::json;

EventingAgent::EventingAgent() :
    CommandJson(eventing::TAG, Procedure(
                eventing::EventingAgent::TAG,
                jsonrpc::PARAMS_BY_NAME,
                "gamiId", jsonrpc::JSON_STRING,
                "component", jsonrpc::JSON_STRING,
                "notification", jsonrpc::JSON_STRING,
                "parent", jsonrpc::JSON_STRING,
                "type", jsonrpc::JSON_STRING,
                nullptr)) {
    }

void EventingAgent::method(const Json::Value&, Json::Value&) { }

void EventingAgent::notification(const Json::Value& params) {

    Command* command = get_command();

    eventing::EventingAgent::Request request{};
    eventing::EventingAgent::Response response{};

    request.m_gami_id = params["gamiId"].asString();
    request.m_component = params["component"].asString();
    request.m_notification = params["notification"].asString();
    request.m_parent = params["parent"].asString();
    request.m_type = params["type"].asString();

    command->execute(request, response);
}

static CommandJson::Register<EventingAgent> g;
