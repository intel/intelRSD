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
 * */

#include "agent-framework/command/chassis/json/get_manager_collection.hpp"

using namespace agent_framework;
using namespace agent_framework::command::chassis::json;

GetManagerCollection::GetManagerCollection() :
    CommandJson(Procedure(TAG,
                jsonrpc::PARAMS_BY_NAME,
                jsonrpc::JSON_STRING,
                nullptr)) {}

void GetManagerCollection::method(const Json::Value&, Json::Value& result) {
        Command* command = get_command();

        chassis::GetManagerCollection::Request request{};
        chassis::GetManagerCollection::Response response{};

        command->execute(request, response);

        result = response.m_managers.to_json();
}

void GetManagerCollection::notification(const Json::Value& /*params*/) { }

static CommandJson::Register<GetManagerCollection> g;
