/*!
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
 *
 * @file smbios_structs.hpp
 *
 * @brief smbios struct definitions
 * */

#pragma once



#include "smbios_header.hpp"
#include "bios.hpp"
#include "memory.hpp"
#include "module.hpp"
#include "chassis.hpp"
#include "processor.hpp"
#include "cpuid.hpp"
#include "system.hpp"
#include "fpga.hpp"
#include "storage_device.hpp"
#include "tpm.hpp"
#include "pcie_port.hpp"
#include "speed_select_information.hpp"
#include <cstdint>



#pragma pack(push, 1)

namespace smbios {
namespace parser {

constexpr uint8_t MAC_AREA_LEN = 32;

struct NIC_PORT_INFO_DATA {
    uint16_t PortIndex;
    uint8_t Mac[MAC_AREA_LEN];
};

struct NIC_INFO_DATA {
    uint8_t PciClassCode;
    uint16_t SlotNo;
    uint16_t VendorID;
    uint16_t DeviceID;
    uint16_t SubVendorID;
    uint16_t SubDeviceID;
    uint32_t LinkSpeed;
    uint32_t LinkWidth;
    NIC_PORT_INFO_DATA PortData;
};

struct NIC_INFO_DATA_V2 {
    uint8_t PciClassCode;
    uint16_t SlotNo;
    uint16_t VendorID;
    uint16_t DeviceID;
    uint16_t SubVendorID;
    uint16_t SubDeviceID;
    uint32_t LinkSpeed;
    uint32_t LinkWidth;
    NIC_PORT_INFO_DATA PortData;
    uint8_t FirmwareVersion;
};

struct SMBIOS_NIC_INFO_DATA {
    static constexpr uint8_t ID = 190;
    SMBIOS_OOB_HEADER header;
    NIC_INFO_DATA data;
};

struct SMBIOS_NIC_INFO_DATA_V2 {
    static constexpr uint8_t ID = 190;
    SMBIOS_OOB_HEADER header;
    NIC_INFO_DATA_V2 data;
};

struct HDD_INFO_DATA {
    static constexpr uint8_t HDD_NAME_STRING_SIZE = 39;
    uint8_t HDDIndex;
    uint8_t HDDName[HDD_NAME_STRING_SIZE];
    uint8_t HDDType;
    uint32_t HDDSizeInGB;
};

struct STORAGE_INFO_DATA {
    uint8_t PciClassCode;
    uint16_t VendorID;
    uint16_t DeviceID;
    uint16_t SubVendorID;
    uint16_t SubDeviceID;
    HDD_INFO_DATA RsaHddInfoData;
};

struct SMBIOS_STORAGE_INFO_DATA {
    static constexpr uint8_t ID = 191;
    SMBIOS_OOB_HEADER header;
    STORAGE_INFO_DATA data;
};

struct HDD_INFO_DATA_V2 {
    static constexpr uint8_t HDD_NAME_STRING_SIZE = 40;
    static constexpr uint8_t HDD_SERIAL_NUMBER_SIZE = 20;
    uint8_t HDDIndex;
    uint8_t HDDName[HDD_NAME_STRING_SIZE];
    uint8_t HDDSerialNumber[HDD_SERIAL_NUMBER_SIZE];
    uint8_t HDDType;
    uint32_t HDDSizeInGB;
};

struct STORAGE_INFO_DATA_V2 {
    uint8_t PciClassCode;
    uint8_t PciSubClassCode;
    uint16_t VendorID;
    uint16_t DeviceID;
    uint16_t SubVendorID;
    uint16_t SubDeviceID;
    HDD_INFO_DATA_V2 RsaHddInfoData;
};

struct SMBIOS_STORAGE_INFO_DATA_V2 {
    static constexpr uint8_t ID = 191;
    SMBIOS_OOB_HEADER header;
    STORAGE_INFO_DATA_V2 data;
};

struct PCIE_INFO_DATA {
    uint8_t ClassCode;
    uint16_t SlotNo;
    uint16_t VendorID;
    uint16_t DeviceID;
    uint16_t SubVendorID;
    uint16_t SubDeviceID;
    uint32_t LinkSpeed;
    uint32_t LinkWidth;
};

struct SMBIOS_PCIE_INFO_DATA {
    static constexpr uint8_t ID = 192;
    SMBIOS_OOB_HEADER header;
    PCIE_INFO_DATA data;
};

enum class TxtStatus : uint8_t {
    DISABLED = 0x00,
    ENABLED = 0x01
};

struct TXT_INFO_DATA {
    TxtStatus status;
};

struct SMBIOS_TXT_INFO_DATA {
    static constexpr uint8_t ID = 196;
    SMBIOS_OOB_HEADER header;
    TXT_INFO_DATA data;
};

#pragma pack(pop)

}
}
