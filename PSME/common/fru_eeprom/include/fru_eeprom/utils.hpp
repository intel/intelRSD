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
 *
 * @file utils.hpp
 *
 * @brief FRU EEPROM Parser utils
 * */

#pragma once

namespace fru_eeprom {
namespace parser {

static constexpr uint8_t AREA_NOT_USED = 0x00;
static constexpr uint8_t AREA_FORMAT_MASK = 0x0f;
static constexpr uint8_t AREA_FIELD_LENGTH_MASK = 0x3f;
static constexpr uint8_t MULTIPLIER_BY_8_SHIFT = 3;
static constexpr uint8_t AREA_FORMAT_VERSION_1 = 0x01;

static constexpr uint8_t AREA_FORMAT_OFFSET = 0x00;
static constexpr uint8_t AREA_LENGTH_OFFSET = 0x01;

static constexpr uint8_t AREA_FIELD_MAX_SIZE = 64;

/* Common Header Are */
static constexpr uint8_t AREA_COMMON_HEADER_FORMAT_OFFSET = 0;
static constexpr uint8_t AREA_COMMON_HEADER_INTERNAL_USE_OFFSET = 1;
static constexpr uint8_t AREA_COMMON_HEADER_CHASSIS_INFO_OFFSET = 2;
static constexpr uint8_t AREA_COMMON_HEADER_BOARD_OFFSET = 3;
static constexpr uint8_t AREA_COMMON_HEADER_PRODUCT_INFO_OFFSET = 4;
static constexpr uint8_t AREA_COMMON_HEADER_MULTIRECORD_OFFSET = 5;
static constexpr uint8_t AREA_COMMON_HEADER_PAD_OFFSET = 6;
static constexpr uint8_t AREA_COMMON_HEADER_CRC_OFFSET = 7;

/* Chassis Info Area */
static constexpr uint8_t AREA_CHASSIS_INFO_TYPE_OFFSET = 2;
static constexpr uint8_t AREA_CHASSIS_INFO_PARAMETERS_OFFSET = 3;

/* Board Area */
static constexpr uint8_t AREA_BOARD_PARAMETERS_OFFSET = 6;
static constexpr uint8_t AREA_BOARD_LANG_CODE_OFFSET = 2;
static constexpr uint8_t AREA_BOARD_DATA_TIME_0_OFFSET = 3;
static constexpr uint8_t AREA_BOARD_DATA_TIME_1_OFFSET = 4;
static constexpr uint8_t AREA_BOARD_DATA_TIME_2_OFFSET = 5;

/* Product Info Area */
static constexpr uint8_t AREA_PRODUCT_INFO_PARAMETERS_OFFSET = 3;
static constexpr uint8_t AREA_PRODUCT_INFO_LANG_CODE_OFFSET = 2;

enum class SystemEnclosureOrChassisType : uint8_t {
    OTHER = 0x01,
    UNKNOWN = 0x02,
    DESKTOP = 0x03,
    LOW_PROFILE_DESKTOP = 0x04,
    PIZZA_BOX = 0x05,
    MINI_TOWER = 0x06,
    TOWER = 0x07,
    PORTABLE = 0x08,
    LAPTOP = 0x09,
    NOTEBOOK = 0x0a,
    HANDHELD = 0x0b,
    DOCKING_STATION = 0x0c,
    ALL_IN_ONE = 0x0d,
    SUB_NOTEBOOK = 0x0e,
    SPACE_SAVING = 0x0f,
    LUNCH_BOX = 0x10,
    MAIN_SERVER_CHASSIS = 0x11,
    EXPANSION_CHASSIS = 0x12,
    SUB_CHASSIS = 0x13,
    BUS_EXPANSION_CHASSIS = 0x14,
    PERIPHERAL_CHASSIS = 0x15,
    RAID_CHASSIS = 0x16,
    RACK_MOUNT_CHASSIS = 0x17,
    SEALED_CASE_PC = 0x18,
    MULTI_SYSTEM_CHASSIS = 0x19,
    COMPACT_PCI = 0x1a,
    ADVANCED_TCA = 0x1b
};

struct Field {
    uint8_t data_len{};
    uint8_t size{};
    char data[AREA_FIELD_MAX_SIZE]{};
};

inline uint8_t to_area_length(const uint8_t length) {
    return uint8_t(length << MULTIPLIER_BY_8_SHIFT);
}

inline uint8_t to_field_length(const uint8_t length) {
    return uint8_t(length & AREA_FIELD_LENGTH_MASK);
}

inline uint8_t to_offset(const uint8_t offset) {
    return uint8_t(offset << MULTIPLIER_BY_8_SHIFT);
}

}
}
