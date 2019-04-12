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
 * @file system.hpp
 *
 * @brief smbios structures describing system components
 * */

#pragma once

#include "smbios_structs.hpp"
#include <cstdint>

#pragma pack(push, 1)

namespace smbios {
namespace parser {

constexpr std::size_t SYSTEM_UUID_LENGTH = 16;

/*!
 * @brief Wake Up Type
 */
enum class WakeUpType : uint8_t {
    RESERVED = 0x00,
    OTHER = 0x01,
    UNKNOWN = 0x02,
    APM_TIMER = 0x03,
    MODEM_RING = 0x04,
    LAN_REMOTE = 0x05,
    POWER_SWITCH = 0x06,
    PCI_PME = 0x07,
    AC_POWER_RESTORED = 0x08
};

/*!
 * @brief System information data
 */
struct SYSTEM_INFO_DATA {
    uint8_t manufacturer;
    uint8_t product_name;
    uint8_t version;
    uint8_t serial_number;
    uint8_t uuid[SYSTEM_UUID_LENGTH];
    WakeUpType wakeup_type;
    uint8_t sku_number;
    uint8_t family;
};

/*!
 * @brief SMBIOS System information data (Type 1)
 */
struct SMBIOS_SYSTEM_INFO_DATA {
    static constexpr uint8_t ID = 1;
    SMBIOS_OOB_HEADER header;
    SYSTEM_INFO_DATA data;
};

}
}
#pragma pack(pop)

