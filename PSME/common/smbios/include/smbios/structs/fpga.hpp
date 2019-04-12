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
 * @file fpga.hpp
 *
 * @brief smbios structures describing fpga components
 * */

#pragma once



#include <cstdint>



namespace smbios {
namespace parser {

#pragma pack(push, 1)

/*!
 * @brief FPGA Type
 */
enum class FpgaType : uint8_t {
    INTEGRATED = 0x00,
    DISCRETE = 0x01,
    DISCRETE_WITH_SOC = 0x02,
};

/*!
 * @brief FPGA Status
 */
enum class FpgaStatus : uint8_t {
    DISABLED = 0x00,
    ENABLED = 0x01
};

/*!
 * @brief FPGA HSSI configuration
 */
enum class FpgaHssiConfiguration : uint8_t {
    NETWORKING = 0x00,
    PCIE = 0x01
};

/*!
 * @brief FPGA On Package Memory Technology
 */
enum class IntegratedMemoryTechnology : uint8_t {
    NONE = 0x00,
    EDRAM = 0x01,
    HBM = 0x02,
    HBM2 = 0x03
};

/*!
 * @brief FPGA Device
 *
 */
struct FPGA_DEVICE {
    uint8_t fpga_index;
    FpgaType fpga_type;
    FpgaStatus fpga_status;
    uint8_t socket_identifier;
    uint8_t fpga_vendor;
    uint8_t fpga_family;
    uint8_t fpga_model;
    uint8_t fpga_bit_stream_version;
    uint8_t fpga_hps_core_count;
    uint8_t reserved;
    uint8_t fpga_hps_isa;
    FpgaHssiConfiguration fpga_hssi_configuration;
    uint8_t fpga_hssi_port_count;
    uint8_t fpga_hssi_port_speed;
    uint8_t fpga_hssi_side_band_configuration;
    uint8_t fpga_reconfiguration_slots;
    uint16_t fpga_pcie_slot_number;
    uint8_t fpga_pcie_bus_identifier;
    uint8_t fpga_pcie_device_identifier;
    uint8_t fpga_pcie_function_identifier;
    uint32_t thermal_design_power;
    IntegratedMemoryTechnology memory_technology;
    uint32_t integrated_memory_capacity;
    uint16_t integrated_memory_speed;

};

/*!
 * @brief SMBIOS FPGA Device (Type 198)
 */
struct SMBIOS_FPGA_DATA {
    static constexpr uint8_t ID = 198;
    SMBIOS_OOB_HEADER header;
    FPGA_DEVICE data;
};

/*!
 * @brief FPGA Device, no reserved byte
 *
 */
struct FPGA_DEVICE_OEM {
    uint8_t fpga_index;
    FpgaType fpga_type;
    FpgaStatus fpga_status;
    uint8_t socket_identifier;
    uint8_t fpga_vendor;
    uint8_t fpga_family;
    uint8_t fpga_model;
    uint8_t fpga_bit_stream_version;
    uint8_t fpga_hps_core_count;
    uint8_t fpga_hps_isa;
    FpgaHssiConfiguration fpga_hssi_configuration;
    uint8_t fpga_hssi_port_count;
    uint8_t fpga_hssi_port_speed;
    uint8_t fpga_hssi_side_band_configuration;
    uint8_t fpga_reconfiguration_slots;
    uint16_t fpga_pcie_slot_number;
    uint8_t fpga_pcie_bus_identifier;
    uint8_t fpga_pcie_device_identifier;
    uint8_t fpga_pcie_function_identifier;
    uint32_t thermal_design_power;
    IntegratedMemoryTechnology memory_technology;
    uint32_t integrated_memory_capacity;
    uint16_t integrated_memory_speed;
};

/*!
 * @brief SMBIOS FPGA Device, without reserved byte (Type 198)
 */
struct SMBIOS_FPGA_DATA_OEM {
    static constexpr uint8_t ID = 198;
    SMBIOS_OOB_HEADER header;
    FPGA_DEVICE_OEM data;
};

#pragma pack(pop)

}
}
