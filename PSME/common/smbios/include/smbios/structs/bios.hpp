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
 * @file bios.hpp
 *
 * @brief smbios structures describing bios
 * */

#pragma once

#include "smbios_structs.hpp"
#include <cstdint>

#pragma pack(push, 1)

namespace smbios {
namespace parser {

constexpr uint8_t BIOS_CHARACTERISTICS_EXTENSION_COUNT = 2;

/*!
 * @brief Version
 */
struct VERSION {
    uint8_t major_release;
    uint8_t minor_release;
};

/*!
 * @brief BIOS information data
 */
struct BIOS_INFO_DATA {
    uint8_t vendor;
    uint8_t version;
    uint16_t starting_address_segment;
    uint8_t release_date;
    uint8_t rom_size;
    uint64_t characteristics;
    uint8_t characteristics_extension[BIOS_CHARACTERISTICS_EXTENSION_COUNT];
    VERSION system_bios_version;
    VERSION embedded_controller_firmware_version;
};

/*!
 * @brief SMBIOS BIOS information data (Type 0)
 */
struct SMBIOS_BIOS_INFO_DATA {
    static constexpr uint8_t ID = 0;
    SMBIOS_OOB_HEADER header;
    BIOS_INFO_DATA data;
};

}
}
#pragma pack(pop)

