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

class MyGetCollection {
private:
    std::string m_component{};
    std::string m_name{};
public:
    MyGetCollection(
        const std::string& component,
        const std::string& name) {
        m_component = component;
        m_name = name;
    }

    void execute(const GetCollection::Request& request,
                 GetCollection::Response& response) {
        auto component = request.get_uuid();
        auto name = request.get_name();

        if (component != m_component || name != m_name) {
            throw std::runtime_error("Not found");
        }

        for (std::uint32_t i = 0; i < 10; ++i) {
            response.add_entry(
                agent_framework::model::attribute::SubcomponentEntry(std::to_string(i)));
        }
    }
};

static constexpr char COMPONENT[] = "component";
static constexpr char NAME[] = "name";

TEST(GetCollectionTest, PositiveExecute) {
    MyGetCollection command{"TestModuleId", "TestName"};
    GetCollection::Request request{"", ""};
    GetCollection::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[COMPONENT] = "TestModuleId";
    params[NAME] = "TestName";

    EXPECT_NO_THROW(request = GetCollection::Request::from_json(params));
    EXPECT_NO_THROW(command.execute(request, response));
    EXPECT_NO_THROW(result = response.to_json());

    ASSERT_TRUE(result.isArray());
    ASSERT_EQ(result.size(), 10);
    for (std::uint32_t i = 0; i < 10; ++i) {
        ASSERT_EQ(result[i]["subcomponent"].asString(), std::to_string(i));
    }
}

TEST(GetCollectionTest, NegativeModuleNotFound) {
    MyGetCollection command{"TestModuleId", "TestName"};
    GetCollection::Request request{"", ""};
    GetCollection::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[COMPONENT] = "OtherTestModuleId";
    params[NAME] = "TestName";

    EXPECT_NO_THROW(request = GetCollection::Request::from_json(params));
    EXPECT_ANY_THROW(command.execute(request, response));
}

TEST(GetCollectionTest, NegativeNameNotFound) {
    MyGetCollection command{"TestModuleId", "TestName"};
    GetCollection::Request request{"", ""};
    GetCollection::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[COMPONENT] = "TestModuleId";
    params[NAME] = "OtherTestName";

    EXPECT_NO_THROW(request = GetCollection::Request::from_json(params));
    EXPECT_ANY_THROW(command.execute(request, response));
}
