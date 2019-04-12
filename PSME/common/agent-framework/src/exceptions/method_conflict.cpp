/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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
 *
 * @file method_conflict.cpp
 * @brief MethodConflict implementation
 * */

#include "agent-framework/exceptions/method_conflict.hpp"

using namespace agent_framework::exceptions;

namespace {
    json::Json create_method_conflict_data(const std::string& task_uuid) {
        json::Json json_data = json::Json();
        json_data[MethodConflict::TASK] = task_uuid;
        return json_data;
    }
}

const constexpr char MethodConflict::TASK[];


MethodConflict::MethodConflict(const std::string& msg, const std::string& task_uuid) :
    GamiException(ErrorCode::METHOD_CONFLICT, msg, create_method_conflict_data(task_uuid)) { }


std::string MethodConflict::get_task_uuid(const json::Json& data) {
    return data.value(MethodConflict::TASK, std::string{});
}

MethodConflict::~MethodConflict() {}

