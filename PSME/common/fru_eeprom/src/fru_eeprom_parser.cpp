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
 *
 * @file fru_eeprom_parser.cpp
 *
 * @brief FRU EEPROM Parser
 * */

#include "fru_eeprom/fru_eeprom_parser.hpp"
#include <safe-string/safe_lib.hpp>

#include <numeric>
#include <vector>

using namespace std;

namespace fru_eeprom {
namespace parser {

FruEepromParser::~FruEepromParser() {}
FruEepromParser::FruEepromException::~FruEepromException() { }

FruEepromParser::FruEepromParser(const uint8_t *buffer, const size_t size) {
    if (nullptr == buffer) {
        throw FruEepromException("Invalid FRU EEPROM blob pointer");
    }
    std::copy(buffer, buffer + size, std::back_inserter(m_buffer));
}

void FruEepromParser::parse() {
    parse_common_header_area();
    parse_internal_area();
    parse_chassis_info_area();
    parse_board_area();
    parse_product_info_area();
    parse_multirecord_area();
    parse_pad_area();
}

void FruEepromParser::parse_common_header_area() {
    check_crc(0, sizeof(CommonHeaderArea));

    m_common_header.format = read_byte(0, AREA_COMMON_HEADER_FORMAT_OFFSET);
    if (!is_area_format_valid(m_common_header.format)) {
        throw FruEepromException("Invalid FRU EEPROM format");
    }

    m_common_header.internal_use_area_offset = to_offset(read_byte(0, AREA_COMMON_HEADER_INTERNAL_USE_OFFSET));
    m_common_header.chassis_info_area_offset = to_offset(read_byte(0, AREA_COMMON_HEADER_CHASSIS_INFO_OFFSET));
    m_common_header.board_area_offset = to_offset(read_byte(0, AREA_COMMON_HEADER_BOARD_OFFSET));
    m_common_header.product_info_area_offset = to_offset(read_byte(0, AREA_COMMON_HEADER_PRODUCT_INFO_OFFSET));
    m_common_header.multirecord_area_offset = to_offset(read_byte(0, AREA_COMMON_HEADER_MULTIRECORD_OFFSET));
    m_common_header.pad = to_offset(read_byte(0, + AREA_COMMON_HEADER_PAD_OFFSET));
    m_common_header.crc = read_byte(0, AREA_COMMON_HEADER_CRC_OFFSET);
}

void FruEepromParser::parse_internal_area() {
    if (AREA_NOT_USED == m_common_header.internal_use_area_offset) {
        return;
    }
    // Our HW doesn't implement this area.
}

void FruEepromParser::parse_chassis_info_area() {
    auto area = m_common_header.chassis_info_area_offset;

    check_crc(area, m_chassis_info.length);

    parse_area_header<ChassisInfoArea>(m_chassis_info, area);
    m_chassis_info.type = SystemEnclosureOrChassisType(read_byte(area, AREA_CHASSIS_INFO_TYPE_OFFSET));

    auto offset = AREA_CHASSIS_INFO_PARAMETERS_OFFSET;
    offset = read_field(m_chassis_info.part_number, area, offset);
    offset = read_field(m_chassis_info.serial_number, area, offset);
    //Don`t parse any OEM fields and don`t check fields end byte (0xc1).
}

void FruEepromParser::parse_board_area() {
    auto area = m_common_header.board_area_offset;

    check_crc(area, m_board.length);

    parse_area_header<BoardArea>(m_board, area);

    m_board.code = read_byte(area, AREA_BOARD_LANG_CODE_OFFSET);

    auto offset = AREA_BOARD_PARAMETERS_OFFSET;
    m_board.data_time[0] = read_byte(area, AREA_BOARD_DATA_TIME_0_OFFSET);
    m_board.data_time[1] = read_byte(area, AREA_BOARD_DATA_TIME_1_OFFSET);
    m_board.data_time[2] = read_byte(area, AREA_BOARD_DATA_TIME_2_OFFSET);

    offset = read_field(m_board.manufacturer, area, offset);
    offset = read_field(m_board.product_name, area, offset);
    offset = read_field(m_board.serial_number, area, offset);
    offset = read_field(m_board.part_number, area, offset);
    offset = read_field(m_board.fru_file_id, area, offset);
    //Don`t parse any OEM fields and don`t check fields end byte (0xc1).
}

void FruEepromParser::parse_product_info_area() {
    auto area = m_common_header.product_info_area_offset;

    check_crc(area, m_product_info.length);

    parse_area_header<ProductInfoArea >(m_product_info, area);

    m_product_info.code = read_byte(area, AREA_PRODUCT_INFO_LANG_CODE_OFFSET);

    auto offset = AREA_PRODUCT_INFO_PARAMETERS_OFFSET;
    offset = read_field(m_product_info.manufacturer, area, offset);
    offset = read_field(m_product_info.product_name, area, offset);
    offset = read_field(m_product_info.model_number, area, offset);
    offset = read_field(m_product_info.product_version, area, offset);
    offset = read_field(m_product_info.serial_number, area, offset);
    offset = read_field(m_product_info.asset_tag, area, offset);
    offset = read_field(m_product_info.fru_file_id, area, offset);
    //Don`t parse any OEM fields and don`t check fields end byte (0xc1).
}

void FruEepromParser::parse_multirecord_area() {
    if (AREA_NOT_USED == m_common_header.multirecord_area_offset) {
        return;
    }
    // Our HW doesn't implement this area.
}

void FruEepromParser::parse_pad_area() {
    if (AREA_NOT_USED == m_common_header.pad) {
        return;
    }
    // Our HW doesn't implement this area.
}

template<typename T>
void FruEepromParser::parse_area_header(T& area, uint8_t offset) {
    if (AREA_NOT_USED == offset) {
        return;
    }
    area.format = read_byte(offset, AREA_FORMAT_OFFSET);
    if (!is_area_format_valid(area.format)) {
        throw FruEepromException("Invalid Chassis Info area format");
    }
    area.length = to_area_length(read_byte(offset, AREA_LENGTH_OFFSET));
    if (0 == area.length) {
        throw FruEepromException("Chassis info area defined but not populated");
    }
}

bool FruEepromParser::is_area_format_valid(const uint8_t format) {
    if (AREA_FORMAT_VERSION_1 == format) {
        return true;
    }
    return false;
}

uint8_t FruEepromParser::read_field(Field& field, const uint8_t area, const uint8_t offset) {
    uint8_t len = to_field_length(read_byte(area, offset));
    if (0 == len || 0xc1 == len) {
        // Field is empty or end of fields area.
        field.data_len = 0;
        field.size = 1;
        return uint8_t(offset + field.size);
    }

    // Shift pointer to field value skipping length byte
    auto src = &m_buffer[0] + area + offset + 1;
    auto ret = strncpy_s(field.data, len + 1, reinterpret_cast<const char*>(src), len);
    if (EOK != ret) {
        throw FruEepromException("Cannot copy data field");
    }
    field.data[len] = '\0';
    field.data_len = len;
    field.size = uint8_t(len + 1);

    return uint8_t(offset + field.size);
}

uint8_t FruEepromParser::read_byte(const uint8_t area, const uint8_t offset) {
    return *(&m_buffer[0] + area + offset);
}

void FruEepromParser::check_crc(const uint8_t start, const size_t size) {
    auto buffer = &m_buffer[0] + start;

    uint8_t sum = uint8_t(std::accumulate(buffer, buffer + size, 0));

    // Modulo 256 on sum provided by sum.size
    if (0 != sum) {
        throw FruEepromException("Invalid CRC sum");
    }
}

}
}
