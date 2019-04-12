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

#include "command_test.hpp"
#include "command_json_test.hpp"

#include <vector>
#include <memory>

using std::vector;
using std::string;
using std::unique_ptr;

using namespace testing;

constexpr const char DummyCommandJsonMethod::AGENT[];
constexpr const char DummyCommandJsonMethod::TAG[];

DummyCommandJsonMethod::~DummyCommandJsonMethod() { }

constexpr const char DummyCommandJsonNotification::AGENT[];
constexpr const char DummyCommandJsonNotification::TAG[];

DummyCommandJsonNotification::~DummyCommandJsonNotification() { }

CommandJsonTest::~CommandJsonTest() { }

TEST_F(CommandJsonTest, PositiveAddCommandSingle) {
    EXPECT_NO_THROW(CommandJson::Register<DummyCommandJsonMethod> cmd1);
    EXPECT_NO_THROW(CommandJson::Register<DummyCommandJsonNotification> cmd2);

    auto& commands = CommandJson::Map::get_instance();
    EXPECT_NO_THROW(commands.at("Testing").at("DummyMethod"));
    EXPECT_NO_THROW(commands.at("Testing").at("DummyNotification"));
}

TEST_F(CommandJsonTest, PositiveSetCommand) {
    DummyCommandJsonMethod command_json;
    DummyCommand1* command = new DummyCommand1;

    EXPECT_NO_THROW(command_json.set_command(command));
    EXPECT_NO_THROW(command_json.call_get_command());
}

TEST_F(CommandJsonTest, NegativeGetCommand) {
    DummyCommandJsonMethod command;

    EXPECT_THROW(command.call_get_command(),
                    agent_framework::exceptions::NotImplemented);
}

TEST_F(CommandJsonTest, NegativeAddCommandOverrideExisting) {
    EXPECT_NO_THROW(CommandJson::Register<DummyCommandJsonMethod> cmd1);
    EXPECT_NO_THROW(CommandJson::Register<DummyCommandJsonMethod> cmd2);
}
