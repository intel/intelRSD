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

#include "agent-framework/command/storage/delete_iscsi_target.hpp"
#include "agent-framework/command/storage/json/delete_iscsi_target.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace agent_framework::command;
using namespace agent_framework::command::exception;

class DeleteIscsiTarget : public storage::DeleteIscsiTarget {
private:
    std::string m_target{};
public:
    DeleteIscsiTarget(std::string target) { m_target = target; }

    using storage::DeleteIscsiTarget::execute;

    void execute(const Request& request, Response& response) {
        auto target = request.get_target();

        if (target != m_target) {
            throw exception::NotFound();
        }

        response.set_oem(agent_framework::model::attribute::Oem());
    }

    virtual ~DeleteIscsiTarget();
};

DeleteIscsiTarget::~DeleteIscsiTarget() { }

class DeleteIscsiTargetTest : public ::testing::Test {
protected:
    static constexpr char TARGET[] = "target";
    static constexpr char OEM[] = "oem";

    virtual ~DeleteIscsiTargetTest();
};

constexpr char DeleteIscsiTargetTest::TARGET[];
constexpr char DeleteIscsiTargetTest::OEM[];

DeleteIscsiTargetTest::~DeleteIscsiTargetTest() { }

TEST_F(DeleteIscsiTargetTest, PositiveExecute) {
    storage::json::DeleteIscsiTarget command_json;
    DeleteIscsiTarget* command = new DeleteIscsiTarget("TestTarget");

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[TARGET] = "TestTarget";

    EXPECT_NO_THROW(command_json.method(params, result));

    ASSERT_TRUE(result.isObject());
    ASSERT_TRUE(result[OEM].isObject());
}

TEST_F(DeleteIscsiTargetTest, NegativeTargetNotFound) {
    storage::json::DeleteIscsiTarget command_json;
    DeleteIscsiTarget* command = new DeleteIscsiTarget("TestTarget");

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[TARGET] = "OtherTestTarget";

    EXPECT_ANY_THROW(command_json.method(params, result));
}
