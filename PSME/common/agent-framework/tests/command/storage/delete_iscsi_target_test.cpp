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

class MyDeleteIscsiTarget {
private:
    std::string m_target{};
public:
    MyDeleteIscsiTarget(std::string target) { m_target = target; }

    void execute(const DeleteIscsiTarget::Request& request,
                 DeleteIscsiTarget::Response&) {
        auto target = request.get_uuid();

        if (target != m_target) {
            throw std::runtime_error("Not found");
        }
    }
};

static constexpr char TARGET[] = "target";
static constexpr char OEM[] = "oem";

TEST(DeleteIscsiTargetTest, PositiveExecute) {
    MyDeleteIscsiTarget command{"TestTarget"};
    DeleteIscsiTarget::Request request{""};
    DeleteIscsiTarget::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[TARGET] = "TestTarget";

    EXPECT_NO_THROW(request = DeleteIscsiTarget::Request::from_json(params));
    EXPECT_NO_THROW(command.execute(request, response));
    EXPECT_NO_THROW(result = response.to_json());

    ASSERT_TRUE(result.isObject());
    ASSERT_TRUE(result[OEM].isObject());
}

TEST(DeleteIscsiTargetTest, NegativeTargetNotFound) {
    MyDeleteIscsiTarget command{"TestTarget"};
    DeleteIscsiTarget::Request request{""};
    DeleteIscsiTarget::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[TARGET] = "OtherTestTarget";

    EXPECT_NO_THROW(request = DeleteIscsiTarget::Request::from_json(params));
    EXPECT_ANY_THROW(command.execute(request, response));
}
