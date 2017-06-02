/*!
 * @copyright
 * Copyright (c) 2016-2017 Intel Corporation
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
 * @file sysfs/sysfs_reader.cpp
 * @brief SysfsReader implementation
 * */

#include "sysfs/sysfs_reader.hpp"

#include "safe-string/safe_lib.hpp"
#include "logger/logger_factory.hpp"

#include <string>
#include <sstream>
#include <regex>
#include <algorithm>

using namespace agent::pnc::sysfs;

namespace {
    const char DRIVER_NVME[] = "nvme";
    const char CLASS_BLOCK[] = "block";
    const char ATTRIBUTE_PHYS_FN[] = "physfn";
    const char ATTRIBUTE_CONFIG[] = "config";
    const char ATTRIBUTE_SIZE[] = "size";
    const char ATTRIBUTE_UNBIND[] = "unbind";
    const char BUS_NAME[] = "pci";
    const double BLOCK_SIZE = 512.0;
}

SysfsReader::~SysfsReader() {}

void SysfsReader::update_drive_info(const std::string& drive_name, RawSysfsDevice& device) const {
    // get class device
    sysfs_class_device* drive_classdev = sysfs_open_class_device(CLASS_BLOCK, drive_name.c_str());
    if (drive_classdev != nullptr) {
        // get class device size
        sysfs_attribute* size_attr = sysfs_get_classdev_attr(drive_classdev, ATTRIBUTE_SIZE);
        if (size_attr != nullptr) {
            double drive_size = std::stod(size_attr->value) * ::BLOCK_SIZE;
            device.drive_names.emplace_back(drive_name);
            device.drive_sizes.emplace_back(drive_size);
        }
        else {
            log_error(GET_LOGGER("sysfs-reader"), "Cannot read drive size for " << drive_name);
        }
    }
    else {
        log_error(GET_LOGGER("sysfs-reader"), "Cannot open block device for " << drive_name);
    }
}

void SysfsReader::update_pci_device_drives(sysfs_device* sys_device, RawSysfsDevice& device) const {

    // open block device class
    sysfs_class* block_class = sysfs_open_class(::CLASS_BLOCK);

    if (block_class != nullptr) {

        // prepare regexp
        std::string reg_expr_str_pre = "/sys/devices/pci....:../(?:....:..:..\\../)*";
        std::string reg_expr_str_post = "/(.*)";
        std::string reg_expr_str = reg_expr_str_pre + sys_device->name + reg_expr_str_post;

        // check each block device
        dlist* block_devices = sysfs_get_class_devices(block_class);
        sysfs_class_device* block_device = 0;
        dlist_for_each_data(block_devices, block_device, struct sysfs_class_device) {

            // check if this is a device on the bus - look for a specific path pattern
            static const int NUM_OF_SUB_EXPR = 2;
            std::string path = block_device->path;

            std::smatch matches{};
            std::regex_match(path, matches, std::regex(reg_expr_str, std::regex_constants::icase));

            if (NUM_OF_SUB_EXPR == matches.size()) {
                update_drive_info(block_device->name, device);
            }
        }
        sysfs_close_class(block_class);
    }
    else {
        log_error(GET_LOGGER("sysfs-reader"), "Cannot open block device class");
    }
}

void SysfsReader::update_pci_device_virtual(sysfs_device* sys_device, RawSysfsDevice& device) const {
    // check if function is virtual, if attribute physfn exists then it is virtual
    sysfs_attribute* sys_attribute = sysfs_get_device_attr(sys_device, ::ATTRIBUTE_PHYS_FN);
    device.is_virtual = (sys_attribute == nullptr ? false : true);
}

bool SysfsReader::update_pci_device_config(sysfs_device* sys_device, RawSysfsDevice& device) const {
    sysfs_attribute* sys_attribute = sysfs_get_device_attr(sys_device, ::ATTRIBUTE_CONFIG);
    if (sys_attribute != nullptr) {
        return EOK == memcpy_s(&device.configuration, RawSysfsDevice::PCI_CONFIGURATION_SPACE_SIZE,
            sys_attribute->value, std::min(std::uint32_t{sys_attribute->len}, RawSysfsDevice::PCI_CONFIGURATION_SPACE_SIZE));
    }
    log_error(GET_LOGGER("sysfs-reader"), "Cannot read PCI configuration space");
    return false;
}

bool SysfsReader::update_pci_device_ids(sysfs_device* sys_device, RawSysfsDevice& device) const {
    device.path = sys_device->path;
    device.name = sys_device->name;
    try {
        device.id = SysfsId::from_string(sys_device->name);
    }
    catch (const std::invalid_argument& e) {
        log_error(GET_LOGGER("sysfs-reader"), "Invalid path format while processing device: " << e.what());
        return false;
    }
    return true;
}

bool SysfsReader::update_pci_device(sysfs_device* sys_device, RawSysfsDevice& device) const {
    RawSysfsDevice result{};
    if (sys_device != nullptr && update_pci_device_config(sys_device, result)
                              && update_pci_device_ids(sys_device, result)) {

        update_pci_device_virtual(sys_device, result);
        update_pci_device_drives(sys_device, result);
        device = result;
        return true;
    }
    log_error(GET_LOGGER("sysfs-reader"), "PCI device discovery failed");
    return false;
}

bool SysfsReader::is_path_matched(sysfs_device* sys_device, const std::string& path) const {
    if (sys_device == nullptr) {
        return false;
    }
    if (path.empty()) {
        return true;
    }
    std::string dev_path = sys_device->path;
    std::smatch matches{};
    std::regex_match(dev_path, matches, std::regex(".*" + path + ".*", std::regex_constants::icase));
    return (matches.size() == 1);
}

std::vector<RawSysfsDevice> SysfsReader::get_raw_sysfs_devices(const std::string& path) const {
    std::vector<RawSysfsDevice> devices{};

    // open bus
    struct sysfs_bus* sys_bus = sysfs_open_bus(::BUS_NAME);
    if (sys_bus != nullptr) {
        // get list of all devices
        struct dlist* sys_devices = sysfs_get_bus_devices(sys_bus);
        if (sys_devices != nullptr) {
            // iterate through the list and read data about each drive
            struct sysfs_device* sys_device = nullptr;
            dlist_for_each_data(sys_devices, sys_device, struct sysfs_device) {
                RawSysfsDevice device{};
                if (is_path_matched(sys_device, path)) {
                    if (update_pci_device(sys_device, device)) {
                        devices.emplace_back(device);
                    }
                }
            }
        }
        else {
            log_error(GET_LOGGER("sysfs-reader"), "Cannot read PCI bus devices");
        }
        sysfs_close_bus(sys_bus);
    }
    else {
        log_error(GET_LOGGER("sysfs-reader"), "Cannot open PCI bus");
    }

    return devices;
}

std::vector<std::string> SysfsReader::get_drives_for_device(
        const std::string& switch_bridge_path, uint8_t bus_num, uint8_t device_num) const {

    std::vector<std::string> drives{};

    // open block device class
    sysfs_class* block_class = sysfs_open_class(::CLASS_BLOCK);

    if (block_class != nullptr) {

        // build device name
        std::stringstream str_dev{};
        // there is '.' in the switch name - but that should not be a problem
        str_dev << std::hex << switch_bridge_path << "/....:";
        str_dev << std::setw(2) << std::setfill('0') << unsigned(bus_num) << ":";
        str_dev << std::setw(2) << std::setfill('0') << unsigned(device_num) << "\\../";
        str_dev << "(?:....:..:..\\../)*(.*)/(\\1.*)/(\\2.*)";
        std::string regex_str = str_dev.str();

        // check each block device
        dlist* block_devices = sysfs_get_class_devices(block_class);
        sysfs_class_device* block_device = 0;
        dlist_for_each_data(block_devices, block_device, struct sysfs_class_device) {

            // check if there is a drive on the device
            static const int NUM_OF_SUB_EXPR = 4;
            std::string path = block_device->path;

            std::smatch matches{};
            std::regex_match(path, matches, std::regex(regex_str, std::regex_constants::icase));
            if (NUM_OF_SUB_EXPR == matches.size()) {
                drives.push_back(matches[3]);
            }
        }
        sysfs_close_class(block_class);
    }
    else {
        log_error(GET_LOGGER("sysfs-reader"), "Cannot open block device class");
    }

    return drives;
}

bool SysfsReader::unbind_nvme_driver(const SysfsId& id) const {
    struct sysfs_driver* driver = sysfs_open_driver(BUS_NAME, DRIVER_NVME);
    if (nullptr == driver) {
        log_error(GET_LOGGER("sysfs-reader"), "Cannot open driver");
        return false;
    }
    struct sysfs_attribute* attr = sysfs_get_driver_attr(driver, ATTRIBUTE_UNBIND);
    if (nullptr == attr) {
        log_error(GET_LOGGER("sysfs-reader"), "Cannot open unbind attribute");
        return false;
    }
    std::string device = id.to_string();
    if (-1 == sysfs_write_attribute(attr, device.c_str(), device.size())) {
        log_error(GET_LOGGER("sysfs-reader"), "Cannot unbind driver");
        return false;
    }

    sysfs_close_driver(driver);
    return true;
}
