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
 * @file sysfs/sysfs_decoder.cpp
 * @brief SysfsDecoder implementation
 * */

#include "sysfs/sysfs_decoder.hpp"

#include "safe-string/safe_lib.hpp"
#include "logger/logger_factory.hpp"

#include <tuple>
#include <array>
#include <cassert>
#include <regex>
#include <sysfs/raw_sysfs_device.hpp>

using namespace agent::pnc::sysfs;

namespace {

    // Things required for device identification

    constexpr const uint16_t VENDOR_PMC_SIERRA = 0x11f8;
    constexpr const uint16_t DEVICE_PMC_SIERRA_8546 = 0x8546;
    constexpr const uint16_t DEVICE_PMC_SIERRA_8536 = 0x8536;

    constexpr const uint8_t CLASS_MEMORY = 0x05;
    constexpr const uint8_t CLASS_BRIDGE = 0x06;

    constexpr const uint8_t SUBCLASS_MEMORY = 0x80;
    constexpr const uint8_t SUBCLASS_PCI_BRIDGE = 0x04;

    constexpr const uint8_t HEADER_TYPE_1 = 0x01;
    constexpr const uint8_t HEADER_TYPE_0 = 0x00;

    constexpr const uint8_t PRIMARY_FUNCTION_NUM = 0x00;

    constexpr const char RESOURCE_PATH[] = "/resource0";

    using DeviceDescriptor = std::tuple<uint16_t, uint16_t>;

    const std::vector<DeviceDescriptor> KNOWN_SWITCHES =
        { {std::make_tuple(VENDOR_PMC_SIERRA, DEVICE_PMC_SIERRA_8546)},
          {std::make_tuple(VENDOR_PMC_SIERRA, DEVICE_PMC_SIERRA_8536)} };

    constexpr const uint8_t PCI_EXPRESS_CAPABILITY_ID = 0x10;
    constexpr const uint16_t PCI_SERIAL_NUMBER_EXTENDED_CAPABILITY_ID = 0x0003;
    constexpr const uint16_t PCI_EXTENDED_CAPABILITY_OFFSET = 0x0100;

    constexpr const uint8_t PCIE_CAPABILITY_LINK_CAPABILITY_OFFSET = 0x0C;
    constexpr const uint8_t PCIE_CAPABILITY_LINK_STATUS_OFFSET = 0x12;

    // Things required for proper values processing

    constexpr const uint8_t PCI_HEADER_MASK = 0x7F;
    constexpr const uint8_t PCI_DEVICE_TYPE_MASK = 0x80;
    constexpr const uint16_t PCI_STATUS_CAPABILITY_BIT_MASK = 0x0010;
    constexpr const uint8_t PCI_CAPABILITY_POINTER_MASK = 0xFC;
    constexpr const uint8_t PCI_EXTENDED_CAPABILITY_POINTER_SHIFT = 0x04;

    // Utility functions

    bool is_pattern_matched(const std::string& pattern, const std::string& str) {
        std::smatch matches{};
        std::regex_match(str, matches, std::regex(pattern, std::regex_constants::icase));
        return matches.size() == 1;
    }

    inline std::uint8_t get_header_type(const PciConfigurationSpace& conf) {
        // only 7 LSBs are used to represent header type
        return (conf.common.header_type & PCI_HEADER_MASK);
    }

    inline bool is_multi_function_device(const PciConfigurationSpace& conf) {
        // MSB bit is 1 if the device is a multifunction device
        return ((conf.common.header_type & PCI_DEVICE_TYPE_MASK) != 0);
    }

    inline bool has_valid_capability_pointer(const PciConfigurationSpace& conf) {
        // 4th bit of the status register is 1 if capability pointer is a valid pointere
        return ((conf.common.status & PCI_STATUS_CAPABILITY_BIT_MASK) != 0);
    }

    inline uint8_t get_capability_pointer(const PciConfigurationSpace& conf) {
        // two LSBs are reserved and should not be used
        switch (get_header_type(conf)) {
            case HEADER_TYPE_0: return conf.extended.type0.capability_pointer & PCI_CAPABILITY_POINTER_MASK;
            case HEADER_TYPE_1: return conf.extended.type1.capability_pointer & PCI_CAPABILITY_POINTER_MASK;
            default: return 0x00;
        }
    }

    inline uint8_t extract_capability_pointer(const PciCapabilityHeader& header) {
        return header.next_capability_pointer;
    }

    inline uint16_t extract_capability_pointer(const PciExtendedCapabilityHeader& header) {
        // two LSBs are reserved and should not be used
        return static_cast<uint16_t>(header.version_next_pointer >> PCI_EXTENDED_CAPABILITY_POINTER_SHIFT);
    }

    template <typename CAP_T, typename ID_T, typename PTR_T>
    PTR_T find_capability(const ID_T capability_id, const uint8_t* config_ptr, PTR_T offset) {
        // get max valid pointer value
        assert(sizeof(uint32_t) >= sizeof(CAP_T));
        const uint32_t max_offset = RawSysfsDevice::PCI_CONFIGURATION_SPACE_SIZE - sizeof(CAP_T);
        // finish only if next pointer is 0 or in case of invalid address
        if (config_ptr != nullptr) {
            while (offset != PTR_T{0} && offset < max_offset) {
                // read capability header
                CAP_T header{};
                if (EOK != memcpy_s(&header, sizeof(CAP_T), config_ptr + offset, sizeof(CAP_T))) {
                    log_error("sysfs-decoder", "Failed to access pci capability header for a device");
                    break;
                }
                // check for capability_id - return address or go to the next capability
                if (capability_id == header.capability_id) {
                    return offset;
                }
                offset = extract_capability_pointer(header);
            }
        }
        return PTR_T{0};
    }

}

SysfsDecoder::~SysfsDecoder() {}

void SysfsDecoder::decode_device_data(const RawSysfsDevice& function, SysfsDevice& device) {
    // continue only if device has a valid capability pointer
    if (!has_valid_capability_pointer(function.configuration.fields)) {
        return;
    }
    // find PCI Express capability
    uint8_t offset = find_capability<PciCapabilityHeader, uint8_t, uint8_t>(PCI_EXPRESS_CAPABILITY_ID,
        function.configuration.raw, get_capability_pointer(function.configuration.fields));
    if (offset != 0x00) {
        device.has_pcie_capabilities = true;
        device.pcie_link_capability_register =
            *(reinterpret_cast<const uint32_t*>(function.configuration.raw + offset + PCIE_CAPABILITY_LINK_CAPABILITY_OFFSET));
        device.pcie_link_status_register =
            *(reinterpret_cast<const uint16_t*>(function.configuration.raw + offset + PCIE_CAPABILITY_LINK_STATUS_OFFSET));

        // find serial number extended capability
        uint16_t offset_ex = find_capability<PciExtendedCapabilityHeader>(
            PCI_SERIAL_NUMBER_EXTENDED_CAPABILITY_ID, function.configuration.raw,
            PCI_EXTENDED_CAPABILITY_OFFSET);
        uint16_t max_offset_ex = RawSysfsDevice::PCI_CONFIGURATION_SPACE_SIZE
                                 - sizeof(PciSerialNumberExtendedCapability);
        // if serial number capability was found - read it
        if (offset_ex != 0x0000 && offset_ex < max_offset_ex) {
            PciSerialNumberExtendedCapability sn_cap{};
            if (EOK == memcpy_s(&sn_cap, sizeof(PciSerialNumberExtendedCapability),
                    function.configuration.raw + offset_ex, sizeof(PciSerialNumberExtendedCapability))) {
                device.has_serial_number = true;
                device.serial_number = (uint64_t{sn_cap.serial_number_upper} << 32)
                                        + uint64_t{sn_cap.serial_number_lower};
            }
            else {
                log_error("sysfs-decoder", "Failed to access pci serial number capability for a device");
            }
        }
    }

}

bool SysfsDecoder::is_known_switch_memory_controller(const RawSysfsDevice& dev_under_test) {

    // build a descriptor for the device
    ::DeviceDescriptor desc = std::make_tuple(
        dev_under_test.configuration.fields.common.vendor_id,
        dev_under_test.configuration.fields.common.device_id);

    // check if this is one of the known devices - test device_id, vendor_id and class_code
    for (const auto& known_switch : ::KNOWN_SWITCHES) {
        if (known_switch == desc
            && ::CLASS_MEMORY == dev_under_test.configuration.fields.common.class_code
            && ::SUBCLASS_MEMORY == dev_under_test.configuration.fields.common.subclass) {
            return true;
        }
    }

    return false;
}

bool SysfsDecoder::is_bridge_for_memory_controller(const RawSysfsDevice& memory,
        const RawSysfsDevice& dev_under_test) {

      // Device should be a PCI bus bridge (check class and subclass)
      // and has to have the same domain, bus, and device numbers as the
      // memory controller. Device_id and vendor_id are assumed to be the same,
      // they are not checked. Header should be a Type1Header (value = 1), so
      // we can read secondary_bus_number from the type 1 extended header field

      const size_t NAME_LENGTH = strlen("0000:00:00.0");
      std::string mem_str = memory.path.substr(0, memory.path.size() - NAME_LENGTH);
      std::string brg_str = dev_under_test.path.substr(0, dev_under_test.path.size() - NAME_LENGTH);

      if (::CLASS_BRIDGE == dev_under_test.configuration.fields.common.class_code
          && ::SUBCLASS_PCI_BRIDGE == dev_under_test.configuration.fields.common.subclass
          && memory.id.is_same(dev_under_test.id, true, true, true, false)
          && ::HEADER_TYPE_1 == get_header_type(dev_under_test.configuration.fields)
          && mem_str == brg_str) {
          return true;
      }
      return false;
}

bool SysfsDecoder::is_bridge_for_switch(const SysfsSwitch& pci_switch,
        const RawSysfsDevice& dev_under_test) {

    // For device to be a bridge under a switch -> it has to have the same domain_num
    // as the switch, has to have proper class, subclass and header type, also,
    // the bus_num has to be equal to the switch's secondary bus number
    SysfsId tmp(pci_switch.bridge_id.domain_num,
                pci_switch.sec_bus_num,
                0x00, 0x00);

    if (tmp.is_same(dev_under_test.id, true, true, false, false)
        && HEADER_TYPE_1 == get_header_type(dev_under_test.configuration.fields)
        && ::CLASS_BRIDGE == dev_under_test.configuration.fields.common.class_code
        && ::SUBCLASS_PCI_BRIDGE == dev_under_test.configuration.fields.common.subclass
        && ::is_pattern_matched("^" + pci_switch.bridge_path + "/.*", dev_under_test.path)) {

        return true;
    }
    return false;
}

inline bool SysfsDecoder::is_device_for_bridge(const SysfsBridge& bridge,
        const RawSysfsDevice& device_under_test) {
    SysfsId tmp(bridge.id.domain_num,
                bridge.sec_bus_num,
                0x00, 0x00);
    return (::is_pattern_matched(bridge.path + ".*", device_under_test.path)
            && tmp.is_same(device_under_test.id, true, true, false, false));
}

inline bool SysfsDecoder::is_function_for_device(const SysfsDevice& device,
        const RawSysfsDevice& device_under_test) {
    return (::is_pattern_matched(device.bridge_path + ".*", device_under_test.path)
            && device.id.is_same(device_under_test.id, true, true, true, false));
}

SysfsSwitch SysfsDecoder::make_switch(const RawSysfsDevice& memory,
        const RawSysfsDevice& bridge) {

    SysfsSwitch pci_switch{};
    pci_switch.device_id = memory.configuration.fields.common.device_id;
    pci_switch.vendor_id = memory.configuration.fields.common.vendor_id;
    pci_switch.memory_resource = memory.path + ::RESOURCE_PATH;
    pci_switch.sec_bus_num = bridge.configuration.fields.extended.type1.secondary_bus_number;
    pci_switch.bridge_path = bridge.path;
    pci_switch.memory_id = memory.id;
    pci_switch.bridge_id = bridge.id;
    pci_switch.pci_revision_id = bridge.configuration.fields.common.revision_id;

    // No serial number is provided by default
    pci_switch.has_serial_number = false;

    // continue only if device has a valid capability pointer
    if (!has_valid_capability_pointer(bridge.configuration.fields)) {
        log_warning("sysfs-decoder", "No serial number provided in PCIe Switch configuration space.");
        return pci_switch;
    }
    // find PCI Express capability
    uint8_t offset = find_capability<PciCapabilityHeader, uint8_t, uint8_t>(PCI_EXPRESS_CAPABILITY_ID,
                                                                            bridge.configuration.raw, get_capability_pointer(bridge.configuration.fields));
    if (offset != 0x00) {
        // find serial number extended capability
        uint16_t offset_ex = find_capability<PciExtendedCapabilityHeader>(
                PCI_SERIAL_NUMBER_EXTENDED_CAPABILITY_ID, bridge.configuration.raw,
                PCI_EXTENDED_CAPABILITY_OFFSET);
        uint16_t max_offset_ex = RawSysfsDevice::PCI_CONFIGURATION_SPACE_SIZE
                                 - sizeof(PciSerialNumberExtendedCapability);
        // if serial number capability was found - read it
        if (offset_ex != 0x0000 && offset_ex < max_offset_ex) {
            PciSerialNumberExtendedCapability sn_cap{};
            if (EOK == memcpy_s(&sn_cap, sizeof(PciSerialNumberExtendedCapability),
                                bridge.configuration.raw + offset_ex, sizeof(PciSerialNumberExtendedCapability))) {
                pci_switch.has_serial_number = true;
                pci_switch.serial_number = (uint64_t{sn_cap.serial_number_upper} << 32)
                                       + uint64_t{sn_cap.serial_number_lower};
            }
            else {
                log_error("sysfs-decoder", "Failed to access pci serial number capability for a PCIe Switch");
            }
        }
    }

    return pci_switch;
}

SysfsBridge SysfsDecoder::make_bridge(const RawSysfsDevice& bridge) {
    SysfsBridge new_bridge{};
    new_bridge.path = bridge.path;
    new_bridge.id = bridge.id;
    new_bridge.sec_bus_num = bridge.configuration.fields.extended.type1.secondary_bus_number;
    return new_bridge;
}

SysfsDevice SysfsDecoder::make_device(const RawSysfsDevice& device) {
    SysfsDevice new_device{};
    new_device.id = device.id;
    new_device.vendor_id = device.configuration.fields.common.vendor_id;
    new_device.device_id = device.configuration.fields.common.device_id;
    new_device.is_multi_function_device = is_multi_function_device(device.configuration.fields);
    return new_device;
}

SysfsFunction SysfsDecoder::make_function(const RawSysfsDevice& function) {
    SysfsFunction new_function{};
    new_function.id = function.id;
    new_function.path = function.path;
    new_function.is_virtual = function.is_virtual;
    new_function.device_class = function.configuration.fields.common.class_code;
    new_function.device_subclass = function.configuration.fields.common.subclass;
    new_function.device_prog_if = function.configuration.fields.common.prog_if;
    new_function.pci_device_id = function.configuration.fields.common.device_id;
    new_function.pci_vendor_id = function.configuration.fields.common.vendor_id;
    new_function.pci_revision_id = function.configuration.fields.common.revision_id;
    if (get_header_type(function.configuration.fields) == ::HEADER_TYPE_0) {
        new_function.pci_subsystem_id = function.configuration.fields.extended.type0.subsystem_id;
        new_function.pci_subsystem_vendor_id = function.configuration.fields.extended.type0.subsystem_vendor_id;
    }
    return new_function;
}

std::vector<SysfsSwitch> SysfsDecoder::decode_switches(
        const std::vector<RawSysfsDevice>& raw_devices) {
        std::vector<SysfsSwitch> switches{};
    // check each device
    for (const auto& memory : raw_devices) {
        // if this is a known switch memory controller...
        if (is_known_switch_memory_controller(memory)) {
            // ... go (again) through all devices ...
            for (const auto& bridge : raw_devices) {
                // ... and check if we can find a bridge for this memory controller
                if (is_bridge_for_memory_controller(memory, bridge)) {
                    // if bridge is found - add it to the internal list
                    switches.emplace_back(make_switch(memory, bridge));
                }
            }
        }
    }
    return switches;
}

std::vector<SysfsBridge> SysfsDecoder::decode_bridges_for_switch(
        const SysfsSwitch& pci_switch, const std::vector<RawSysfsDevice>& raw_devices) {

    std::vector<SysfsBridge> bridges{};
    // find all pci_switch secondary buses
    for (const auto& bridge : raw_devices) {
        // if the device is a bridge on the switch ...
        if (is_bridge_for_switch(pci_switch, bridge) ) {
            // ... add it as a bus on the switch
            bridges.emplace_back(make_bridge(bridge));
        }
    }
    return bridges;
}

std::vector<SysfsDevice> SysfsDecoder::decode_devices_for_bridge(
        const SysfsBridge& bridge, const std::vector<RawSysfsDevice>& raw_devices) {

    std::vector<SysfsDevice> devices{};
    // there may be several functions per device - set keeps list of devices unique
    std::set<uint8_t> known_devices{};
    for (const auto& device : raw_devices) {
        // if this is a new device on the bus
        if (is_device_for_bridge(bridge, device)
            && known_devices.find(device.id.device_num) == known_devices.end()) {

            // add a new device, and add it to the known devices list
            SysfsDevice new_device = make_device(device);
            new_device.bridge_path = bridge.path;
            // Some data about the device have to be read from its primary function
            if (PRIMARY_FUNCTION_NUM == device.id.function_num) {
                decode_device_data(device, new_device);
            }
            devices.emplace_back(new_device);
            known_devices.insert(device.id.device_num);
        }
    }

    return devices;
}

std::vector<SysfsFunction> SysfsDecoder::decode_functions_for_device(
        const SysfsDevice& device, const std::vector<RawSysfsDevice>& raw_devices) {

    std::vector<SysfsFunction> functions;
    for (const auto& function : raw_devices) {
        if (is_function_for_device(device, function)) {
            auto new_function = make_function(function);
            new_function.drives = decode_drives_for_function(function);
            functions.emplace_back(new_function);

            // exit if there is only one function
            if (!is_multi_function_device(function.configuration.fields)) {
                break;
            }
        }
    }

    return functions;
}

std::vector<SysfsDrive> SysfsDecoder::decode_drives_for_function(
        const RawSysfsDevice& function) {

    std::vector<SysfsDrive> drives{};

    if (function.drive_names.size() == function.drive_sizes.size()) {
        for (size_t i = 0; i < function.drive_names.size(); ++i) {
            SysfsDrive new_drive{};
            new_drive.name = function.drive_names[i];
            new_drive.size_bytes = function.drive_sizes[i];
            drives.emplace_back(new_drive);
        }
    }

    return drives;
}

SysfsBridge SysfsDecoder::get_bridge_by_switch_path(const std::string bridge_path,
        uint8_t bridge_id) const {
    bool found = false;
    SysfsSwitch sw{};
    for (const auto& dev : m_raw_data) {
        if (bridge_path == dev.path) {
            sw = make_switch(RawSysfsDevice{}, dev);
            found = true;
            break;
        }
    }
    if (!found) {
        throw std::invalid_argument(std::string{"Bridge not found, path = "} + bridge_path);
    }
    for (const auto& bridge : get_bridges(sw)) {
        if (bridge.id.device_num == bridge_id) {
            return bridge;
        }
    }
    throw std::invalid_argument(std::string{"Bridge not found, id = "}
        + std::to_string(unsigned{bridge_id}));
}
