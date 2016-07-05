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

#include "agent-framework/command/compute/get_components.hpp"
#include "agent-framework/command/compute/json/get_components.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace agent_framework::command;
using namespace agent_framework::command::exception;

class GetComponents : public compute::GetComponents {
private:
    std::string m_id{};
public:
    GetComponents(const std::string& id) { m_id = id; }

    using compute::GetComponents::execute;

    void execute(const Request& request, Response& response) {
        const auto& uuid = request.get_component();

        if (!uuid.empty() && uuid != m_id) {
            throw exception::NotFound();
        }

        std::vector<Response::Component> vec_components{};

        for (std::uint32_t i = 0; i < 10; ++i) {
            vec_components.push_back(
                m_create_compute(std::to_string(i)));
        }

        response.set_components(vec_components);
    }

    virtual ~GetComponents();

private:
    Response::Component m_create_compute(const std::string& compute_name) {
        Response::Component compute;
        compute.set_type("Test Compute Type");
        compute.set_name(compute_name);

        std::vector<Response::Component> vec_blades;

        for (std::uint32_t i = 0; i < 12; ++i) {
            vec_blades.push_back(
                m_create_blade(compute_name + "_" + std::to_string(i)));
        }
        compute.set_components(vec_blades);
        return compute;
    }

    Response::Component m_create_blade(const string& blade_name) {
        Response::Component blade;
        blade.set_type("Test System Type");
        blade.set_name(blade_name);
        std::vector<Response::Component> vec_blades;
        blade.set_components(vec_blades);
        return blade;
    }

};

GetComponents::~GetComponents() { }

class GetComponentsTest : public ::testing::Test {
public:
    virtual ~GetComponentsTest();
};

GetComponentsTest::~GetComponentsTest() { }

TEST_F(GetComponentsTest, PositiveExecute) {
    compute::json::GetComponents command_json;
    GetComponents* command = new GetComponents("");

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params["id"] = "";

    EXPECT_NO_THROW(command_json.method(params, result));

    ASSERT_TRUE(result.isArray());
    ASSERT_EQ(result.size(), 10);
    for (std::uint32_t i = 0; i < 10; ++i) {
        ASSERT_EQ(result[i]["component"].asString(), std::to_string(i));
    }
}
