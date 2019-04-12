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

#include "configuration/schema_property.hpp"
#include "configuration/validators/max.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "json-wrapper/json-wrapper.hpp"

using ::testing::Return;
using ::testing::Throw;

class SchemaPropertyTest : public ::testing::Test {
protected:
    static constexpr const char TEST_NAME[] = "TestName";
    static constexpr const int VALID_TEST_VALUE = 5;
    static constexpr const int INVALID_TEST_VALUE = 15;
    static constexpr const int MAX_VALID_VALUE = 10;
    virtual ~SchemaPropertyTest();
};

constexpr const char SchemaPropertyTest::TEST_NAME[];
constexpr const int SchemaPropertyTest::VALID_TEST_VALUE;
constexpr const int SchemaPropertyTest::INVALID_TEST_VALUE;

SchemaPropertyTest::~SchemaPropertyTest() {}

/* Positive. */

TEST_F(SchemaPropertyTest, PositiveValidValue) {
    std::unique_ptr<configuration::SchemaProperty> schema_property(new configuration::SchemaProperty(TEST_NAME, false));
    json::Json json_val = VALID_TEST_VALUE;
    std::shared_ptr<configuration::MaxValidator> validator(new configuration::MaxValidator(MAX_VALID_VALUE));
    schema_property->add_validator(validator);
    configuration::SchemaErrors errors{};
    schema_property->validate(json_val, errors);
    ASSERT_EQ(errors.count(), 0);
}

/* Negative. */

TEST_F(SchemaPropertyTest, NegativeSchemaErrors) {
    std::unique_ptr<configuration::SchemaProperty> schema_property(new configuration::SchemaProperty(TEST_NAME, false));
    json::Json json_val = INVALID_TEST_VALUE;
    std::shared_ptr<configuration::MaxValidator> validator(new configuration::MaxValidator(MAX_VALID_VALUE));
    schema_property->add_validator(validator);
    configuration::SchemaErrors errors{};
    schema_property->validate(json_val, errors);
    ASSERT_NE(errors.count(), 0);
}


