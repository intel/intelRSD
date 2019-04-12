/*!
 * @brief smbios structures describing intel(R) speed select
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file speed_select_information.hpp
 */

#pragma once

#include "smbios_structs.hpp"
#include <cstdint>
#pragma pack(push, 1)


namespace smbios {
namespace parser {

struct SPEED_SELECT_CONFIGURATION {
    uint8_t configuration_number;
    uint8_t reserved[3];
    uint8_t high_priority_core_count;
    uint8_t high_priority_base_frequency;
    uint8_t low_priority_core_count;
    uint8_t low_priority_base_frequency;
    uint16_t max_tdp;
    uint8_t max_junction_temperature;
    uint8_t high_priority_code_apic_ids;
    uint8_t reserved_2[20];
};

struct SPEED_SELECT_INFO_DATA {
    uint8_t socket_number;
    uint8_t structure_version;
    uint8_t number_of_configs;
    uint8_t current_config;
    uint8_t reserved[8];
};

/*!
 * @brief SMBIOS Speed Select Information structure (Type 210)
 */
struct SMBIOS_SPEED_SELECT_INFO_DATA {
    static constexpr uint8_t ID = 210;
    SMBIOS_OOB_HEADER header;
    SPEED_SELECT_INFO_DATA data;
};


}
}
#pragma pack(pop)
