/*!
 * @copyright Copyright (c) 2018-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file local_drive_handler.cpp
 */

#include "drive_handling/local_drive_handler.hpp"

#include "logger/logger.hpp"

#include <thread>

using namespace agent::storage;

namespace {

constexpr const std::uint32_t SECTOR_SIZE_BYTES = 512;
constexpr const char SYSFS_BLOCK_CLASS[] = "/sys/class/block";
constexpr const char SIZE[] = "size";
constexpr const std::uint32_t SSD_RPM_VALUE = 1;
constexpr const std::uint8_t CRITICAL_WARNING_FLAG_DEGRADED_RELIABILITY = 0x04;

}

LocalDriveHandler::~LocalDriveHandler() {}

void LocalDriveHandler::load() {
    try {
        auto device_path = sysfs::Path(::SYSFS_BLOCK_CLASS).append(m_name);
        m_capacity_value = m_sysfs_interface->get_file(device_path / ::SIZE).value;
    }
    catch (const std::exception& ex) {
        log_debug("storage-agent",
                  "Encountered an error while reading " << m_name << " size: " << ex.what());
    }

    m_ata_attributes = m_ioctl_interface->read_ata_attributes();
    if (m_ata_attributes == nullptr) {
        log_error("storage-agent", "Could not load ioctl ATA attributes for drive: " << m_name);
    }
}

BaseDriveHandler::DriveData LocalDriveHandler::get_drive_data() const {
    DriveData dd{};

    auto rpm = m_ioctl_interface->get_rpm(m_ata_attributes);
    if (rpm == ::SSD_RPM_VALUE) {
        dd.drive_type = BaseDriveHandler::SSD_TYPE;
    }
    else {
        dd.rpm = rpm;
        dd.drive_type = BaseDriveHandler::HDD_TYPE;
    }

    dd.serial_number = m_ioctl_interface->get_serial_number(m_ata_attributes);
    dd.manufacturer_number = m_ioctl_interface->get_manufacturer(m_ata_attributes);
    dd.model_number = m_ioctl_interface->get_model_number(m_ata_attributes);

    dd.block_size_bytes = ::SECTOR_SIZE_BYTES;
    dd.size_lba = std::stoull(m_capacity_value);
    dd.namespaces = {};
    return dd;
}

BaseDriveHandler::SmartData LocalDriveHandler::get_smart_info() const {
    BaseDriveHandler::SmartData sm{};
    if (m_ata_attributes == nullptr) {
        sm.critical_warnings = ::CRITICAL_WARNING_FLAG_DEGRADED_RELIABILITY;
    }
    return sm;
}
