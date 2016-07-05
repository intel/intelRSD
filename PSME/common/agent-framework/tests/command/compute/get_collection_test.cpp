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

#include "agent-framework/command/compute/get_collection.hpp"
#include "agent-framework/command/compute/json/get_collection.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace agent_framework::command;
using namespace agent_framework::command::exception;

class GetCollection : public compute::GetCollection {
private:
    std::string m_component{};
    std::string m_name{};
public:
    GetCollection(
        const std::string& component,
        const std::string& name) {
        m_component = component;
        m_name = name;
    }

    using compute::GetCollection::execute;

    void execute(const Request& request, Response& response) {
        const auto& component = request.get_component();
        const auto& name = request.get_name();

        if (component != m_component || name != m_name) {
            throw exception::NotFound();
        }

        for (std::uint32_t i = 0; i < 10; ++i) {
            response.add_subcomponent(
                agent_framework::model::attribute::SubcomponentEntry(std::to_string(i)));
        }
    }

    virtual ~GetCollection();
};

GetCollection::~GetCollection() { }

class GetCollectionTest : public ::testing::Test {
public:
    virtual ~GetCollectionTest();
};

GetCollectionTest::~GetCollectionTest() { }

TEST_F(GetCollectionTest, PositiveExecute) {
    compute::json::GetCollection command_json;
    GetCollection* command = new GetCollection("TestModuleId", "TestName");

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params["component"] = "TestModuleId";
    params["name"] = "TestName";

    EXPECT_NO_THROW(command_json.method(params, result));

    ASSERT_TRUE(result.isArray());
    ASSERT_EQ(result.size(), 10);
    for (std::uint32_t i = 0; i < 10; ++i) {
        ASSERT_EQ(result[i]["subcomponent"].asString(), std::to_string(i));
    }
}

TEST_F(GetCollectionTest, NegativeModuleNotFound) {
    compute::json::GetCollection command_json;
    GetCollection* command = new GetCollection("TestModuleId", "TestName");

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params["component"] = "OtherTestModuleId";
    params["name"] = "TestName";

    EXPECT_THROW(command_json.method(params, result), exception::NotFound);
}

TEST_F(GetCollectionTest, NegativeNameNotFound) {
    compute::json::GetCollection command_json;
    GetCollection* command = new GetCollection("TestModuleId", "TestName");

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params["component"] = "TestModuleId";
    params["name"] = "OtherTestName";

    EXPECT_THROW(command_json.method(params, result), exception::NotFound);
}
