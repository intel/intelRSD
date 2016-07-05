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

#include "agent-framework/command/storage/add_iscsi_target.hpp"
#include "agent-framework/command/storage/json/add_iscsi_target.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace agent_framework::command;
using namespace agent_framework::command::exception;

class AddIscsiTarget : public storage::AddIscsiTarget {
private:
    std::string m_initiator_iqn{};
    std::string m_target_iqn{};
public:
    AddIscsiTarget(std::string initiator_iqn,
                   std::string target_iqn) {
        m_initiator_iqn = initiator_iqn;
        m_target_iqn = target_iqn;
    }

    using storage::AddIscsiTarget::execute;

    void execute(const Request& request, Response& response) {
        auto initiator_iqn = request.get_initiator_iqn();
        auto target_iqn = request.get_target_iqn();

        if (initiator_iqn != m_initiator_iqn || target_iqn != m_target_iqn) {
            throw exception::NotFound();
        }

        response.set_target("Test Target");
        response.set_oem(agent_framework::model::attribute::Oem());
    }

    virtual ~AddIscsiTarget();
};

AddIscsiTarget::~AddIscsiTarget() { }

class AddIscsiTargetTest : public ::testing::Test {
protected:
    static constexpr char INITIATOR_IQN[] = "initiatorIQN";
    static constexpr char TARGET_IQN[] = "targetIQN";
    static constexpr char TARGET[] = "target";
    static constexpr char OEM[] = "oem";

    virtual ~AddIscsiTargetTest();
};

constexpr char AddIscsiTargetTest::INITIATOR_IQN[];
constexpr char AddIscsiTargetTest::TARGET_IQN[];
constexpr char AddIscsiTargetTest::TARGET[];
constexpr char AddIscsiTargetTest::OEM[];

AddIscsiTargetTest::~AddIscsiTargetTest() { }

TEST_F(AddIscsiTargetTest, PositiveExecute) {
    storage::json::AddIscsiTarget command_json;
    AddIscsiTarget* command = new AddIscsiTarget("TestInitiatorIQN", "TestTargetIQN");

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[TARGET_IQN] = "TestTargetIQN";
    params[INITIATOR_IQN] = "TestInitiatorIQN";

    EXPECT_NO_THROW(command_json.method(params, result));

    ASSERT_TRUE(result.isObject());
    ASSERT_TRUE(result[TARGET].isString());
    ASSERT_TRUE(result[OEM].isObject());
    ASSERT_EQ(result[TARGET].asString(), "Test Target");
}

TEST_F(AddIscsiTargetTest, NegativeTargetNotFound) {
    storage::json::AddIscsiTarget command_json;
    AddIscsiTarget* command = new AddIscsiTarget("TestInitiatorIQN", "TestTargetIQN");

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[TARGET_IQN] = "OtherTestTargetIQN";
    params[INITIATOR_IQN] = "TestInitiatorIQN";

    EXPECT_ANY_THROW(command_json.method(params, result));
}

TEST_F(AddIscsiTargetTest, NegativeInitiatortNotFound) {
    storage::json::AddIscsiTarget command_json;
    AddIscsiTarget* command = new AddIscsiTarget("TestInitiatorIQN", "TestTargetIQN");

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[TARGET_IQN] = "TestTargetIQN";
    params[INITIATOR_IQN] = "OtherTestInitiatorIQN";

    EXPECT_ANY_THROW(command_json.method(params, result));
}
