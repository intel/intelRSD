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

#include "agent-framework/command/storage/get_iscsi_target_info.hpp"
#include "agent-framework/command/storage/json/get_iscsi_target_info.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace agent_framework::model;
using namespace agent_framework::command;
using namespace agent_framework::command::exception;

class GetIscsiTargetInfo : public storage::GetIscsiTargetInfo {
private:
    std::string m_target{};

    agent_framework::model::IscsiTarget create_demo_target() {
        agent_framework::model::IscsiTarget target;
        std::vector<agent_framework::model::attribute::TargetLun> vec_lun;
        vec_lun.push_back(agent_framework::model::attribute::TargetLun());
        target.set_target_iqn("Test IscsiTarget IQN");
        target.set_target_lun(vec_lun);
        target.set_target_port("Test IscsiTarget Port");
        target.set_target_address("Test IscsiTarget Address");
        target.set_oem(agent_framework::model::attribute::Oem());
        target.set_initiator_iqn("Test Initiator IQN");

        return target;
    }
public:
    GetIscsiTargetInfo(const std::string& target) { m_target = target; }

    using storage::GetIscsiTargetInfo::execute;

    void execute(const Request& request, Response& response) {
        auto target = request.get_target();

        if (target != m_target) {
            throw exception::NotFound();
        }
        response.set_target(create_demo_target());
    }

    virtual ~GetIscsiTargetInfo();
};

GetIscsiTargetInfo::~GetIscsiTargetInfo() { }

TEST(GetIscsiTargetInfoTest, PositiveExecute) {
    storage::json::GetIscsiTargetInfo command_json;
    GetIscsiTargetInfo* command = new GetIscsiTargetInfo("TestIscsiTarget");

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::IscsiTarget::TARGET] = "TestIscsiTarget";

    EXPECT_NO_THROW(command_json.method(params, result));

    ASSERT_TRUE(result.isObject());
    ASSERT_TRUE(result[literals::IscsiTarget::TARGET_ADDRESS].isString());
    ASSERT_TRUE(result[literals::IscsiTarget::TARGET_PORT].isString());
    ASSERT_TRUE(result[literals::IscsiTarget::TARGET_IQN].isString());
    ASSERT_TRUE(result[literals::IscsiTarget::TARGET_LUNS].isArray());
    ASSERT_TRUE(result[literals::IscsiTarget::INITIATOR_IQN].isString());
    ASSERT_TRUE(result[literals::IscsiTarget::OEM].isObject());
    ASSERT_EQ(result[literals::IscsiTarget::TARGET_ADDRESS].asString(), "Test IscsiTarget Address");
    ASSERT_EQ(result[literals::IscsiTarget::TARGET_PORT].asString(), "Test IscsiTarget Port");
    ASSERT_EQ(result[literals::IscsiTarget::TARGET_IQN].asString(), "Test IscsiTarget IQN");
    ASSERT_EQ(result[literals::IscsiTarget::INITIATOR_IQN].asString(), "Test Initiator IQN");
}

TEST(GetIscsiTargetInfoTest, NegativeIscsiTargetNotFound) {
    storage::json::GetIscsiTargetInfo command_json;
    GetIscsiTargetInfo* command = new GetIscsiTargetInfo("TestIscsiTarget");

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::IscsiTarget::TARGET] = "OtherTestIscsiTarget";

    EXPECT_ANY_THROW(command_json.method(params, result));
}
