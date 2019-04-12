/*!
 * @brief Test for the get_acpi_table_names
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file get_acpi_table_names_test.cpp
 */

#include "ipmi/command/sdv/rsd/get_acpi_table_names.hpp"
#include <gtest/gtest.h>

using namespace ipmi::command::sdv;
using namespace ipmi::command::sdv::rsd;

class GetAcpiTableNamesTest: public ::testing::Test {

protected:
    ipmi::IpmiInterface::ByteBuffer valid_response = {
        0x00, // Completion code
        0x04, // Intel RSD Specification
        0x0b, // Response length
        0x4e, 0x46, 0x49, 0x54, // Table name "NFIT"
        0x53, 0x52, 0x41, 0x54  // Table name "SRAT"
    };
    GetAcpiTableNamesTest();
    virtual ~GetAcpiTableNamesTest();
};

GetAcpiTableNamesTest::GetAcpiTableNamesTest() {
}

GetAcpiTableNamesTest::~GetAcpiTableNamesTest() {
}

TEST_F(GetAcpiTableNamesTest, RequestUnpacksCorrectly) {

    rsd::response::GetAcpiTableNames rsp {};
    rsp.do_unpack(valid_response);

    std::vector<std::string> expected_acpi_table_names = {"NFIT","SRAT"};
    ASSERT_EQ(expected_acpi_table_names, rsp.get_acpi_table_names());
    ASSERT_STREQ("GetAcpiTableNames", rsp.get_command_name());
}