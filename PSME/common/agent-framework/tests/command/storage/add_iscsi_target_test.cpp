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

class MyAddIscsiTarget {
private:
    std::string m_initiator_iqn{};
    std::string m_target_iqn{};
public:
    MyAddIscsiTarget(std::string initiator_iqn, std::string target_iqn) {
        m_initiator_iqn = initiator_iqn;
        m_target_iqn = target_iqn;
    }

    void execute(const AddIscsiTarget::Request& request,
                 AddIscsiTarget::Response& response) {
        auto initiator_iqn = request.get_initiator_iqn();
        auto target_iqn = request.get_target_iqn();

        if (initiator_iqn != m_initiator_iqn || target_iqn != m_target_iqn) {
            throw std::runtime_error("Not found");
        }

        response.set_target("Test Target");
    }
};

static constexpr char INITIATOR_IQN[] = "initiatorIQN";
static constexpr char TARGET_IQN[] = "targetIQN";
static constexpr char AUTHENTICATION_METHOD[] = "authenticationMethod";
static constexpr char TARGET[] = "target";
static constexpr char OEM[] = "oem";

TEST(AddIscsiTargetTest, PositiveExecute) {
    MyAddIscsiTarget command{"TestInitiatorIQN", "TestTargetIQN"};
    AddIscsiTarget::Request request{};
    AddIscsiTarget::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[TARGET_IQN] = "TestTargetIQN";
    params[INITIATOR_IQN] = "TestInitiatorIQN";
    params[AUTHENTICATION_METHOD] = json::Json::null;

    EXPECT_NO_THROW(request = AddIscsiTarget::Request::from_json(params));
    EXPECT_NO_THROW(command.execute(request, response));
    EXPECT_NO_THROW(result = response.to_json());

    ASSERT_TRUE(result.isObject());
    ASSERT_TRUE(result[TARGET].isString());
    ASSERT_TRUE(result[OEM].isObject());
    ASSERT_EQ(result[TARGET].asString(), "Test Target");
}

TEST(AddIscsiTargetTest, NegativeTargetNotFound) {
    MyAddIscsiTarget command{"TestInitiatorIQN", "TestTargetIQN"};
    AddIscsiTarget::Request request{};
    AddIscsiTarget::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[TARGET_IQN] = "OtherTestTargetIQN";
    params[INITIATOR_IQN] = "TestInitiatorIQN";
    params[AUTHENTICATION_METHOD] = json::Json::null;

    EXPECT_NO_THROW(request = AddIscsiTarget::Request::from_json(params));
    EXPECT_ANY_THROW(command.execute(request, response));
}

TEST(AddIscsiTargetTest, NegativeInitiatortNotFound) {
    MyAddIscsiTarget command{"TestInitiatorIQN", "TestTargetIQN"};
    AddIscsiTarget::Request request{};
    AddIscsiTarget::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[TARGET_IQN] = "TestTargetIQN";
    params[INITIATOR_IQN] = "OtherTestInitiatorIQN";
    params[AUTHENTICATION_METHOD] = json::Json::null;

    EXPECT_NO_THROW(request = AddIscsiTarget::Request::from_json(params));
    EXPECT_ANY_THROW(command.execute(request, response));
}
