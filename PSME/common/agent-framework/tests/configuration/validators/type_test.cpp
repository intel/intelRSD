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

#include "configuration/validators/type.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "json-wrapper/json-wrapper.hpp"

using namespace configuration;
using namespace json;

using ::testing::Return;
using ::testing::Throw;

class TypeValidatorTest : public ::testing::Test {
protected:
    static constexpr const int INT_VALUE = 5;
    static constexpr const uint UINT_VALUE = 5;
    static constexpr const bool BOOL_VALUE = true;
    static constexpr const char ARRAY_VALUE[] = "Testing";
    static constexpr const char STRING_VALUE[] = "TestString";
    virtual ~TypeValidatorTest();
};

constexpr const int TypeValidatorTest::INT_VALUE;
constexpr const uint TypeValidatorTest::UINT_VALUE;
constexpr const bool TypeValidatorTest::BOOL_VALUE;
constexpr const char TypeValidatorTest::ARRAY_VALUE[];
constexpr const char TypeValidatorTest::STRING_VALUE[];

TypeValidatorTest::~TypeValidatorTest() {}

/* Positive. */

TEST_F(TypeValidatorTest, PositiveIntValue) {
    std::unique_ptr<configuration::TypeValidator> type_validator(new configuration::TypeValidator("int"));
    json::Json json_val(INT_VALUE);
    ASSERT_EQ(type_validator->is_valid(json_val), true);
}

TEST_F(TypeValidatorTest, PositiveUintValue) {
    std::unique_ptr<configuration::TypeValidator> type_validator(new configuration::TypeValidator("uint"));
    json::Json json_val(UINT_VALUE);
    ASSERT_EQ(type_validator->is_valid(json_val), true);
}

TEST_F(TypeValidatorTest, PositiveStringValue) {
    std::unique_ptr<configuration::TypeValidator> type_validator(new configuration::TypeValidator("string"));
    json::Json json_val(STRING_VALUE);
    ASSERT_EQ(type_validator->is_valid(json_val), true);
}

TEST_F(TypeValidatorTest, PositiveBoolValue) {
    std::unique_ptr<configuration::TypeValidator> type_validator(new configuration::TypeValidator("bool"));
    json::Json json_val(BOOL_VALUE);
    ASSERT_EQ(type_validator->is_valid(json_val), true);
}

TEST_F(TypeValidatorTest, PositiveArrayValue) {
    std::unique_ptr<configuration::TypeValidator> type_validator(new configuration::TypeValidator("array"));
    json::Json json_val(INT_VALUE);
    json::Json json_array(INT_VALUE, json_val);
    ASSERT_EQ(type_validator->is_valid(json_array), true);
}

TEST_F(TypeValidatorTest, PositiveNullValue) {
    std::unique_ptr<configuration::TypeValidator> type_validator(new configuration::TypeValidator("null"));
    json::Json json_val(nullptr);
    ASSERT_EQ(type_validator->is_valid(json_val), true);
}

TEST_F(TypeValidatorTest, PositiveObjectValue) {
    std::unique_ptr<configuration::TypeValidator> type_validator(new configuration::TypeValidator("object"));
    json::Json json_val(INT_VALUE);
    json::Json json_object = json::Json();
    json_object.push_back(json::Json::object_t::value_type(ARRAY_VALUE, json_val));
    ASSERT_EQ(type_validator->is_valid(json_object), true);
}

/* Negative. */

TEST_F(TypeValidatorTest, NegativeIntValue) {
    std::unique_ptr<configuration::TypeValidator> type_validator(new configuration::TypeValidator("int"));
    json::Json json_val(BOOL_VALUE);
    ASSERT_EQ(type_validator->is_valid(json_val), false);
}

TEST_F(TypeValidatorTest, NegativeUintValue) {
    std::unique_ptr<configuration::TypeValidator> type_validator(new configuration::TypeValidator("uint"));
    json::Json json_val(BOOL_VALUE);
    ASSERT_EQ(type_validator->is_valid(json_val), false);
}

TEST_F(TypeValidatorTest, NegativeStringValue) {
    std::unique_ptr<configuration::TypeValidator> type_validator(new configuration::TypeValidator("string"));
    json::Json json_val(BOOL_VALUE);
    ASSERT_EQ(type_validator->is_valid(json_val), false);
}

TEST_F(TypeValidatorTest, NegativeBoolValue) {
    std::unique_ptr<configuration::TypeValidator> type_validator(new configuration::TypeValidator("bool"));
    json::Json json_val(INT_VALUE);
    ASSERT_EQ(type_validator->is_valid(json_val), false);
}

TEST_F(TypeValidatorTest, NegativeArrayValue) {
    std::unique_ptr<configuration::TypeValidator> type_validator(new configuration::TypeValidator("array"));
    json::Json json_val(BOOL_VALUE);
    ASSERT_EQ(type_validator->is_valid(json_val), false);
}

TEST_F(TypeValidatorTest, NegativeNullValue) {
    std::unique_ptr<configuration::TypeValidator> type_validator(new configuration::TypeValidator("null"));
    json::Json json_val(BOOL_VALUE);
    ASSERT_EQ(type_validator->is_valid(json_val), false);
}

TEST_F(TypeValidatorTest, NegativeObjectValue) {
    std::unique_ptr<configuration::TypeValidator> type_validator(new configuration::TypeValidator("object"));
    json::Json json_val(BOOL_VALUE);
    ASSERT_EQ(type_validator->is_valid(json_val), false);
}

TEST_F(TypeValidatorTest, NegativeTypeValue) {
    std::unique_ptr<configuration::TypeValidator> type_validator(new configuration::TypeValidator("type"));
    json::Json json_val(BOOL_VALUE);
    ASSERT_EQ(type_validator->is_valid(json_val), false);
}
