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
 * @file storage_device.hpp
 *
 * @brief SMBIOS structures describing storage device
 * */

#pragma once

#include <cstdint>

#pragma pack(push, 1)

namespace smbios {
namespace parser {



/*!
 * @brief Connector Type
 */
enum class ConnectorType : uint8_t {
    UNKNOWN = 0x00,
    SATA = 0x01,
    SAS = 0x02,
    PCIE = 0x03,
    M_2 = 0x04,
    USB = 0x05,
    U_2 = 0x06
};

/*!
 * @brief Device Protocol
 */
enum class DeviceProtocol : uint8_t {
    UNKNOWN = 0x00,
    IDE = 0x01,
    AHCI = 0x02,
    NVME = 0x03,
    USB = 0x04
};

/*!
 * @brief Device Type
 */
enum class StorageDeviceType : uint8_t {
    HDD = 0x01,
    SSD = 0x02,
    DVD = 0x03,
    BLUE_RAY = 0x04,
    USB = 0x05
};

/*!
 * @brief BIOS information data
 */
struct STORAGE_DEVICE_INFO_DATA {
    uint8_t port_designation;
    uint8_t device_index;
    ConnectorType connector_type;
    DeviceProtocol device_protocol;
    StorageDeviceType device_type;
    uint32_t device_capacity;
    uint16_t device_rpm;
    uint8_t device_model;
    uint8_t device_serial;
    uint8_t pci_class_code;
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t sub_vendor_id;
    uint16_t sub_device_id;
    uint8_t firmware_version;
};

/*!
 * @brief SMBIOS Storage Device information data (Type 194)
 */
struct SMBIOS_STORAGE_DEVICE_INFO_DATA {
    static constexpr uint8_t ID = 194;
    SMBIOS_OOB_HEADER header;
    STORAGE_DEVICE_INFO_DATA data;
};

}
}
#pragma pack(pop)

