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
 * @file chassis.hpp
 *
 * @brief smbios structure describing system enclosure or chassis
 * */

#pragma once

#include "smbios_structs.hpp"
#include <cstdint>

#pragma pack(push, 1)

namespace smbios {
namespace parser {

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
    LUCH_BOX = 0x10,
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

/*!
 * @brief SYSTEM_ENCLOSURE_OR_CHASSIS information data
 */
struct SYSTEM_ENCLOSURE_OR_CHASSIS_DATA {
    uint8_t manufacturer;
    uint8_t type;
    uint8_t version;
    uint8_t serial_number;
    uint8_t asset_tag_number;
    uint8_t boot_up_state;
    uint8_t power_supply_state;
    uint8_t thermal_state;
    uint8_t security_status;
    uint32_t oem_defined;
    uint8_t height;
    uint8_t number_of_power_cords;
    uint8_t contained_element_count;
    uint8_t contained_element_record_length;
    uint8_t first_byte_of_the_rest_data;

    uint8_t get_sku() const {
        const uint8_t n = contained_element_count;
        const uint8_t m = contained_element_record_length;
        return *(reinterpret_cast<const uint8_t*>(&first_byte_of_the_rest_data) + n * m);
    }
};

/*!
 * @brief SYSTEM_ENCLOSURE_OR_CHASSIS information data (Type 3)
 */
struct SMBIOS_SYSTEM_ENCLOSURE_OR_CHASSIS {
    static constexpr uint8_t ID = 3;
    SMBIOS_OOB_HEADER header;
    SYSTEM_ENCLOSURE_OR_CHASSIS_DATA data;
};

}
}
#pragma pack(pop)
