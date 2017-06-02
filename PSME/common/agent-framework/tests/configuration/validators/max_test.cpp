/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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

#include "configuration/validators/max.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "json/json.hpp"

using namespace configuration;
using namespace json;

using ::testing::Return;
using ::testing::Throw;

class MaxValidatorTest : public ::testing::Test {
protected:
    static constexpr const int MAX_VALUE = 5;
    static constexpr const int ABOVE_LIMIT_VALUE = 10;
    static constexpr const int BELOW_LIMIT_VALUE = 0;
    virtual ~MaxValidatorTest();
};

MaxValidatorTest::~MaxValidatorTest() {}

/* Positive. */

TEST_F(MaxValidatorTest, PositiveBelowLimit) {
    std::unique_ptr<configuration::MaxValidator> max_validator(new configuration::MaxValidator(MAX_VALUE));
    json::Value json_int(BELOW_LIMIT_VALUE);
    ASSERT_EQ(max_validator->is_valid(json_int), true);
}

TEST_F(MaxValidatorTest, PositiveMarginalValue) {
    std::unique_ptr<configuration::MaxValidator> max_validator(new configuration::MaxValidator(MAX_VALUE));
    json::Value json_int(MAX_VALUE);
    ASSERT_EQ(max_validator->is_valid(json_int), true);
}

/* Negative. */

TEST_F(MaxValidatorTest, NegativeInvalidType) {
    std::unique_ptr<configuration::MaxValidator> max_validator(new configuration::MaxValidator(MAX_VALUE));
    json::Value json_int;
    ASSERT_EQ(max_validator->is_valid(json_int), false);
}

TEST_F(MaxValidatorTest, NegativeAboveLimit) {
    std::unique_ptr<configuration::MaxValidator> max_validator(new configuration::MaxValidator(MAX_VALUE));
    json::Value json_int(ABOVE_LIMIT_VALUE);
    ASSERT_EQ(max_validator->is_valid(json_int), false);
}
