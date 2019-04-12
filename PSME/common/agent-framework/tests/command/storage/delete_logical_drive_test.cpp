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

class MyDeleteLogicalDrive {
private:
    std::string m_drive{};
public:
    MyDeleteLogicalDrive(std::string drive) { m_drive = drive; }

    void execute(const DeleteLogicalDrive::Request& request,
                 DeleteLogicalDrive::Response&) {
        auto drive = request.get_uuid();

        if (drive != m_drive) {
            throw std::runtime_error("Not found");
        }
    }
};

static constexpr char DRIVE[] = "drive";
static constexpr char OEM[] = "oem";

TEST(DeleteLogicalDriveTest, PositiveExecute) {
    MyDeleteLogicalDrive command{"TestDrive"};
    DeleteLogicalDrive::Request request{""};
    DeleteLogicalDrive::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[DRIVE] = "TestDrive";

    EXPECT_NO_THROW(request = DeleteLogicalDrive::Request::from_json(params));
    EXPECT_NO_THROW(command.execute(request, response));
    EXPECT_NO_THROW(result = response.to_json());

    ASSERT_TRUE(result.isObject());
    ASSERT_TRUE(result[OEM].isObject());
}

TEST(DeleteLogicalDriveTest, NegativeDriveNotFound) {
    MyDeleteLogicalDrive command{"TestDrive"};
    DeleteLogicalDrive::Request request{""};
    DeleteLogicalDrive::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[DRIVE] = "OtherTestDrive";

    EXPECT_NO_THROW(request = DeleteLogicalDrive::Request::from_json(params));
    EXPECT_ANY_THROW(command.execute(request, response));
}
