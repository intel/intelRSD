/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * */

#include "psme/rest/validators/json_validator.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <agent-framework/module/enum/compute.hpp>


using namespace agent_framework::model::enums;
using namespace psme::rest::validators;

class JsonValidatorTest : public testing::Test {
public:
    void SetUp() { }

    void TearDown() { }

    ~JsonValidatorTest();
};

JsonValidatorTest::~JsonValidatorTest() { }

TEST_F(JsonValidatorTest, AllAllowableValuesPositive) {
    // Checks all enum elements.
    const auto& allowalbe_values = BootOverrideTarget::get_values();
    BootOverrideTarget target_to_check = BootOverrideTarget::Cd;

    ASSERT_FALSE(allowalbe_values.empty());
    ASSERT_TRUE(JsonValidator::validate_allowable_values(allowalbe_values, std::string(target_to_check.to_string())));
}

TEST_F(JsonValidatorTest, EmptyAllowableValuesPositive) {
    // Checks none of enum elements.
    const auto& allowalbe_values = std::vector<BootOverrideTarget>();
    BootOverrideTarget target_to_check = BootOverrideTarget::Cd;

    ASSERT_TRUE(allowalbe_values.empty());
    ASSERT_FALSE(JsonValidator::validate_allowable_values(allowalbe_values, target_to_check));
}

TEST_F(JsonValidatorTest, OneAllowableValuesPositive) {
    // Checks one enum elements.
    const auto& allowalbe_values = std::vector<BootOverrideTarget>{
        BootOverrideTarget::Cd
    };
    BootOverrideTarget target_to_check = BootOverrideTarget::Cd;

    ASSERT_FALSE(allowalbe_values.empty());
    ASSERT_TRUE(JsonValidator::validate_allowable_values(allowalbe_values, target_to_check));
}

TEST_F(JsonValidatorTest, OneAllowableValuesNegative) {
    // Checks one enum elements.
    const auto& allowalbe_values = std::vector<BootOverrideTarget>{
        BootOverrideTarget::Pxe
    };
    BootOverrideTarget target_to_check = BootOverrideTarget::Cd;

    ASSERT_FALSE(allowalbe_values.empty());
    ASSERT_FALSE(JsonValidator::validate_allowable_values(allowalbe_values, target_to_check));
}

TEST_F(JsonValidatorTest, SomeAllowableValuesPositive) {
    // Checks one enum elements.
    const auto& allowalbe_values = std::vector<BootOverrideTarget>{
        BootOverrideTarget::Pxe,
        BootOverrideTarget::Cd,
        BootOverrideTarget::BiosSetup
    };
    BootOverrideTarget target_to_check = BootOverrideTarget::Cd;

    ASSERT_FALSE(allowalbe_values.empty());
    ASSERT_TRUE(JsonValidator::validate_allowable_values(allowalbe_values, target_to_check));
}


TEST_F(JsonValidatorTest, SomeAllowableValuesNegative) {
    // Checks one enum elements.
    const auto& allowalbe_values = std::vector<BootOverrideTarget>{
        BootOverrideTarget::Pxe,
        BootOverrideTarget::Diags,
        BootOverrideTarget::BiosSetup
    };
    BootOverrideTarget target_to_check = BootOverrideTarget::Cd;

    ASSERT_FALSE(allowalbe_values.empty());
    ASSERT_FALSE(JsonValidator::validate_allowable_values(allowalbe_values, target_to_check));
}
