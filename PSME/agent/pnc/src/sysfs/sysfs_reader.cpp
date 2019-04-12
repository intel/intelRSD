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

#include <iostream>



using namespace agent::pnc::sysfs;
using namespace ::sysfs;

namespace {

const char PATH_DRIVER_NVME_UNBIND[] = "/sys/bus/pci/drivers/nvme/unbind";
const char PATH_BUS_PCI_DEVICES[] = "/sys/bus/pci/devices";
const char PATH_CLASS_BLOCK[] = "/sys/block";
const char PATH_CLASS_FPGA[] = "/sys/class/fpga";
const char ATTRIBUTE_PHYS_FN[] = "physfn";
const char ATTRIBUTE_CONFIG[] = "config";
const char ATTRIBUTE_SIZE[] = "size";
const double BLOCK_SIZE = 512.0;
}


SysfsReader::~SysfsReader() {}


void SysfsReader::update_drive_info(const std::string& drive_name, RawSysfsDevice& device) const {
    // get class device
    try {
        SysfsFile size_file = m_sysfs_interface->get_file(Path(PATH_CLASS_BLOCK) / drive_name / ATTRIBUTE_SIZE);
        double drive_size = std::stod(size_file.value) * ::BLOCK_SIZE;
        device.drive_names.emplace_back(drive_name);
        device.drive_sizes.emplace_back(drive_size);
    }
    catch (const std::exception& e) {
        log_error("sysfs-reader", "Exception while reading drive info for drive " << drive_name
                                                                                  << ": " << e.what());
    }
}


void SysfsReader::update_pci_device_drives(const Path& device_path, RawSysfsDevice& device) const {

    // read all block devices
    SysfsDir block_devices{};
    try {
        block_devices = m_sysfs_interface->get_dir(Path(PATH_CLASS_BLOCK));
    }
    catch (const std::exception& e) {
        log_error("sysfs-reader", "Exception while reading block devices: " << e.what());
        return;
    }

    // prepare regexp
    std::string reg_expr_str_pre = "/sys/devices/pci....:../(?:....:..:..\\../)*";
    std::string reg_expr_str_post = "/(.*)";
    std::string reg_expr_str = reg_expr_str_pre + device_path.basename() + reg_expr_str_post;
    static const int NUM_OF_SUB_EXPR = 2;

    for (const auto& link_path : block_devices.links) {
        try {
            std::string absolute_path_str = m_sysfs_interface->get_absolute_path(link_path).to_string();
            std::smatch matches{};

            std::regex_match(absolute_path_str, matches, std::regex(reg_expr_str, std::regex_constants::icase));
            if (NUM_OF_SUB_EXPR == matches.size()) {
                update_drive_info(link_path.basename(), device);
            }
        }
        catch (const std::exception& e) {
            log_error("sysfs-reader", "Exception while reading symlink " << link_path.to_string()
                                                                         << ": " << e.what());
        }
    }
}


void SysfsReader::update_pci_device_virtual(const Path& device_path, RawSysfsDevice& device) const {
    // check if function is virtual, if attribute physfn exists then it is virtual
    device.is_virtual = m_sysfs_interface->file_exists(device_path / ::ATTRIBUTE_PHYS_FN);
}


bool SysfsReader::update_pci_device_config(const Path& device_path, RawSysfsDevice& device) const {
    try {
        auto config = m_sysfs_interface->get_file(device_path / ::ATTRIBUTE_CONFIG);
        if (config.value.size() > std::numeric_limits<uint32_t>::max()) {
            throw std::runtime_error("PCI configuration space size is too large!");
        }
        return EOK == memcpy_s(&device.configuration, RawSysfsDevice::PCI_CONFIGURATION_SPACE_SIZE,
                               config.value.c_str(),
                               std::min(uint32_t(config.value.size()), RawSysfsDevice::PCI_CONFIGURATION_SPACE_SIZE));
    }
    catch (const std::exception& e) {
        log_error("sysfs-reader", "Exception while reading PCI configuration space: " << e.what());
        return false;
    }
}


bool SysfsReader::update_pci_device_ids(const Path& device_path, RawSysfsDevice& device) const {
    device.path = m_sysfs_interface->get_absolute_path(device_path).to_string();
    device.name = device_path.basename();
    try {
        device.id = SysfsId::from_string(device.name);
    }
    catch (const std::invalid_argument& e) {
        log_error("sysfs-reader", "Invalid path format while processing device: " << e.what());
        return false;
    }
    return true;
}


bool SysfsReader::update_pci_device(const Path& device_path, RawSysfsDevice& device) const {
    RawSysfsDevice result{};
    if (update_pci_device_config(device_path, result) && update_pci_device_ids(device_path, result)) {

        update_pci_device_virtual(device_path, result);
        update_pci_device_drives(device_path, result);
        device = result;
        return true;
    }
    log_error("sysfs-reader", "PCI device discovery failed");
    return false;
}


bool SysfsReader::is_path_matched(const Path& device_path, const std::string& path) const {
    std::string device_path_str = device_path.to_string();
    if (device_path_str.empty()) {
        return false;
    }
    if (path.empty()) {
        return true;
    }
    std::smatch matches{};
    std::regex_match(device_path_str, matches, std::regex(".*" + path + ".*", std::regex_constants::icase));
    return (matches.size() == 1);
}


std::vector<RawSysfsDevice> SysfsReader::get_raw_sysfs_devices(const std::string& path) const {

    std::vector<RawSysfsDevice> devices{};

    // open bus directory
    SysfsDir pci_devices{};
    try {
        pci_devices = m_sysfs_interface->get_dir(PATH_BUS_PCI_DEVICES);
    }
    catch (const std::exception& e) {
        log_error("sysfs-reader", "Exception while reading PCI devices: " << e.what());
        return {};
    }

    // check all links in the dir
    for (const Path& device_link_path : pci_devices.links) {
        RawSysfsDevice sysfs_device{};

        // if path is matched, try reading device data and add it to the returned vector
        if (is_path_matched(device_link_path, path) && update_pci_device(device_link_path, sysfs_device)) {
            devices.push_back(std::move(sysfs_device));
        }
    }

    return devices;
}


PcieAddress SysfsReader::get_pcie_address_for_fpga_device(
    const std::string& switch_bridge_path, uint8_t bus_num, uint8_t device_num) const {

    PcieAddress pcie_address{};

    // open sys class directory
    SysfsDir fpga_devices{};
    try {
        fpga_devices = m_sysfs_interface->get_dir(PATH_CLASS_FPGA);
    }
    catch (const std::exception& e) {
        log_error("sysfs-reader", "Exception while reading block devices: " << e.what());
        return {};
    }


    // build device name
    std::stringstream str_dev{};
    // there is '.' in the switch name - but that should not be a problem
    str_dev << std::hex << switch_bridge_path << "/....:";
    str_dev << std::setw(2) << std::setfill('0') << unsigned(bus_num) << ":";
    str_dev << std::setw(2) << std::setfill('0') << unsigned(device_num) << "\\../";
    str_dev << "(?:....:..:..\\../)*(.*)/(\\1.*)/(\\2.*)";
    std::string regex_str = str_dev.str();


    // for each link in that fpga, only one matching pcie_address_regex
    for (const auto& link_path : fpga_devices.links) {
        try {
            std::string absolute_path_str = m_sysfs_interface->get_absolute_path(link_path).to_string();

            // Looking for device links
            absolute_path_str += std::string("/device");

            SysfsLink fpga_pcie_link{};
            try {
                fpga_pcie_link = m_sysfs_interface->get_link(absolute_path_str);
            }
            catch (const std::exception& e) {
                log_error("sysfs-reader", "Exception while reading block devices: " << e.what());
            }

            // value is in format "../../../0000:3d:00.0"
            std::string fpga_link_string(fpga_pcie_link.value);
            std::regex pcie_address_regex("(..\\/..\\/..\\/(0000):(..):(..).(.))");

            auto pcie_address_begin = std::sregex_iterator(fpga_link_string.begin(), fpga_link_string.end(),
                                                    pcie_address_regex);
            auto pcie_address_end = std::sregex_iterator();

            if (pcie_address_begin == pcie_address_end) {
                throw std::runtime_error("Exception while looking for FPGA pcie address");
            }
            for (std::sregex_iterator i = pcie_address_begin; i != pcie_address_end; ++i) {
                std::smatch match = *i;
                std::string match_str = match.str();

                try {
                    // bus device and function are 3rd, 4th, an 5th group in regex, all in 16 base
                    pcie_address.bus = static_cast<uint8_t>(std::stoi(match.str(3).c_str(), 0, 16));
                    pcie_address.device = static_cast<uint8_t>(std::stoi(match.str(4).c_str(), 0, 16));
                    pcie_address.function = static_cast<uint8_t>(std::stoi(match.str(5).c_str(), 0, 16));
                }
                catch (std::invalid_argument& e) {
                    log_error("sysfs-reader", "Exception while reading FPGA pcie address: " << e.what());

                }
            }

        }
        catch (const std::exception& e) {
            log_error("sysfs-reader", "Exception while reading symlink " << link_path.to_string() << ": " << e.what());
        }
    }
    return pcie_address;
}


std::vector<std::string> SysfsReader::get_drives_for_device(
    const std::string& switch_bridge_path, uint8_t bus_num, uint8_t device_num) const {

    std::vector<std::string> drives{};

    // open bus directory
    SysfsDir block_devices{};
    try {
        block_devices = m_sysfs_interface->get_dir(PATH_CLASS_BLOCK);
    }
    catch (const std::exception& e) {
        log_error("sysfs-reader", "Exception while reading block devices: " << e.what());
        return {};
    }

    // build device name
    std::stringstream str_dev{};
    // there is '.' in the switch name - but that should not be a problem
    str_dev << std::hex << switch_bridge_path << "/....:";
    str_dev << std::setw(2) << std::setfill('0') << unsigned(bus_num) << ":";
    str_dev << std::setw(2) << std::setfill('0') << unsigned(device_num) << "\\../";
    str_dev << "(?:....:..:..\\../)*(.*)/(\\1.*)/(\\2.*)";
    std::string regex_str = str_dev.str();
    static constexpr int NUM_OF_SUB_EXPR = 4;

    for (const auto& link_path : block_devices.links) {
        try {
            std::string absolute_path_str = m_sysfs_interface->get_absolute_path(link_path).to_string();

            std::smatch matches{};
            std::regex_match(absolute_path_str, matches, std::regex(regex_str, std::regex_constants::icase));
            if (NUM_OF_SUB_EXPR == matches.size()) {
                drives.push_back(matches[3]);
            }
        }
        catch (const std::exception& e) {
            log_error("sysfs-reader", "Exception while reading symlink " << link_path.to_string() << ": " << e.what());
        }
    }
    return drives;
}


bool SysfsReader::unbind_nvme_driver(const SysfsId& id) const {
    try {
        m_sysfs_interface->set_file(Path(PATH_DRIVER_NVME_UNBIND), id.to_string());
    }
    catch (const std::exception& e) {
        log_error("sysfs-reader", "Exception while unbinding NVMe driver for device " << id.to_string()
                                                                                      << ": " << e.what());
        return false;
    }
    return true;
}
