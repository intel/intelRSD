/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * @section DESCRIPTION
 * */
#ifndef PSME_FW_BUILD_TEST_FIXTURE_HPP
#define PSME_FW_BUILD_TEST_FIXTURE_HPP

#include "smbios/smbios_parser.hpp"
#include "smbios/smbios_21_entry_point.hpp"
#include "smbios/smbios_30_entry_point.hpp"

namespace fixture {

union FixtureEntryPoint  {
    uint8_t buffer[sizeof(smbios::parser::Smbios21EntryPoint::EntryPointStructure)];
    smbios::parser::Smbios21EntryPoint::EntryPointStructure entry_point;
};

union Fixture64EntryPoint  {
    uint8_t buffer[sizeof(smbios::parser::Smbios30EntryPoint::EntryPointStructure)];
    smbios::parser::Smbios30EntryPoint::EntryPointStructure entry_point;
};

static const FixtureEntryPoint entry_point = {{
    0x5f, 0x53, 0x4d, 0x5f, 0xef, 0x1f, 0x02, 0x08, 0x8a, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x5f, 0x44, 0x4d, 0x49, 0x5f, 0x52, 0x7f+0x2f, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x41, 0x00, 0x27
}};


static const FixtureEntryPoint entry_point_32 = {{
    0x5f, 0x53, 0x4d, 0x5f,         // anchor string "_SM_"
    0xef,                           // checksum
    0x1f,                           // length
    0x02,                           // major version
    0x08,                           // minor version
    0x8a, 0x00,                     // maximum structure size
    0x00,                           // entry point revision
    0x00, 0x00, 0x00, 0x00, 0x00,   // formatted area
    0x5f, 0x44, 0x4d, 0x49, 0x5f,   // intermediate anchor string "_DMI_"
    0x50,                           // intermediate checksum
    0x7f+(0x41-0x07), 0x0f,         // structure table length
    0x20, 0x00, 0x00, 0x00,         // structure table address
    0x08, 0x00,                     // number of smbios structures
    0x28                            // smbios BCD revision
}};


static const uint8_t entry_point_3_0[] = {
    0x5f, 0x53, 0x4d, 0x33, 0x5f,   // anchor string "_SM3_"
    0x35,                           // checksum
    0x18,                           // length
    0x03,                           // major version
    0x00,                           // minor version
    0x00,                           // smbios docrev
    0x01,                           // entry point revision
    0x00,                           // reserved
    0x0c, 0x00, 0x00, 0x00,         // structure table max size
    0x12, 0x00, 0x00, 0x00,         // struct table address
    0x00, 0x00, 0x00, 0x00
};


static const uint8_t entry_point_3_0_unknown_revision[] = {
    0x5f, 0x53, 0x4d, 0x33, 0x5f,   // anchor string "_SM3_"
    0x34,                           // checksum
    0x18,                           // length
    0x03,                           // major version
    0x00,                           // minor version
    0x00,                           // smbios docrev
    0x02,                           // entry point revision
    0x00,                           // reserved
    0x0c, 0x00, 0x00, 0x00,         // structure table max size
    0x12, 0x00, 0x00, 0x00,         // struct table address
    0x00, 0x00, 0x00, 0x00
};

static const uint8_t smbios[] = {
        /*
         * 2.1 (32-bit) entry point structure
         */
        0x5f, 0x53, 0x4d, 0x5f,     // anchor string "_SM_"
        0xef,                       // checksum
        0x1f,                       // length
        0x02,                       // major version
        0x08,                       // minor version
        0x8a, 0x00,                 // maximum structure size
        0x00,                       // entry point revision
        0x00, 0x00, 0x00, 0x00, 0x00, // formatted area
        0x5f, 0x44, 0x4d, 0x49, 0x5f, // intermediate anchor string "_DMI_"
        0x49,                       // intermediate checksum
        0x7f+(0x41-0x07), 0x0f,     // structure table length
        0x20, 0x00, 0x00, 0x00,     // structure table address
        0x0F, 0x00,                 // number of smbios structures
        0x28,                       // smbios BCD revision
        0x00,                       // align to 32-bits

        /*
        SMBIOS_SYSTEM_INFO_DATA	@1
            UUID            : 70f47d7a9577e411a2d12c60c73669a
            Wake-up Type    : 0x
         */
        0x01, 0x1b, 0x01, 0x00, 0x01, 0x02, 0x03, 0x04, 0x70, 0xf4, 0x7d, 0x7a,
        0x95, 0x77, 0xe4, 0x11, 0xa2, 0xd1, 0x2c, 0x60, 0x0c, 0x73, 0x66, 0x9a,
        0x06, 0x05, 0x06,

        //Manufacturer   : ABCDEF
        0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x00,

        //Product Name    : XXXX_XXX
        0x58, 0x58, 0x58, 0x58, 0x5f, 0x58, 0x58, 0x58, 0x00,

        //Version         : To be filled by O.E.M.
        0x54, 0x6f, 0x20, 0x62, 0x65, 0x20, 0x66, 0x69, 0x6c, 0x6c, 0x65, 0x64,
        0x20, 0x62, 0x79, 0x20, 0x4f, 0x2e, 0x45, 0x2e, 0x4d, 0x2e, 0x00,

        //Serial Number   : 6666666666666
        0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
        0x36, 0x00,

        //SKU Number      : To be filled by O.E.M.
        0x54, 0x6f, 0x20, 0x62, 0x65, 0x20, 0x66, 0x69, 0x6c, 0x6c, 0x65, 0x64,
        0x20, 0x62, 0x79, 0x20, 0x4f, 0x2e, 0x45, 0x2e, 0x4d, 0x2e, 0x00,

        //Family          : Server
        0x53, 0x65, 0x72, 0x76, 0x65, 0x72,
        0x00, 0x00,

        /*
        PCIE_INFO_DATA	@108
            PciClassCode    : 0x44
            SlotNo          : 0xee
            VendorID        : 0x2211
            DeviceID        : 0x5566
            SubVendorID     : 0x1133
            SubDeviceID     : 0x0
            LinkSpeed       : 8
            LinkWidth       : 18
         */
        0xc0, 0x17, 0x6c, 0x00, 0x44, 0xee, 0x00, 0x11, 0x22, 0x66, 0x55, 0x33,
        0x11, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00,
        0x00,

        /*
            PCIE_INFO_DATA	@109
            PciClassCode    : 0x2
            SlotNo          : 0x4
            VendorID        : 0x2211
            DeviceID        : 0x4321
            SubVendorID     : 0x152d
            SubDeviceID     : 0x89f2
            LinkSpeed       : 11
            LinkWidth       : 10
         */
        0xc0, 0x17, 0x6d, 0x00, 0x02, 0x04, 0x00, 0x11, 0x22, 0x21, 0x43, 0x2d,
        0x15, 0xf2, 0x89, 0x11, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00,
        0x00,

        /*
        PCIE_INFO_DATA	@110
        PciClassCode    : 0x3
        SlotNo          : 0x0
        VendorID        : 0x8a44
        DeviceID        : 0x8000
        SubVendorID     : 0x1a03
        SubDeviceID     : 0x2000
        LinkSpeed       : 1
        LinkWidth       : 1
        */
        0xc0, 0x17, 0x6e, 0x00, 0x03, 0x00, 0x00, 0x44, 0x8a, 0x00, 0x80, 0x03,
        0x1a, 0x00, 0x20, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
        0x00,

        /*
        NIC_INFO_DATA	@111
        PciClassCode    : 0x2
        SlotNo          : 0xff
        VendorID        : 0x1234
        DeviceID        : 0x9999
        SubVendorID     : 0x152d
        SubDeviceID     : 0x89f2
        LinkSpeed       : 1
        LinkWidth       : 1
        PortIndex       : 0
        MAC             : cc60c736699
        */
        0xbe,                           // 190
        57,                             // length
        0x6f, 0x00,                     // handle
        0x02,                           // class code
        0xff, 0x00,                     // slot number
        0x34, 0x12,                     // vendor id
        0x99, 0x99,                     // device id
        0x2d, 0x15,                     // subvendor if
        0xf2, 0x89,                     // subdevice id
        0x01, 0x00, 0x00, 0x00,         // link speed
        0x01, 0x00, 0x00, 0x00,         // link width
        0x00, 0x00,                     // port index
        0xcc, 0x60, 0x0c, 0x73, 0x66, 0x99, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00,

        /*
        NIC_INFO_DATA_V2	@112
        PciClassCode    : 0x2
        SlotNo          : 0x1e
        VendorID        : 0x2244
        DeviceID        : 0x7654
        SubVendorID     : 0x1155
        SubDeviceID     : 0x0
        LinkSpeed       : 3
        LinkWidth       : 8
        PortIndex       : 0
        MAC             : 2c60c79e544
        FW version      : "VERSION_123"
        */
        0xbe,
        58,
        0x70, 0x00,
        0x02,
        0x1e, 0x00,
        0x44, 0x22,
        0x54, 0x76,
        0x55, 0x11,
        0x00, 0x00,
        0x03, 0x00, 0x00, 0x00,
        0x08, 0x00, 0x00, 0x00,
        0x00, 0x00,
        0x2c, 0x60, 0x0c, 0x79, 0xe5, 0x44, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        1,
        'V', 'E', 'R', 'S', 'I', 'O', 'N', '_', '1', '2', '3',
        0x00, 0x00,

        /*
        HDD_INFO_DATA	@113
        PciClassCode    : 0x1
        VendorID        : 0x5432
        DeviceID        : 0x1111
        SubVendorID     : 0x152d
        SubDeviceID     : 0x77ff
        HDDIndex        : 4
        HDDName         : IMPEL SSD
        HDDType         : 0x0
        HDDSizeInGB     : 120
         */
        0xbf, 0x3a, 0x71, 0x00, 0x01, 0x32, 0x54, 0x11, 0x11, 0x2d, 0x15, 0xff,
        0x77, 0x04, 0x49, 0x4d, 0x50, 0x45, 0x4c, 0x20, 0x53, 0x53, 0x44, 0x00,
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
        0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00,

        /*
        HDD_INFO_DATA_V2   @114
        PciClassCode    : 0x1
        PciSubClassCode : 0x0
        VendorID        : 0x5432
        DeviceID        : 0x1111
        SubVendorID     : 0x152d
        SubDeviceID     : 0x77ff
        HDDIndex        : 0
        HDDName         : INTEL SSD
        HDDSerialNumber : VCIL134410FX21E0
        HDDType         : 0x0
        HDDSizeInGB     : 120
        */
        0xbf, 0x50, 0x72, 0x00, 0x01, 0x00, 0x32, 0x54, 0x11, 0x11, 0x2d, 0x15, 0xff, 0x77, 0x00, 0x49,
        0x4d, 0x50, 0x45, 0x4c, 0x20, 0x53, 0x53, 0x44, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x56, 0x43, 0x49, 0x4c, 0x31, 0x33, 0x34, 0x34, 0x31,
        0x30, 0x46, 0x58, 0x32, 0x31, 0x45, 0x30, 0x20, 0x20, 0x20, 0x20, 0x00, 0x78, 0x00, 0x00, 0x00,
        0x00, 0x00,

        /*
        FPGA_DEVICE     @115
        */
        0xc6,                   // FPGA
        36,                     // length
        0x73, 0x00,             // handle
        1,                      // uint8_t fpga_index;
        0,                      // FpgaType fpga_type;
        1,                      // FpgaStatus fpga_status;
        58,                     // uint8_t socket_identifier;
        1,                      // uint8_t fpga_vendor;
        2,                      // uint8_t fpga_family;
        3,                      // uint8_t fpga_model;
        4,                      // uint8_t fpga_bit_stream_version;
        0x22,                   // uint8_t fpga_hps_core_count;
        0,                      // uint8_t reserved
        0x03,                   // uint8_t fpga_hps_isa;
        0x00,                   // uint8_t fpga_hssi_configuration;
        0x07,                   // uint8_t fpga_hssi_port_count;
        0x08,                   // uint8_t fpga_hssi_port_speed;
        5,                      // uint8_t fpga_hssi_side_band_configuration;
        0x36,                   // uint8_t fpga_reconfiguration_slots;
        0x26, 0x76,             // uint16_t fpga_pcie_slot_number;
        0x66,                   // uint8_t fpga_pcie_bus_identifier;
        0x55,                   // uint8_t fpga_pcie_device_identifier;
        0x21,                   // uint8_t fpga_pcie_function_identifier;
        0x02, 0x32, 0x61, 0x12, // uint32_t thermal_design_power;
        0x02,                   // IntegratedMemoryTechnology memory_technology;
        0x01, 0x55, 0x47, 0x38, // uint32_t integrated_memory_capacity;
        0x42, 0x63,             // uint16_t integrated_memory_speed;
        'A', 'B', 0x00,         // "AB"
        'C', 'D', 0x00,         // "CD"
        'E', 'F', 'F', 0x00,    // "EFF"
        'G', 'H', 'H', 0x00,    // "GHH"
        'I', 'P', 'Q', 0x00,    // "IPQ
        0x00,

        /*
        STORAGE_DEVICE     @116
        */
        0xc2,                   // Storage Device
        27,                     // length
        0x74, 0x00,             // handle
        7,                      // uint8_t port_designation;
        3,                      // uint8_t device_index;
        0x01,                   // ConnectorType connector_type;
        0x02,                   // DeviceProtocol device_protocol;
        0x03,                   // DeviceType device_type;
        0x01, 0x55, 0x47, 0x38, // uint32_t device_capacity;
        0x42, 0x63,             // uint16_t device_rpm;
        1,                      // uint8_t device_model;
        2,                      // uint8_t device_serial;
        0x67,                   // uint8_t pci_class_code;
        0x26, 0x76,             // uint16_t vendor_id;
        0x34, 0x13,             // uint16_t device_id;
        0xac, 0xde,             // uint16_t sub_vendor_id;
        0x53, 0xaf,             // uint16_t sub_device_id;
        3,                      // uint8_t firmware_version;
        '3', '2', '1', 'D', 'R', 'I', 'V', 'E', 0x00,
        '1', '3', '2', '6', '5', '4', '8', '7', 0x00,
        'A', 'B', 'C', 'D', '2', '4', '6', '8', 0x00,
        0x00,

        /*
        TPM_DEVICE     @117
        */
        0xc3,                               // TPM
        7,                                  // length
        0x75, 0x00,                         // handle
        1,                                  // uint8_t tpm_configuration_index;
        1,                                  // uint8_t tpm_version;
        0x01,                               // TPMStatus tpm_status;
        'T', 'P', 'M', ' ', '2', '.', '0', 0x00,
        0x00,

        /*
        TPM_DEVICE     @118
        */
        0xc3,                               // TPM
        7,                                  // length
        0x76, 0x00,                         // handle
        2,                                  // uint8_t tpm_configuration_index;
        1,                                  // uint8_t tpm_version;
        0x00,                               // TPMStatus tpm_status;
        'T', 'P', 'M', ' ', '1', '.', '2', 0x00,
        0x00,

        /*
        TXT @ 119
         */
        0xc4,                               // TXT
        5,                                  // length
        0x77, 0x00,                         // handle
        0x01,                               // status
        0x00, 0x00,

        /*
         * Memory Device Extended @ 120
         */
        0xc5,                   // memory device extended
        15,                     // length
        0x78, 0x00,             // handle
        0x01, 0x00,             // uint16_t memory_device_handle;
        0x01,                   // MemoryDeviceExtendedType type;
        0x02,                   // MemoryDeviceExtendedMedia media;
        1,                      // uint8_t firmware_revision;
        2,                      // uint8_t firmware_api_version;
        0x12, 0x23, 0x45, 0x67, // uint32_t max_tdp;
        0x07,                   // uint8_t smbus_address;
        '1', '2', '3', 'M', 'E', 'M', 'O', 'R', 'Y', 0x00,
        'X', 'Y', 'Z', '_', '5', '6', '7', '8', 0x00,
        0x00,

        /*
        FPGA_DEVICE_OEM     @121
        */
        0xc6,                   // FPGA
        35,                     // length
        0x79, 0x00,             // handle
        1,                      // uint8_t fpga_index;
        0,                      // FpgaType fpga_type;
        1,                      // FpgaStatus fpga_status;
        58,                     // uint8_t socket_identifier;
        1,                      // uint8_t fpga_vendor;
        2,                      // uint8_t fpga_family;
        3,                      // uint8_t fpga_model;
        4,                      // uint8_t fpga_bit_stream_version;
        0x22,                   // uint8_t fpga_hps_core_count;
        0x03,                   // uint8_t fpga_hps_isa;
        0x00,                   // uint8_t fpga_hssi_configuration;
        0x07,                   // uint8_t fpga_hssi_port_count;
        0x08,                   // uint8_t fpga_hssi_port_speed;
        5,                      // uint8_t fpga_hssi_side_band_configuration;
        0x36,                   // uint8_t fpga_reconfiguration_slots;
        0x26, 0x76,             // uint16_t fpga_pcie_slot_number;
        0x66,                   // uint8_t fpga_pcie_bus_identifier;
        0x55,                   // uint8_t fpga_pcie_device_identifier;
        0x21,                   // uint8_t fpga_pcie_function_identifier;
        0x02, 0x32, 0x61, 0x12, // uint32_t thermal_design_power;
        0x02,                   // IntegratedMemoryTechnology memory_technology;
        0x01, 0x55, 0x47, 0x38, // uint32_t integrated_memory_capacity;
        0x42, 0x63,             // uint16_t integrated_memory_speed;
        'A', 'B', 0x00,         // "AB"
        'C', 'D', 0x00,         // "CD"
        'E', 'F', 'F', 0x00,    // "EFF"
        'G', 'H', 'H', 0x00,    // "GHH"
        'I', 'P', 'Q', 0x00,    // "IPQ
        0x00
};

static const uint8_t smbios3_0[] = {
    /*
     * 64 bit entry point
     */
    0x5f, 0x53, 0x4d, 0x33, 0x5f,   // anchor string "_SM3_"
    0x35,                           // checksum
    0x18,                           // length
    0x03,                           // major version
    0x00,                           // minor version
    0x00,                           // smbios docrev
    0x01,                           // entry point revision
    0x00,                           // reserved
    0x0c, 0x00, 0x00, 0x00,         // structure table max size
    0x12, 0x00, 0x00, 0x00,         // struct table address
    0x00, 0x00, 0x00, 0x00,

    /*
    SMBIOS_SYSTEM_INFO_DATA	@1
        UUID            : 70f47d7a9577e411a2d12c60c73669a
        Wake-up Type    : 0x
     */
    0x01, 0x1b, 0x01, 0x00, 0x01, 0x02, 0x03, 0x04, 0x70, 0xf4, 0x7d, 0x7a,
    0x95, 0x77, 0xe4, 0x11, 0xa2, 0xd1, 0x2c, 0x60, 0x0c, 0x73, 0x66, 0x9a,
    0x06, 0x05, 0x06,

    //Manufacturer   : ABCDEF
    0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x00,

    //Product Name    : XXXX_XXX
    0x58, 0x58, 0x58, 0x58, 0x5f, 0x58, 0x58, 0x58, 0x00,

    //Version         : To be filled by O.E.M.
    0x54, 0x6f, 0x20, 0x62, 0x65, 0x20, 0x66, 0x69, 0x6c, 0x6c, 0x65, 0x64,
    0x20, 0x62, 0x79, 0x20, 0x4f, 0x2e, 0x45, 0x2e, 0x4d, 0x2e, 0x00,

    //Serial Number   : 6666666666666
    0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
    0x36, 0x00,

    //SKU Number      : To be filled by O.E.M.
    0x54, 0x6f, 0x20, 0x62, 0x65, 0x20, 0x66, 0x69, 0x6c, 0x6c, 0x65, 0x64,
    0x20, 0x62, 0x79, 0x20, 0x4f, 0x2e, 0x45, 0x2e, 0x4d, 0x2e, 0x00,

    //Family          : Server
    0x53, 0x65, 0x72, 0x76, 0x65, 0x72,
    0x00, 0x00,

    0x00
};

static const uint8_t smbios_v3_with_zero_speed_select_configs[] = {
    // 64 bit entry point
    0x5f, 0x53, 0x4d, 0x33, 0x5f,   // anchor string "_SM3_"
    0x2f,                           // checksum
    0x18,                           // length
    0x03,                           // major version
    0x00,                           // minor version
    0x00,                           // smbios docrev
    0x01,                           // entry point revision
    0x00,                           // reserved
    0x0c, 0x00, 0x00, 0x00,         // structure table max size
    0x18, 0x00, 0x00, 0x00,         // struct table address: start at 24
    0x00, 0x00, 0x00, 0x00,

    // SMBIOS_SPEED_SELECT_INFO_DATA @201
    210, // ISS config
    16, // length: 16 for config
    1, 0, // handle
    0, //uint8_t socket_number;
    1, //uint8_t structure_version;
    0, //uint8_t number_of_configs;
    0, //uint8_t current_config;
    0, 0, 0, 0, 0, 0, 0, 0 //uint8_t reserved[8];
};


static const uint8_t smbios_v3_with_one_speed_select_config[] = {
    // 64 bit entry point
    0x5f, 0x53, 0x4d, 0x33, 0x5f,   // anchor string "_SM3_"
    0x2f,                           // checksum
    0x18,                           // length
    0x03,                           // major version
    0x00,                           // minor version
    0x00,                           // smbios docrev
    0x01,                           // entry point revision
    0x00,                           // reserved
    0x0c, 0x00, 0x00, 0x00,         // structure table max size
    0x18, 0x00, 0x00, 0x00,         // struct table address: start at 24
    0x00, 0x00, 0x00, 0x00,

    // SMBIOS_SPEED_SELECT_INFO_DATA @201
    210, // ISS config
    48, // length: 16 + 32 for config
    1, 0, // handle
    0, //uint8_t socket_number;
    1, //uint8_t structure_version;
    1, //uint8_t number_of_configs;
    0, //uint8_t current_config;
    0, 0, 0, 0, 0, 0, 0, 0, //uint8_t reserved[8];

    // configuration #0
    0, //uint8_t configuration_number;
    0, 0, 0, //uint8_t reserved[3];
    18, //uint8_t high_priority_core_count;
    16, //uint8_t high_priority_base_frequency; 1600MHz
    0, //uint8_t low_priority_core_count;
    0, //uint8_t low_priority_base_frequency;
    150, 0, //uint16_t max_tdp;
    105, //uint8_t max_junction_temperature;
    1, //uint8_t high_priority_code_apic_ids;
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //uint8_t reserved[20];
    // string: APIC IDS
    'T', 'B', 'D', 0x00,
    0x00, 0x00 // two zeros mark end of strings
};

static const uint8_t smbios_v3_with_three_speed_select_configs[] = {
    // 64 bit entry point
    0x5f, 0x53, 0x4d, 0x33, 0x5f,   // anchor string "_SM3_"
    0x2f,                           // checksum
    0x18,                           // length
    0x03,                           // major version
    0x00,                           // minor version
    0x00,                           // smbios docrev
    0x01,                           // entry point revision
    0x00,                           // reserved
    0x0c, 0x00, 0x00, 0x00,         // structure table max size
    0x18, 0x00, 0x00, 0x00,         // struct table address: start at 24
    0x00, 0x00, 0x00, 0x00,

    // SMBIOS_SPEED_SELECT_INFO_DATA @201
    210, // ISS config
    112, // length: 16 + 3x32 for config
    1, 0, // handle
    0, //uint8_t socket_number;
    1, //uint8_t structure_version;
    3, //uint8_t number_of_configs;
    0, //uint8_t current_config;
    0, 0, 0, 0, 0, 0, 0, 0, //uint8_t reserved[8];

    // configuration #0
    0, //uint8_t configuration_number;
    0, 0, 0, //uint8_t reserved[3];
    18, //uint8_t high_priority_core_count;
    16, //uint8_t high_priority_base_frequency; 1600MHz
    0, //uint8_t low_priority_core_count;
    0, //uint8_t low_priority_base_frequency;
    150, 0, //uint16_t max_tdp;
    105, //uint8_t max_junction_temperature;
    1, //uint8_t high_priority_code_apic_ids;
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //uint8_t reserved[20];

    // configuration #1
    1, //uint8_t configuration_number;
    0, 0, 0, //uint8_t reserved[3];
    14, //uint8_t high_priority_core_count;
    28, //uint8_t high_priority_base_frequency; 2800MHz
    0, //uint8_t low_priority_core_count;
    0, //uint8_t low_priority_base_frequency;
    150, 0, //uint16_t max_tdp;
    105, //uint8_t max_junction_temperature;
    2, //uint8_t high_priority_code_apic_ids;
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //uint8_t reserved[20];

    // configuration #2
    2, //uint8_t configuration_number;
    0, 0, 0, //uint8_t reserved[3];
    4, //uint8_t high_priority_core_count;
    26, //uint8_t high_priority_base_frequency; 1600MHz
    14, //uint8_t low_priority_core_count;
    18, //uint8_t low_priority_base_frequency; 1800MHz
    150, 0, //uint16_t max_tdp;
    105, //uint8_t max_junction_temperature;
    3, //uint8_t high_priority_code_apic_ids;
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //uint8_t reserved[20];

    // strings: APIC IDS
    'T', 'B', 'D', '0', 0x00,
    'T', 'B', 'D', '1', 0x00,
    'T', 'B', 'D', '2', 0x00,
    0x00, 0x00 // two zeros mark end of strings
};

}

#endif //PSME_FW_BUILD_TEST_FIXTURE_HPP
