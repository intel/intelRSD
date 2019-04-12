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
 * @file module.hpp
 *
 * @brief smbios structure describing module
 * */

#pragma once

#include "smbios_structs.hpp"
#include <cstdint>

#pragma pack(push, 1)

namespace smbios {
namespace parser {

/*!
 * @brief Board Type
 */
enum class BoardType : uint8_t {
    UNKNOWN = 0x01,
    OTHER = 0x02,
    SERVER_BLADE = 0x03,
    CONNECTIVITY_SWITCH = 0x04,
    SYSTEM_MANAGEMENT_MODULE = 0x05,
    PROCESSOR_MODULE = 0x06,
    IO_MODULE = 0x07,
    MEMORY_MODULE = 0x08,
    DAUGHTER_BOARD = 0x09,
    MOTHERBOARD = 0x0A,
    PROCESSOR_MEMORY_MODULE = 0x0B,
    PROCESSOR_IO_MODULE = 0x0C,
    INTERCONNECTED_BOARD = 0x0D
};

/*!
 * @brief MODULE information data
 */
struct MODULE_INFO_DATA {
    uint8_t manufacturer;
    uint8_t product;
    uint8_t version;
    uint8_t serial_number;
    uint8_t asset_tag;
    uint8_t feature_flags;
    uint8_t location_in_chassis;
    uint16_t chassis_handle;
    BoardType board_type;
    uint8_t number_of_contained_object_handles;
};

/*!
 * @brief SMBIOS MODULE information data (Type 2)
 */
struct SMBIOS_MODULE_INFO_DATA {
    static constexpr uint8_t ID = 2;
    SMBIOS_OOB_HEADER header;
    MODULE_INFO_DATA data;
};

}
}
#pragma pack(pop)
