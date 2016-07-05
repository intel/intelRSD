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
 * @file command/compute/json/get_processor_info.cpp
 *
 * @brief JSON command get processor information implementation
 * */

#include "agent-framework/command/compute/json/get_processor_info.hpp"

using namespace agent_framework::command::compute::json;
using namespace agent_framework::model::literals;

GetProcessorInfo::GetProcessorInfo() :
    CommandJson(Procedure(TAG,
                jsonrpc::PARAMS_BY_NAME,
                jsonrpc::JSON_OBJECT,
                Processor::PROCESSOR, jsonrpc::JSON_STRING,
                nullptr)) { }

void GetProcessorInfo::method(const Json::Value& params, Json::Value& result) {
        Command* command = get_command();

        compute::GetProcessorInfo::Request request{};
        compute::GetProcessorInfo::Response response{};

        request.m_processor = params[Processor::PROCESSOR].asString();

        command->execute(request, response);

        result = response.m_processor.to_json();
}

void GetProcessorInfo::notification(const Json::Value&) { }

static CommandJson::Register<GetProcessorInfo> g;
