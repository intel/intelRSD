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
 * @file hard_drive_test.cpp
 *
 * @brief Unit test for HardDrive class
 * */

#include "agent-framework/module/hard_drive.hpp"
#include "gtest/gtest.h"

#include <uuid++.hh>

using namespace agent_framework::generic;

/* Testing configuration */
static constexpr const char DRIVE_CONFIGURATION[] =
R"({
    "status":{
        "state":"Enabled",
        "health":"OK"
    },
    "interface":"SAS",
    "type":"SSD",
    "capacityGB":120,
    "rpm":0,
    "fruInfo":{
        "serialNumber":"CLVI3180000F120B",
        "manufacturer":"Intel",
        "modelNumber":"SSDMCEAC120A3",
        "partNumber":"SN03"
    },
    "oem":{

    }
})";


/* Positive tests */
/* read_configuration test */

TEST(HardDriveTest, PositiveReadConfiguration) {
    json::Value drive_configuration;
    DRIVE_CONFIGURATION >> drive_configuration;

    HardDrive hard_drive;
    EXPECT_NO_THROW(hard_drive.read_configuration(drive_configuration));

    ASSERT_EQ(drive_configuration["interface"].as_string(),
                hard_drive.get_interface());
    ASSERT_EQ(drive_configuration["type"].as_string(), hard_drive.get_type());
    ASSERT_EQ(drive_configuration["capacityGB"].as_uint(),
            hard_drive.get_capacity_gb());
    ASSERT_EQ(drive_configuration["rpm"].as_uint(), hard_drive.get_rpm());


    const auto& fru_info = hard_drive.get_fru_info();
    ASSERT_EQ(fru_info.get_serial_number(),
                drive_configuration["fruInfo"]["serialNumber"].as_string());
    ASSERT_EQ(fru_info.get_manufacturer(),
                drive_configuration["fruInfo"]["manufacturer"].as_string());
    ASSERT_EQ(fru_info.get_model_number(),
                drive_configuration["fruInfo"]["modelNumber"].as_string());
    ASSERT_EQ(fru_info.get_part_number(),
                drive_configuration["fruInfo"]["partNumber"].as_string());

    const auto& drive_status = hard_drive.get_status();
    ASSERT_EQ(drive_status.get_state(),
                drive_configuration["status"]["state"].as_string());
    ASSERT_EQ(drive_status.get_health(),
                drive_configuration["status"]["health"].as_string());
}

/* Negative tests */
/* read_configuration test */

TEST(HardDriveTest, NegativeReadConfiguration) {
    constexpr const unsigned int default_uint{};
    const std::string empty_string = "";
    json::Value drive_configuration;

    HardDrive hard_drive;
    EXPECT_NO_THROW(hard_drive.read_configuration(drive_configuration));

    ASSERT_EQ(empty_string, hard_drive.get_interface());
    ASSERT_EQ(empty_string, hard_drive.get_type());
    ASSERT_EQ(default_uint, hard_drive.get_capacity_gb());
    ASSERT_EQ(default_uint, hard_drive.get_rpm());

    const auto& fru_info = hard_drive.get_fru_info();
    ASSERT_EQ(fru_info.get_serial_number(), empty_string);
    ASSERT_EQ(fru_info.get_manufacturer(), empty_string);
    ASSERT_EQ(fru_info.get_model_number(), empty_string);
    ASSERT_EQ(fru_info.get_part_number(), empty_string);

    const auto& drive_status = hard_drive.get_status();
    ASSERT_EQ(drive_status.get_state(), empty_string);
    ASSERT_EQ(drive_status.get_health(), empty_string);
}
