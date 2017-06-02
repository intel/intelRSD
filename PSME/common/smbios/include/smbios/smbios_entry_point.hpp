/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @file signal.hpp
 *
 * @brief Signal interface
 * */

#pragma once
#include <cstdint>

namespace smbios {
namespace parser {

#pragma pack(push, 1)

/*!
 * @brief Entry point to the SMBIOS table
 * */
struct SmbiosEntryPoint {
    uint8_t anchor[4];
    uint8_t checksum;
    uint8_t length;
    uint8_t major;
    uint8_t minor;
    uint16_t max_struct_size;
    uint8_t revision;
    uint8_t formatted_area[5];
    uint8_t intermediate_anchor[5];
    uint8_t intermediate_checksum;
    uint16_t struct_table_length;
    uint32_t struct_table_address;
    uint16_t struct_count;
    uint8_t bcd_revision;
};

#pragma pack(pop)

}
}

