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

#include <cstdio>

#include "configuration/schema_errors.hpp"
#include "configuration/validators/max.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "json/json.hpp"

using ::testing::Return;
using ::testing::Throw;

class SchemaErrorsTest : public ::testing::Test {
protected:
    static constexpr char TEST_NAME[] = "TestName";
    static constexpr char TEST_VALUE[] = "TestValue";
    static constexpr char TEST_PATH[] = "./schema_errors_test";

    virtual ~SchemaErrorsTest();
};

constexpr char SchemaErrorsTest::TEST_NAME[];
constexpr char SchemaErrorsTest::TEST_VALUE[];
constexpr char SchemaErrorsTest::TEST_PATH[];

SchemaErrorsTest::~SchemaErrorsTest() {}

/* Positive. */

TEST_F(SchemaErrorsTest, PositiveGetSetValue) {
    configuration::SchemaErrors::Error schema_error{TEST_NAME};

    ASSERT_NO_THROW(schema_error.set_value(TEST_VALUE));
    ASSERT_EQ(schema_error.get_value(), TEST_VALUE);
}

TEST_F(SchemaErrorsTest, PositiveGetSetPath) {
    configuration::SchemaErrors::Error schema_error{TEST_NAME};

    ASSERT_NO_THROW(schema_error.set_path(TEST_PATH));
    ASSERT_EQ(schema_error.get_path(), TEST_PATH);
}

TEST_F(SchemaErrorsTest, PositiveErrorsListNull) {
    configuration::SchemaErrors schema_errors;
    ASSERT_EQ(schema_errors.count(), 0);
}

TEST_F(SchemaErrorsTest, PositiveAddError) {
    configuration::SchemaErrors schema_errors;
    auto validator = std::make_shared<configuration::MaxValidator>(10);
    ASSERT_NO_THROW(schema_errors.add_error(validator->get_error()));
    ASSERT_NE(schema_errors.count(), 0);
}

TEST_F(SchemaErrorsTest, PositiveNoErrors) {
    configuration::SchemaErrors schema_errors;
    ASSERT_EQ(schema_errors.is_valid(), true);
}

/* Negative. */

TEST_F(SchemaErrorsTest, NegativeStoredErrors) {
    configuration::SchemaErrors schema_errors;
    auto validator = std::make_shared<configuration::MaxValidator>(10);
    ASSERT_NO_THROW(schema_errors.add_error(validator->get_error()));
    ASSERT_EQ(schema_errors.is_valid(), false);
}
