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

#include "agent-framework/command/command_json.hpp"

namespace testing {

using namespace agent_framework::command;
using namespace agent_framework::command::exception;

class DummyCommandJsonMethod : public CommandJson {
public:

    static constexpr char AGENT[] = "Testing";
    static constexpr char TAG[] = "DummyMethod";

    DummyCommandJsonMethod() : CommandJson(
            Procedure("DummyMethod", jsonrpc::PARAMS_BY_NAME,
                jsonrpc::JSON_OBJECT,
                "test", jsonrpc::JSON_STRING, nullptr)) { }

    void method(const json::Json&, json::Json&) { }

    void notification(const json::Json&) { }

    Command* call_get_command() { return get_command(); }

    virtual ~DummyCommandJsonMethod();
};

class DummyCommandJsonNotification : public CommandJson {
public:

    static constexpr char AGENT[] = "Testing";
    static constexpr char TAG[] = "DummyNotification";

    DummyCommandJsonNotification() : CommandJson(
            Procedure("DummyNotification", jsonrpc::PARAMS_BY_NAME,
                "test", jsonrpc::JSON_STRING, nullptr)) { }

    void method(const json::Json&, json::Json&) { }

    void notification(const json::Json&) { }

    virtual ~DummyCommandJsonNotification();
};

class CommandJsonTest : public ::testing::Test {
public:
    virtual void SetUp() {
        CommandJson::Map::cleanup();
    }

    virtual ~CommandJsonTest();
};

} /* namespace testing */

