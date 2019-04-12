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
 * @file sysfs/raw_sysfs_device.hpp
 * @brief RawSysfsDevice and other structures used to describe raw sysfs data
 * */

#pragma once

#include "sysfs/sysfs_id.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace agent {
namespace pnc {
namespace sysfs {

#pragma pack(push, 1)

/*!
 * Struct representing PCI device Configuration space, consists of different parts
 * that can be accessed depending on the header_type field
 */
struct PciConfigurationSpace final {

    /*! Representation of the PCI Configuration Space common fields */
    struct PciCommonHeader final {
        uint16_t  vendor_id{};
        uint16_t  device_id{};
        uint16_t  command{};
        uint16_t  status{};
        uint8_t   revision_id{};
        uint8_t   prog_if{};
        uint8_t   subclass{};
        uint8_t   class_code{};
        uint8_t   cache_line_size{};
        uint8_t   latency_timer{};
        uint8_t   header_type{};
        uint8_t   bist{};
    } common{};

    /*!
     * Union keeping all different header types together, only one is valid at the time
     * depending on the header_type value from common header part. Note that header_type
     * field stores not only the header type and has to be properly decoded (see
     * RawSysfsDevice::get_header_type and RawSysfsDevice::is_multi_function_device).
     */
    union PciExtendedHeader {
        /*! Remaining part of the header for most PCI devices (header_type = 0) */
        struct PciType0Header final {
            uint32_t  base_address[6]{};
            uint32_t  cardbus_cis_pointer{};
            uint16_t  subsystem_vendor_id{};
            uint16_t  subsystem_id{};
            uint32_t  expansion_rom_base_address{};
            uint8_t   capability_pointer{};
            uint8_t   reserved[7]{};
            uint8_t   interrupt_line{};
            uint8_t   interrupt_pin{};
            uint8_t   min_grant{};
            uint8_t   max_latency{};
        } type0;

        /*! Remaining part of the header for PCI-to-PCI bridges (header_type = 1) */
        struct PciType1Header final {
            uint32_t  base_address[2]{};
            uint8_t   primary_bus_number{};
            uint8_t   secondary_bus_number{};
            uint8_t   subordinate_bus_number{};
            uint8_t   secondary_latency_timer{};
            uint8_t   io_base{};
            uint8_t   io_limit{};
            uint16_t  secondary_status{};
            uint16_t  memory_base{};
            uint16_t  memory_limit{};
            uint16_t  prefetchable_memory_base{};
            uint16_t  prefetchable_memory_limit{};
            uint32_t  prefetchable_base_upper_32_bits{};
            uint32_t  prefetchable_limit_upper_32_bits{};
            uint16_t  io_base_upper_16_bits{};
            uint16_t  io_limit_upper_16_bits{};
            uint8_t   capability_pointer{};
            uint8_t   reserved[3]{};
            uint32_t  expansion_rom_base_address{};
            uint8_t   interrupt_line{};
            uint8_t   interrupt_pin{};
            uint16_t  bridge_control{};
        } type1;
    } extended{};
};

/*! Representation of the PCI Configuration Space Capability header */
struct PciCapabilityHeader final {
    std::uint8_t capability_id{};
    std::uint8_t next_capability_pointer{};
};

/*! Representation of the PCI Express Capability */
struct PciExpressCapability final {
    std::uint8_t  capability_id{};
    std::uint8_t  next_capability_pointer{};
    std::uint16_t pci_express_capabilities{};
    std::uint32_t device_capabilities{};
    std::uint16_t device_control{};
    std::uint16_t device_status{};
    std::uint32_t link_capabilities{};
    std::uint16_t link_control{};
    std::uint16_t link_status{};
    std::uint32_t slot_capabilities{};
    std::uint16_t slot_control{};
    std::uint16_t slot_status{};
    std::uint16_t root_control{};
    std::uint16_t root_capabilities{};
    std::uint32_t root_status{};
    std::uint32_t device_capabilities_2{};
    std::uint16_t device_control_2{};
    std::uint16_t device_status_2{};
    std::uint32_t link_capabilities_2{};
    std::uint16_t link_control_2{};
    std::uint16_t link_status_2{};
    std::uint32_t slot_capabilities_2{};
    std::uint16_t slot_control_2{};
    std::uint16_t slot_status_2{};
};

/*! Representation of the PCI Extended Capability header */
struct PciExtendedCapabilityHeader final {
    std::uint16_t capability_id{};
    std::uint16_t version_next_pointer{};
};

/*! Representation of the Device Serial Number PCI Extended Capability */
struct PciSerialNumberExtendedCapability final {
    std::uint16_t capability_id{};
    std::uint16_t version_next_pointer{};
    std::uint32_t serial_number_lower{};
    std::uint32_t serial_number_upper{};
};

#pragma pack(pop)

/*!
 * Class representing PCI device data read from sysfs
 */
struct RawSysfsDevice final {

    /*! Size of the PCI Configuration space */
    static constexpr uint32_t PCI_CONFIGURATION_SPACE_SIZE = 16*256;
    /*! Numbers used to identify device in the sysfs */
    SysfsId id{};
    /*! Full path to the device */
    std::string path{};
    /*! Name of the device */
    std::string name{};
    /*! True for virtual devices */
    bool is_virtual{false};
    /*! Names of drives under the devices */
    std::vector<std::string> drive_names{};
    /*! Sizes of drives under the device */
    std::vector<double> drive_sizes{};
    /*! PCI Configuration space of the device */
    union {
        PciConfigurationSpace fields{};
        std::uint8_t raw[PCI_CONFIGURATION_SPACE_SIZE];
    } configuration{};

};

/*! Simple represenation of the PCIe Address*/
struct PcieAddress final {
    std::uint8_t bus{};
    std::uint8_t device{};
    std::uint8_t function{};
};

}
}
}
