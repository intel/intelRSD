/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * @file pcie_port.hpp
 *
 * @brief smbios structures describing cabled pcie ports
 * */

#pragma once



#include <cstdint>



namespace smbios {
namespace parser {

#pragma pack(push, 1)


struct PCIE_PORT_INFO {
    uint16_t slot_id;
    uint8_t cable_port_link_width;
    uint8_t cable_index_count;
    uint8_t cable_port_index_0;
    uint8_t cable_port_index_0_start_lane;
    uint8_t cable_port_index_1;
    uint8_t cable_port_index_1_start_lane;
    uint8_t cable_port_index_2;
    uint8_t cable_port_index_2_start_lane;
    uint8_t cable_port_index_3;
    uint8_t cable_port_index_3_start_lane;
};

/*!
 * @brief SMBIOS PCIe Port Info (Type 199)
 */
struct SMBIOS_PCIE_PORT_INFO_DATA {
    static constexpr uint8_t ID = 199;
    SMBIOS_OOB_HEADER header;
    PCIE_PORT_INFO data;
};

#pragma pack(pop)

}
}
