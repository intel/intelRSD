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

#include "agent-framework/command/compute/json/get_storage_controller_info.hpp"

using namespace agent_framework::command::compute::json;
using namespace agent_framework::model::literals;

GetStorageControllerInfo::GetStorageControllerInfo() :
    CommandJson(Procedure(TAG,
                jsonrpc::PARAMS_BY_NAME,
                jsonrpc::JSON_STRING,
                StorageController::CONTROLLER, jsonrpc::JSON_STRING,
                nullptr)) { }

void GetStorageControllerInfo::method(const Json::Value& params, Json::Value& result) {
        Command* command = get_command();

        compute::GetStorageControllerInfo::Request request{};
        compute::GetStorageControllerInfo::Response response{};

        request.m_controller =
            params[StorageController::CONTROLLER].asString();

        command->execute(request, response);

        result = response.m_controller.to_json();
}

void GetStorageControllerInfo::notification(const Json::Value&) { }

static CommandJson::Register<GetStorageControllerInfo> g;
