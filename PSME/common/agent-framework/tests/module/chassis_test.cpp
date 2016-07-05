/*!
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
 *
 * @file chassis_test.cpp
 *
 * @brief Unit test for Chassis class
 * */

#include "agent-framework/module/chassis.hpp"
#include "gtest/gtest.h"

#include <uuid++.hh>

using namespace agent_framework::generic;

/* Testing configuration */
static constexpr const char CHASSIS_CONFIGURATION[] =
R"({
    "type": "test",
    "size": 1,
    "locationOffset": 1,
    "computeZones": [
        {}
    ]
})";


/* Positive tests */
/* read_configuration test */

TEST(ChassisTest, PositiveReadConfiguration) {
    json::Value chassis_configuration;
    CHASSIS_CONFIGURATION >> chassis_configuration;

    Chassis chassis;
    EXPECT_NO_THROW(chassis.read_configuration(chassis_configuration));

    ASSERT_EQ(chassis_configuration["type"].as_string(), chassis.get_type());
    ASSERT_EQ(chassis_configuration["size"].as_uint(), chassis.get_size());
    ASSERT_EQ(chassis_configuration["locationOffset"].as_uint(),
            chassis.get_location_offset());
}

/* Negative tests */
/* read_configuration test */

TEST(ChassisTest, NegativeReadConfiguration) {
    const std::string empty_string = "";
    constexpr const unsigned int default_uint{};
    json::Value empty_configuration;

    Chassis chassis;
    EXPECT_NO_THROW(chassis.read_configuration(empty_configuration));

    ASSERT_EQ(empty_string, chassis.get_type());
    ASSERT_EQ(default_uint, chassis.get_size());
    ASSERT_EQ(default_uint, chassis.get_location_offset());
}

