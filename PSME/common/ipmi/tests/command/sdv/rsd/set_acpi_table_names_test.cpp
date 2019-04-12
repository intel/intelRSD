/*!
 * @brief Test for the set_acpi_table_names
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
 * @file set_acpi_table_names_test.cpp
 */

#include "ipmi/command/sdv/rsd/set_acpi_table_names.hpp"
#include "ipmi/command/sdv/enums.hpp"
#include <gtest/gtest.h>


using namespace ipmi::command::sdv;
using namespace ipmi::command::sdv::rsd;


class SetAcpiTableNamesTest : public ::testing::Test {

protected:
    ipmi::IpmiInterface::ByteBuffer valid_response = {
        0x00, // Completion code
        0x04, // Intel RSD Specification
        0x13, // Response length
        0x4e, 0x46, 0x49, 0x54, // Table name "NFIT"
        0x53, 0x52, 0x41, 0x54, // Table name "SRAT"
        0x48, 0x4d, 0x41, 0x54, // Table name "HMAT"
        0x50, 0x43, 0x41, 0x54  // Table name "PCAT"
    };


    SetAcpiTableNamesTest();


    virtual ~SetAcpiTableNamesTest();
};


SetAcpiTableNamesTest::SetAcpiTableNamesTest() {
}


SetAcpiTableNamesTest::~SetAcpiTableNamesTest() {
}


TEST_F(SetAcpiTableNamesTest, RequestPacksCorrectly) {
    rsd::request::SetAcpiTableNames req{};

    std::vector <std::string> acpi_table_names = {"NFIT", "SRAT", "HMAT", "PCAT"};
    req.set_acpi_table_names(acpi_table_names);
    uint8_t data_length = static_cast<uint8_t>(3 + 4 * acpi_table_names.size());
    req.set_data_length(data_length);

    const auto& message = req.do_pack();
    ASSERT_EQ(static_cast<uint8_t>(GroupExtension::RACKSCALE), message[0]);
    ASSERT_EQ(data_length, message[1]);

    ipmi::IpmiInterface::ByteBuffer valid_request = {
        0x4e, 0x46, 0x49, 0x54, // Table name "NFIT"
        0x53, 0x52, 0x41, 0x54, // Table name "SRAT"
        0x48, 0x4d, 0x41, 0x54, // Table name "HMAT"
        0x50, 0x43, 0x41, 0x54  // Table name "PCAT"
    };

    std::vector<uint8_t>::const_iterator first = message.begin() + 3;
    std::vector<uint8_t> table_names(first, message.end());
    ASSERT_EQ(valid_request, table_names);
}


TEST_F(SetAcpiTableNamesTest, RequestUnpacksCorrectly) {
    rsd::response::SetAcpiTableNames rsp{};
    rsp.do_unpack(valid_response);

    std::vector<std::string> expected_acpi_table_names = {"NFIT", "SRAT", "HMAT", "PCAT"};
    ASSERT_EQ(expected_acpi_table_names, rsp.get_acpi_table_names());
    ASSERT_STREQ("SetAcpiTableNames", rsp.get_command_name());
}


