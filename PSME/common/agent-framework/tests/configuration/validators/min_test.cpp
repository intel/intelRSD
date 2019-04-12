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


#include "configuration/validators/min.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "json-wrapper/json-wrapper.hpp"

using namespace configuration;
using namespace json;
using ::testing::Return;
using ::testing::Throw;

class MinValidatorTest : public ::testing::Test {
protected:
    static constexpr const int MIN_VALUE = 5;
    static constexpr const int ABOVE_LIMIT_VALUE = 10;
    static constexpr const int BELOW_LIMIT_VALUE = 0;
    virtual ~MinValidatorTest();
};

constexpr const int MinValidatorTest::MIN_VALUE;
constexpr const int MinValidatorTest::ABOVE_LIMIT_VALUE;
constexpr const int MinValidatorTest::BELOW_LIMIT_VALUE;

MinValidatorTest::~MinValidatorTest() {}

/* Positive. */

TEST_F(MinValidatorTest, PositiveAboveLimit) {
    std::unique_ptr<configuration::MinValidator> min_validator(new configuration::MinValidator(MIN_VALUE));
    json::Json json_int(ABOVE_LIMIT_VALUE);
    ASSERT_EQ(min_validator->is_valid(json_int), true);
}

TEST_F(MinValidatorTest, PositiveMarginalValue) {
    std::unique_ptr<configuration::MinValidator> min_validator(new configuration::MinValidator(MIN_VALUE));
    json::Json json_int(MIN_VALUE);
    ASSERT_EQ(min_validator->is_valid(json_int), true);
}

/* Negative. */

TEST_F(MinValidatorTest, NegativeInvalidType) {
    std::unique_ptr<configuration::MinValidator> min_validator(new configuration::MinValidator(MIN_VALUE));
    json::Json json_int = json::Json();
    ASSERT_EQ(min_validator->is_valid(json_int), false);
}

TEST_F(MinValidatorTest, NegativeBelowLimit) {
    std::unique_ptr<configuration::MinValidator> min_validator(new configuration::MinValidator(MIN_VALUE));
    json::Json json_int(BELOW_LIMIT_VALUE);
    ASSERT_EQ(min_validator->is_valid(json_int), false);
}
