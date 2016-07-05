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
 * */

#include "agent-framework/command/storage/json/set_component_attributes.hpp"
#include "agent-framework/module-ref/constants/common.hpp"

using namespace agent_framework;
using namespace agent_framework::command::storage::json;
using namespace agent_framework::model::attribute;
using namespace agent_framework::model::literals;

SetComponentAttributes::SetComponentAttributes() :
    CommandJson(Procedure(TAG,
                jsonrpc::PARAMS_BY_NAME,
                jsonrpc::JSON_STRING,
                Component::COMPONENT, jsonrpc::JSON_STRING,
                Component::ATTRIBUTES, jsonrpc::JSON_OBJECT,
                nullptr)) {}

void
SetComponentAttributes::method(const Json::Value& params, Json::Value& result) {
    Command* command = get_command();

    storage::SetComponentAttributes::Request request{};
    storage::SetComponentAttributes::Response response{};

    request.m_component = params[Component::COMPONENT].asString();

    if (params[Component::ATTRIBUTES].isObject()) {
        request.m_attributes = Attributes::from_json(params[Component::ATTRIBUTES]);
    }

    command->execute(request, response);

    result = Json::objectValue;
}

void SetComponentAttributes::notification(const Json::Value&) { }

static CommandJson::Register<SetComponentAttributes> g;
