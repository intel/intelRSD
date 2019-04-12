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

#pragma once
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "agent-framework/command/command.hpp"
#include "agent-framework/exceptions/exception.hpp"

namespace testing {

using namespace agent_framework::command;
using namespace agent_framework::command::exception;

class DummyCommand1 : public Command {
private:
    std::unique_ptr<Argument> m_invalid_argument{new Argument};
public:

    static constexpr const char AGENT[] = "Testing";
    static constexpr const char TAG[] = "DummyCommand1";

    DummyCommand1() : Command() { }

    inline Argument& get_invalid_argument() const {
        return *m_invalid_argument.get();
    }

    virtual void execute(const Argument& in, Argument&) {
        if (&in == &get_invalid_argument()) {
            throw InvalidArgument();
        }

        throw agent_framework::exceptions::NotImplemented({});
    }

    virtual ~DummyCommand1();
};

class DummyCommand2 : public Command {
public:

    static constexpr const char AGENT[] = "Testing";
    static constexpr const char TAG[] = "DummyCommand2";

    DummyCommand2() : Command() { }

    virtual void execute(const Argument&, Argument&) {
        throw agent_framework::exceptions::NotImplemented({});
    }

    virtual ~DummyCommand2();
};

class CommandTest : public ::testing::Test {
public:
    virtual void SetUp() {
        Command::Map::cleanup();
    }

    virtual ~CommandTest();
};

}

