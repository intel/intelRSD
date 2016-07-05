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
 * @file command/storage/json/add_iscsi_target.cpp
 *
 * @brief JSON command AddIscsiTarget
 * */

#include "agent-framework/command/storage/json/add_iscsi_target.hpp"

using namespace agent_framework::command::storage::json;
using namespace agent_framework::model::literals;

AddIscsiTarget::AddIscsiTarget() :
    CommandJson(Procedure(TAG,
                jsonrpc::PARAMS_BY_NAME,
                jsonrpc::JSON_STRING,
                IscsiTarget::TARGET_IQN, jsonrpc::JSON_STRING,
                IscsiTarget::INITIATOR_IQN, jsonrpc::JSON_STRING,
                IscsiTarget::TARGET_LUNS, jsonrpc::JSON_ARRAY,
                IscsiTarget::OEM, jsonrpc::JSON_OBJECT,
                nullptr)) { }

void AddIscsiTarget::method(const Json::Value& params, Json::Value& result) {
        Command* command = get_command();

        storage::AddIscsiTarget::Request request{};
        storage::AddIscsiTarget::Response response{};

        request.m_initiator_iqn =
            params[IscsiTarget::INITIATOR_IQN].asString();
        request.m_target_luns =
            TargetLunArray::from_json(params[IscsiTarget::TARGET_LUNS]);
        request.m_target_iqn = params[IscsiTarget::TARGET_IQN].asString();
        request.m_oem =
            model::attribute::Oem::from_json(params[IscsiTarget::OEM]);

        command->execute(request, response);

        result[IscsiTarget::TARGET] = response.m_target;
        result[IscsiTarget::OEM] = response.m_oem.to_json();
}

void AddIscsiTarget::notification(const Json::Value&) { }

static CommandJson::Register<AddIscsiTarget> g;
