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


#include "test_data.hpp"

#include "fru_eeprom/fru_eeprom_parser.hpp"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <fstream>
#include <cstdint>

using ::testing::Return;
using ::testing::Throw;

using namespace std;
using namespace fru_eeprom::parser;

class FruEepromParserTest : public ::testing::Test {
protected:
    virtual void SetUp() {
    }

    virtual void TearDown() { }
    virtual ~FruEepromParserTest();
};

FruEepromParserTest::~FruEepromParserTest(){}

TEST_F(FruEepromParserTest, FruEepromParserCreateIsSucceesfull) {
    ASSERT_NO_THROW(FruEepromParser(&FRU_EEPROM_DATA_1[0], sizeof(FRU_EEPROM_DATA_1)));
}

TEST_F(FruEepromParserTest, FruEepromParserIncorrectDataPointerThrows) {
    ASSERT_THROW(FruEepromParser(nullptr, sizeof(FRU_EEPROM_DATA_1)), FruEepromParser::FruEepromException);
}

TEST_F(FruEepromParserTest, FruEepromParserParseIncorrectEepromFormatThrows) {
    auto parser = FruEepromParser(static_cast<const std::uint8_t*>(FRU_EEPROM_DATA_INVALID_DATA_FORMAT),
                                  sizeof(FRU_EEPROM_DATA_INVALID_DATA_FORMAT));
    ASSERT_THROW(parser.parse(), FruEepromParser::FruEepromException);
}

TEST_F(FruEepromParserTest, FruEepromParserParseCommonHeaderIsSuccessful) {
    auto parser = FruEepromParser(static_cast<const std::uint8_t*>(FRU_EEPROM_DATA_1), sizeof(FRU_EEPROM_DATA_1));
    ASSERT_NO_THROW(parser.parse());

    auto common_header = parser.get_common_header();
    ASSERT_EQ(static_cast<uint8_t>(common_header.format),
              FRU_EEPROM_DATA_1[AREA_COMMON_HEADER_FORMAT_OFFSET]);
    ASSERT_EQ(static_cast<uint8_t>(common_header.internal_use_area_offset),
              to_offset(FRU_EEPROM_DATA_1[AREA_COMMON_HEADER_INTERNAL_USE_OFFSET]));
    ASSERT_EQ(static_cast<uint8_t>(common_header.chassis_info_area_offset),
              to_offset(FRU_EEPROM_DATA_1[AREA_COMMON_HEADER_CHASSIS_INFO_OFFSET]));
    ASSERT_EQ(static_cast<uint8_t>(common_header.board_area_offset),
              to_offset(FRU_EEPROM_DATA_1[AREA_COMMON_HEADER_BOARD_OFFSET]));
    ASSERT_EQ(static_cast<uint8_t>(common_header.product_info_area_offset),
              to_offset(FRU_EEPROM_DATA_1[AREA_COMMON_HEADER_PRODUCT_INFO_OFFSET]));
    ASSERT_EQ(static_cast<uint8_t>(common_header.multirecord_area_offset),
              to_offset(FRU_EEPROM_DATA_1[AREA_COMMON_HEADER_MULTIRECORD_OFFSET]));
    ASSERT_EQ(static_cast<uint8_t>(common_header.pad),
              to_offset(FRU_EEPROM_DATA_1[AREA_COMMON_HEADER_PAD_OFFSET]));
    ASSERT_EQ(static_cast<uint8_t>(common_header.crc),
              FRU_EEPROM_DATA_1[AREA_COMMON_HEADER_CRC_OFFSET]);
}

TEST_F(FruEepromParserTest, FruEepromParserParseChassisInfo1IsSuccessful) {
    auto parser = FruEepromParser(static_cast<const std::uint8_t*>(FRU_EEPROM_DATA_1), sizeof(FRU_EEPROM_DATA_1));
    ASSERT_NO_THROW(parser.parse());

    auto chassis_info = parser.get_chassis_info();

    ASSERT_EQ(chassis_info.length, to_area_length(FRU_EEPROM_DATA_1[TEST_1_CHASSIS_INFO_AREA_LEN_OFFSET]));
    ASSERT_EQ(chassis_info.type, SystemEnclosureOrChassisType::RACK_MOUNT_CHASSIS);
    ASSERT_STREQ("", std::string(chassis_info.serial_number.data).c_str());
    ASSERT_STREQ("", std::string(chassis_info.part_number.data).c_str());
}

TEST_F(FruEepromParserTest, FruEepromParserParseChassisInfo2IsSuccessful) {
    auto parser = FruEepromParser(static_cast<const std::uint8_t*>(FRU_EEPROM_DATA_2), sizeof(FRU_EEPROM_DATA_2));
    ASSERT_NO_THROW(parser.parse());

    auto chassis_info = parser.get_chassis_info();

    ASSERT_EQ(chassis_info.length, to_area_length(FRU_EEPROM_DATA_2[TEST_2_CHASSIS_INFO_AREA_LEN_OFFSET]));
    ASSERT_EQ(chassis_info.type, SystemEnclosureOrChassisType::RACK_MOUNT_CHASSIS);
    ASSERT_STREQ("ABCD", std::string(chassis_info.part_number.data).c_str());
    ASSERT_STREQ("EFGH", std::string(chassis_info.serial_number.data).c_str());
}

TEST_F(FruEepromParserTest, FruEepromParserParseChassisInfo3IsSuccessful) {
    auto parser = FruEepromParser(static_cast<const std::uint8_t*>(FRU_EEPROM_DATA_3), sizeof(FRU_EEPROM_DATA_3));
    ASSERT_NO_THROW(parser.parse());

    auto chassis_info = parser.get_chassis_info();

    ASSERT_EQ(chassis_info.length, to_area_length(FRU_EEPROM_DATA_3[TEST_3_CHASSIS_INFO_AREA_LEN_OFFSET]));
    ASSERT_EQ(chassis_info.type, SystemEnclosureOrChassisType::RACK_MOUNT_CHASSIS);
    ASSERT_STREQ("", std::string(chassis_info.part_number.data).c_str());
    ASSERT_STREQ("BELUSSI2016", std::string(chassis_info.serial_number.data).c_str());
}

TEST_F(FruEepromParserTest, FruEepromParserParseBoard1IsSuccessful) {
    auto parser = FruEepromParser(static_cast<const std::uint8_t*>(FRU_EEPROM_DATA_1), sizeof(FRU_EEPROM_DATA_1));
    ASSERT_NO_THROW(parser.parse());

    auto board = parser.get_board();
    ASSERT_EQ(board.length, to_area_length(FRU_EEPROM_DATA_1[TEST_1_BOARD_AREA_LEN_OFFSET]));
    ASSERT_EQ(board.code, FRU_EEPROM_DATA_1[TEST_1_BOARD_AREA_LANG_CODE_OFFSET]);
    ASSERT_STREQ("Company", std::string(board.manufacturer.data).c_str());
    ASSERT_STREQ("Board", std::string(board.product_name.data).c_str());
    ASSERT_STREQ("0123456789ABCDEFG", std::string(board.serial_number.data).c_str());
    ASSERT_STREQ("Part 1V", std::string(board.part_number.data).c_str());
    ASSERT_STREQ("FRU ver. 0.3", std::string(board.fru_file_id.data).c_str());
}

TEST_F(FruEepromParserTest, FruEepromParserParseBoard2IsSuccessful) {
    auto parser = FruEepromParser(static_cast<const std::uint8_t*>(FRU_EEPROM_DATA_2), sizeof(FRU_EEPROM_DATA_2));
    ASSERT_NO_THROW(parser.parse());

    auto board = parser.get_board();
    ASSERT_EQ(board.length, to_area_length(FRU_EEPROM_DATA_2[TEST_2_BOARD_AREA_LEN_OFFSET]));
    ASSERT_EQ(board.code, FRU_EEPROM_DATA_2[TEST_2_BOARD_AREA_LANG_CODE_OFFSET]);
    ASSERT_STREQ("CompanyCompany", std::string(board.manufacturer.data).c_str());
    ASSERT_STREQ("", std::string(board.product_name.data).c_str());
    ASSERT_STREQ("0123456789ABCDEFG", std::string(board.serial_number.data).c_str());
    ASSERT_STREQ("", std::string(board.part_number.data).c_str());
    ASSERT_STREQ("FRU 0.8", std::string(board.fru_file_id.data).c_str());
}

TEST_F(FruEepromParserTest, FruEepromParserParseBoard3IsSuccessful) {
    auto parser = FruEepromParser(static_cast<const std::uint8_t*>(FRU_EEPROM_DATA_3), sizeof(FRU_EEPROM_DATA_3));
    ASSERT_NO_THROW(parser.parse());

    auto board = parser.get_board();
    ASSERT_EQ(board.length, to_area_length(FRU_EEPROM_DATA_3[TEST_3_BOARD_AREA_LEN_OFFSET]));
    ASSERT_EQ(board.code, FRU_EEPROM_DATA_3[TEST_3_BOARD_AREA_LANG_CODE_OFFSET]);
    ASSERT_STREQ("", std::string(board.manufacturer.data).c_str());
    ASSERT_STREQ("", std::string(board.product_name.data).c_str());
    ASSERT_STREQ("0123456789ABCDEFG", std::string(board.serial_number.data).c_str());
    ASSERT_STREQ("Part 1V", std::string(board.part_number.data).c_str());
    ASSERT_STREQ("", std::string(board.fru_file_id.data).c_str());
}

TEST_F(FruEepromParserTest, FruEepromParserParseProductInfo1IsSuccessful) {
    auto parser = FruEepromParser(static_cast<const std::uint8_t*>(FRU_EEPROM_DATA_1), sizeof(FRU_EEPROM_DATA_1));
    ASSERT_NO_THROW(parser.parse());

    auto product_info = parser.get_product_info();
    ASSERT_EQ(product_info.length, to_area_length(FRU_EEPROM_DATA_1[TEST_1_PRODUCT_INFO_AREA_LEN_OFFSET]));
    ASSERT_EQ(product_info.code, FRU_EEPROM_DATA_1[TEST_1_PRODUCT_INFO_AREA_LANG_CODE_OFFSET]);
    ASSERT_STREQ("Company", std::string(product_info.manufacturer.data).c_str());
    ASSERT_STREQ("Chassis", std::string(product_info.product_name.data).c_str());
    ASSERT_STREQ("1234567890", std::string(product_info.model_number.data).c_str());
    ASSERT_STREQ("Ver. 1.0", std::string(product_info.product_version.data).c_str());
    ASSERT_STREQ("0123456789ABCDEFG", std::string(product_info.serial_number.data).c_str());
    ASSERT_STREQ("Tag", std::string(product_info.asset_tag.data).c_str());
    ASSERT_STREQ("FRU ver. 0.3", std::string(product_info.fru_file_id.data).c_str());
}

TEST_F(FruEepromParserTest, FruEepromParserParseProductInfo2IsSuccessful) {
    auto parser = FruEepromParser(static_cast<const std::uint8_t*>(FRU_EEPROM_DATA_2), sizeof(FRU_EEPROM_DATA_2));
    ASSERT_NO_THROW(parser.parse());

    auto product_info = parser.get_product_info();
    ASSERT_EQ(product_info.length, to_area_length(FRU_EEPROM_DATA_2[TEST_2_PRODUCT_INFO_AREA_LEN_OFFSET]));
    ASSERT_EQ(product_info.code, FRU_EEPROM_DATA_2[TEST_2_PRODUCT_INFO_AREA_LANG_CODE_OFFSET]);
    ASSERT_STREQ("Company", std::string(product_info.manufacturer.data).c_str());
    ASSERT_STREQ("", std::string(product_info.product_name.data).c_str());
    ASSERT_STREQ("1234567890", std::string(product_info.model_number.data).c_str());
    ASSERT_STREQ("", std::string(product_info.product_version.data).c_str());
    ASSERT_STREQ("0123456789ABCDEFG", std::string(product_info.serial_number.data).c_str());
    ASSERT_STREQ("Tag", std::string(product_info.asset_tag.data).c_str());
    ASSERT_STREQ("", std::string(product_info.fru_file_id.data).c_str());
}

TEST_F(FruEepromParserTest, FruEepromParserParseProductInfo3IsSuccessful) {
    auto parser = FruEepromParser(static_cast<const std::uint8_t*>(FRU_EEPROM_DATA_3), sizeof(FRU_EEPROM_DATA_3));
    ASSERT_NO_THROW(parser.parse());

    auto product_info = parser.get_product_info();
    ASSERT_EQ(product_info.length, to_area_length(FRU_EEPROM_DATA_3[TEST_3_PRODUCT_INFO_AREA_LEN_OFFSET]));
    ASSERT_EQ(product_info.code, FRU_EEPROM_DATA_3[TEST_3_PRODUCT_INFO_AREA_LANG_CODE_OFFSET]);
    ASSERT_STREQ("", std::string(product_info.manufacturer.data).c_str());
    ASSERT_STREQ("F0A BaseBoard", std::string(product_info.product_name.data).c_str());
    ASSERT_STREQ("", std::string(product_info.model_number.data).c_str());
    ASSERT_STREQ("", std::string(product_info.product_version.data).c_str());
    ASSERT_STREQ("", std::string(product_info.serial_number.data).c_str());
    ASSERT_STREQ("Tag", std::string(product_info.asset_tag.data).c_str());
    ASSERT_STREQ("", std::string(product_info.fru_file_id.data).c_str());
}
