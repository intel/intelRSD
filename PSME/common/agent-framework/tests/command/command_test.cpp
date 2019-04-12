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

#include <vector>
#include <memory>

using std::vector;
using std::string;
using std::unique_ptr;

using namespace testing;

constexpr const char DummyCommand1::AGENT[];
constexpr const char DummyCommand1::TAG[];

constexpr const char DummyCommand2::AGENT[];
constexpr const char DummyCommand2::TAG[];

DummyCommand1::~DummyCommand1() { }

DummyCommand2::~DummyCommand2() { }

CommandTest::~CommandTest() { }

TEST_F(CommandTest, PositiveAddCommandSingle) {
    EXPECT_NO_THROW(Command::Register<DummyCommand1> register_command1("DummyImplementation"));

    Command::Map::CommandMap& commands = Command::Map::get_instance();
    EXPECT_NO_THROW(commands.at("Testing").at("DummyImplementation").at("DummyCommand1"));
}

TEST_F(CommandTest, PositiveCommandExceptionWhat) {
    EXPECT_NE(NotFound().what(), nullptr);
    EXPECT_NE(InvalidCommand().what(), nullptr);
    EXPECT_NE(agent_framework::exceptions::NotImplemented({}).what(), nullptr);
    EXPECT_NE(InvalidArgument().what(), nullptr);
    EXPECT_NE(OverrideCommand().what(), nullptr);
}

TEST_F(CommandTest, PositiveAddCommandDifferentImplementations) {
    EXPECT_NO_THROW(Command::Register<DummyCommand1>
                    register_command1("DummyImplementation1"));
    EXPECT_NO_THROW(Command::Register<DummyCommand1>
                    register_command2("DummyImplementation2"));
}

TEST_F(CommandTest, PositiveAddCommandDifferentCommands) {
    EXPECT_NO_THROW(Command::Register<DummyCommand1>
                    register_command1("DummyImplementation"));
    EXPECT_NO_THROW(Command::Register<DummyCommand2>
                    register_command2("DummyImplementation"));
}

TEST_F(CommandTest, PositiveAddCommandOverrideExisting) {
    EXPECT_NO_THROW(Command::Register<DummyCommand1>
                    register_command1("DummyImplementation"));
    EXPECT_NO_THROW(Command::Register<DummyCommand1>
                    register_command2("DummyImplementation"));
}

TEST_F(CommandTest, NegativeCommandExecuteNotImplemented) {
    EXPECT_NO_THROW(Command::Register<DummyCommand1>
                    register_command1("DummyImplementation"));

    auto command = Command::Map::get_instance()
        .at("Testing").at("DummyImplementation").at("DummyCommand1")();

    Command::Argument dummyArgIn;
    Command::Argument dummyArgOut;

    EXPECT_THROW(command->execute(dummyArgIn, dummyArgOut),
                    agent_framework::exceptions::NotImplemented);
    delete command;
}

TEST_F(CommandTest, NegativeCommandExecuteInvalidArgument) {
    EXPECT_NO_THROW(Command::Register<DummyCommand1>
                    register_command1("DummyImplementation"));

    auto command = Command::Map::get_instance()
        .at("Testing").at("DummyImplementation").at("DummyCommand1")();

    Command::Argument& dummyArgIn =
        static_cast<DummyCommand1*>(command)->get_invalid_argument();
    Command::Argument dummyArgOut;

    EXPECT_THROW(command->execute(dummyArgIn, dummyArgOut), InvalidArgument);
    delete command;
}

TEST_F(CommandTest, NegativeGetCommandNotFound) {
    EXPECT_THROW(Command::Map::get_instance()
                 .at("Testing").at("DummyImplementation").at("DummyCommand1"),
                 std::out_of_range);
}
