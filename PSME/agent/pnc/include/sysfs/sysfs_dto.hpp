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
 * @file sysfs/sysfs_dto.hpp
 * @brief DTO classes representing decoded sysfs data obtained from SysfsDecoders
 * */

#pragma once

#include <string>
#include <vector>

#include "sysfs/sysfs_id.hpp"

namespace agent {
namespace pnc {
namespace sysfs {

/*!
 * Struct representing information about PCIe drives found in sysfs
 */
struct SysfsDrive final {
    /*! Name of the drive, as visible under /sys/block */
    std::string name{};
    /*! Size of the drive in bytes */
    double size_bytes{};
};

/*!
 * Struct representing information about PCIe functions found in sysfs
 */
struct SysfsFunction final {
    /*! Function id, read from the path */
    SysfsId id{};
    /*! Function path, read from the path */
    std::string path{};

    /*! True for virtual functions */
    bool is_virtual{};
    /*! Device class read from configuration space */
    uint8_t device_class{};
    /*! Device subclass read from configuration space */
    uint8_t device_subclass{};
    /*! Device programming interface, read from configuration space*/
    uint8_t device_prog_if{};
    /*! Pci Device Id, read from configuration space */
    uint16_t pci_device_id{};
    /*! Pci Vendor Id, read from configuration space */
    uint16_t pci_vendor_id{};
    /*! Revision Id, read from configuration space */
    uint8_t pci_revision_id{};
    /*! Pci Subsystem Device Id, read configuration space (extended header type 0) */
    uint16_t  pci_subsystem_id{};
    /*! Pci Subsystem Vendor Id, read configuration space (extended header type 0) */
    uint16_t  pci_subsystem_vendor_id{};

    /*! List of Drives on the function */
    std::vector<SysfsDrive> drives{};
};

/*!
 * Struct representing information about PCIe devices found in sysfs
 */
struct SysfsDevice final {
    /*! Device id, read from the path, used for identification, function_num is always invalid */
    SysfsId id{};
    /*! Bridge path, used for identification */
    std::string bridge_path{};

    /*! True for multi-function devices */
    bool is_multi_function_device{false};
    /*! Vendor Id read from the device configuration space */
    uint16_t vendor_id{};
    /*! Device Id read from the device configuration space */
    uint16_t device_id{};
    /*! True if device PCIe capabilities were read from the hardware */
    bool has_pcie_capabilities{false};
    /*! Stored value of the link capability register from PCIe capabilities */
    uint32_t pcie_link_capability_register{};
    /*! Stored value of the link status register from PCIe capabilities */
    uint16_t pcie_link_status_register{};
    /*! True if serial number was read from the hardware and serial_number stores a valid value */
    bool has_serial_number{false};
    /*! Serial number of the device, read from serial pci extended capabilities */
    uint64_t serial_number{};
};

/*!
 * Struct representing information about a bridge of the PCI switch
 */
struct SysfsBridge final {
    /*! Path to the bridge device, used for identification */
    std::string path{};
    /*! Id of the device, used for identification */
    SysfsId id{};

    /*! Number of the secondinary bus of the switch */
    uint8_t sec_bus_num{};
};

/*!
 * Struct representing information about PCIe switch found in sysfs
 */
struct SysfsSwitch final {
    /*! Device Ids for the memory controller of the switch, used for identification */
    SysfsId memory_id{};
    /*! Device Ids for the logical bridge of the switch, used for identification */
    SysfsId bridge_id{};
    /*! Path to the logical bridge of the switch, used for identification */
    std::string bridge_path{};

    /*! Vendor Id read from the device configuration space */
    uint16_t vendor_id{};
    /*! Device Id read from the device configuration space */
    uint16_t device_id{};
    /*! Path to the sysfs MemoryController device for the switch */
    std::string memory_resource{};
    /*! Secondinary bus number read from the device configuration space */
    uint8_t sec_bus_num{};
    /*! True if serial number was read from the hardware and serial_number stores a valid value */
    bool has_serial_number{false};
    /*! Serial number of the switch, read from serial pci extended capabilities */
    uint64_t serial_number{};
    /*! Revision Id, read from configuration space */
    uint8_t pci_revision_id{};
};

}
}
}
