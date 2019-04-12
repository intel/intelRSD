/*!
 * @brief ValueRounder test
 *
 *
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
 * @file value_rounder_test.cpp
 */

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "telemetry/value_rounder.hpp"

using namespace telemetry;

TEST(ValueRounderTest, DefaultRoundingCalculations) {
    const auto precision = 1.0;
    ValueRounder<> r(precision);
    EXPECT_EQ(1.0, r(0.5).get<double>());
    EXPECT_EQ(0.0, r(0.1).get<double>());
    EXPECT_EQ(11.0, r(10.5).get<double>());
}

TEST(ValueRounderTest, FloorRoundingCalculations) {
    const auto precision = 1.0;
    EXPECT_EQ(10, ValueRounder<std::floor>(precision)(10.5).get<double>());
}
