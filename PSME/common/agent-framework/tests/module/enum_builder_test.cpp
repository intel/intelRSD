/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file enum_builder_test.cpp
 */

#include "gtest/gtest.h"
#include "agent-framework/module/enum/enum_builder.hpp"



ENUM(OneValueEnum, uint32_t, OnlyPossibility);
ENUM(TwoValueEnum, uint32_t, One, Two);


using InvalidValue = agent_framework::exceptions::InvalidValue;


class EnumBuilderTest : public ::testing::Test {
public:
    virtual ~EnumBuilderTest();
};
EnumBuilderTest::~EnumBuilderTest() {}


TEST_F(EnumBuilderTest, TestEnumFromString) {
    ASSERT_NO_THROW(OneValueEnum::from_string("OnlyPossibility"));
    ASSERT_THROW(OneValueEnum::from_string("OtherPossibility"), InvalidValue);

    ASSERT_NO_THROW(TwoValueEnum::from_string("One"));
    ASSERT_NO_THROW(TwoValueEnum::from_string("Two"));
    ASSERT_THROW(TwoValueEnum::from_string("Three"), InvalidValue);
}


TEST_F(EnumBuilderTest, TestEnumToString) {
    OneValueEnum first{OneValueEnum::OnlyPossibility};
    ASSERT_EQ(first.to_string(), "OnlyPossibility");
    TwoValueEnum second{TwoValueEnum::One};
    ASSERT_EQ(second.to_string(), "One");
    TwoValueEnum third{TwoValueEnum::Two};
    ASSERT_EQ(third.to_string(), "Two");
}
