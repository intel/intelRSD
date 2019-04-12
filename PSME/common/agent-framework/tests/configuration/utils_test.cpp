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

#include <cstdio>

#include "configuration/utils.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "json-wrapper/json-wrapper.hpp"

using ::testing::Return;
using ::testing::Throw;

class UtilsTest : public ::testing::Test {
protected:
    static constexpr char INVALID_STRING[] = "TestName";
    static constexpr char VALID_STRING[] = R"({
        "logger" : {
            "rest" : {
                "level" : {
                    "validator" : true,
                    "type" : "string",
                    "anyof": ["DEBUG", "ERROR", "INFO"]
                }
            }
        }
    })";
    static constexpr char TEST_FILE_NAME[] = "test_file";
    virtual ~UtilsTest();
};

constexpr char UtilsTest::INVALID_STRING[];
constexpr char UtilsTest::VALID_STRING[];
constexpr char UtilsTest::TEST_FILE_NAME[];

UtilsTest::~UtilsTest() {}

/* Positive. */

TEST_F(UtilsTest, PositiveJsonStringCasting) {
    json::Json json_string = json::Json();
    json::Json casted_json_string = json::Json();
    ASSERT_EQ(configuration::string_to_json(VALID_STRING, json_string), true);
    configuration::string_to_json(configuration::json_value_to_string(json_string), casted_json_string);
    ASSERT_EQ(casted_json_string, json_string);
}

TEST_F(UtilsTest, PositiveJsonFileCasting) {
    json::Json json_string = json::Json();
    json::Json casted_json_string = json::Json();
    ASSERT_NO_THROW(configuration::string_to_json(VALID_STRING, json_string));
    configuration::json_to_file(TEST_FILE_NAME, json_string);
    configuration::file_to_json(TEST_FILE_NAME, casted_json_string);
    ASSERT_EQ(casted_json_string, json_string);
}

/* Negative. */

TEST_F(UtilsTest, NegativeInvalidStringValue) {
    json::Json json_string = json::Json();
    ASSERT_EQ(configuration::string_to_json(INVALID_STRING, json_string), false);
}


