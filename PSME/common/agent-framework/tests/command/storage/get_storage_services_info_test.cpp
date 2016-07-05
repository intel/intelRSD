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

#include "agent-framework/command/storage/get_storage_services_info.hpp"
#include "agent-framework/command/storage/json/get_storage_services_info.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace agent_framework::command;
using namespace agent_framework::command::exception;

static constexpr char SERVICES[] = "services";

class GetStorageServicesInfo : public storage::GetStorageServicesInfo {
private:
    std::string m_services{};
public:
    GetStorageServicesInfo(std::string services) { m_services = services; }

    using storage::GetStorageServicesInfo::execute;

    void execute(const Request& request, Response& response) {
        auto services = request.get_services();

        if (services != m_services) {
            throw exception::NotFound();
        }

        agent_framework::model::StorageServices service{};
        response.set_services(service);
    }

    virtual ~GetStorageServicesInfo();
};

GetStorageServicesInfo::~GetStorageServicesInfo() { }

TEST(GetStorageServicesInfoTest, PositiveExecute) {
    storage::json::GetStorageServicesInfo command_json;
    GetStorageServicesInfo* command = new GetStorageServicesInfo("TestServices");

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[SERVICES] = "TestServices";

    EXPECT_NO_THROW(command_json.method(params, result));
}

TEST(GetStorageServicesInfoTest, NegativeServicesNotFound) {
    storage::json::GetStorageServicesInfo command_json;
    GetStorageServicesInfo* command = new GetStorageServicesInfo("TestServices");

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[SERVICES] = "OtherTestServices";

    EXPECT_THROW(command_json.method(params, result), NotFound);
}
