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
using namespace agent_framework::model;

class MyGetIscsiTargetInfo {
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
    MyGetIscsiTargetInfo(const std::string& target) { m_target = target; }

    void execute(const GetIscsiTargetInfo::Request& request,
                 GetIscsiTargetInfo::Response& response) {
        auto target = request.get_uuid();

        if (target != m_target) {
            throw std::runtime_error("Not found");
        }
        response = create_demo_target();
    }
};

TEST(GetIscsiTargetInfoTest, PositiveExecute) {
    MyGetIscsiTargetInfo command{"TestIscsiTarget"};
    GetIscsiTargetInfo::Request request{""};
    GetIscsiTargetInfo::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::IscsiTarget::TARGET] = "TestIscsiTarget";

    EXPECT_NO_THROW(request = GetIscsiTargetInfo::Request::from_json(params));
    EXPECT_NO_THROW(command.execute(request, response));
    EXPECT_NO_THROW(result = response.to_json());

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
    MyGetIscsiTargetInfo command{"TestIscsiTarget"};
    GetIscsiTargetInfo::Request request{""};
    GetIscsiTargetInfo::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::IscsiTarget::TARGET] = "OtherTestIscsiTarget";

    EXPECT_NO_THROW(request = GetIscsiTargetInfo::Request::from_json(params));
    EXPECT_ANY_THROW(command.execute(request, response));
}
