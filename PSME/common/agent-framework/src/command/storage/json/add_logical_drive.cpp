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
 * @file command/storage/json/add_logical_drive.cpp
 *
 * @brief JSON command AddLogicalDrive
 * */

#include "agent-framework/command/storage/json/add_logical_drive.hpp"

using namespace agent_framework::command::storage::json;
using namespace agent_framework::model::literals;


AddLogicalDrive::AddLogicalDrive() :
    CommandJson(Procedure(TAG,
                jsonrpc::PARAMS_BY_NAME,
                jsonrpc::JSON_STRING,
                LogicalDrive::TYPE, jsonrpc::JSON_STRING,
                LogicalDrive::CAPACITY, CommandJson::JSON_ANYTHING,
                LogicalDrive::MODE, jsonrpc::JSON_STRING,
                LogicalDrive::MASTER, jsonrpc::JSON_STRING,
                LogicalDrive::IMAGE, jsonrpc::JSON_STRING,
                LogicalDrive::SNAPSHOT, jsonrpc::JSON_BOOLEAN,
                LogicalDrive::PROTECTED, jsonrpc::JSON_BOOLEAN,
                LogicalDrive::DRIVES, jsonrpc::JSON_ARRAY,
                LogicalDrive::OEM, jsonrpc::JSON_OBJECT,
                nullptr)) { }

void AddLogicalDrive::method(const Json::Value& params, Json::Value& result) {
        Command* command = get_command();

        storage::AddLogicalDrive::Request request{};
        storage::AddLogicalDrive::Response response{};

        request.m_type = model::enums::LogicalDriveType::from_string(
            params[LogicalDrive::TYPE].asString());
        request.m_capacity_gb = params[LogicalDrive::CAPACITY].asDouble();
        request.m_mode = model::enums::LogicalDriveMode::from_string(
            params[LogicalDrive::MODE].asString());
        request.m_master = params[LogicalDrive::MASTER].asString();
        request.m_image = params[LogicalDrive::IMAGE].asString();
        request.m_snapshot = params[LogicalDrive::SNAPSHOT].asBool();
        request.m_protected = params[LogicalDrive::PROTECTED].asBool();
        request.m_oem = model::attribute::Oem::from_json(
            params[LogicalDrive::OEM]);

        command->execute(request, response);

        result[LogicalDrive::DRIVE] = response.m_drive;
        result[LogicalDrive::OEM] = response.m_oem.to_json();
}

void AddLogicalDrive::notification(const Json::Value&) { }

static CommandJson::Register<AddLogicalDrive> g;
