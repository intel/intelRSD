/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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

#include "agent-framework/command-ref/storage_commands.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace agent_framework::command_ref;

static constexpr char SERVICES[] = "services";
static constexpr char TEST_SERVICES[] = "TestServices";

class MyGetStorageServicesInfo {
private:
    std::string m_services{};
public:
    MyGetStorageServicesInfo(std::string services) { m_services = services; }

    void execute(const GetStorageServicesInfo::Request& request,
                 GetStorageServicesInfo::Response& response) {
        auto services = request.get_uuid();

        if (services != m_services) {
            throw std::runtime_error("Not found");
        }

        agent_framework::model::StorageServices service{};
        response = service;
    }
};

TEST(GetStorageServicesInfoTest, PositiveExecute) {
    MyGetStorageServicesInfo command{TEST_SERVICES};
    GetStorageServicesInfo::Request request{""};
    GetStorageServicesInfo::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[SERVICES] = TEST_SERVICES;

    EXPECT_NO_THROW(request = GetStorageServicesInfo::Request::from_json(params));
    EXPECT_NO_THROW(command.execute(request, response));
    EXPECT_NO_THROW(result = response.to_json());
}

TEST(GetStorageServicesInfoTest, NegativeServicesNotFound) {
    MyGetStorageServicesInfo command{TEST_SERVICES};
    GetStorageServicesInfo::Request request{""};
    GetStorageServicesInfo::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[SERVICES] = "OtherTestServices";

    EXPECT_NO_THROW(request = GetStorageServicesInfo::Request::from_json(params));
    EXPECT_ANY_THROW(command.execute(request, response));
}
