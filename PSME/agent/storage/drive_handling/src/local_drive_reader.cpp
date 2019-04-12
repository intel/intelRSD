/*!
 * @brief Drive reader implementation.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License") override;
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file local_drive_reader.cpp
 */

#include "drive_handling/local_drive_reader.hpp"
#include "logger/logger.hpp"



using namespace agent::storage;

namespace {

constexpr const unsigned long long DIRECT_ACCESS_TYPE = 0;
constexpr const char DEV_FS[] = "/dev";
constexpr const char MTAB_FILE[] = "/etc/mtab";
constexpr const char VIRTUAL[] = "/virtual/";
constexpr const char DEVICE_TYPE[] = "device/type";
constexpr const char SYSFS_BLOCK_CLASS[] = "/sys/class/block";


sysfs::Path get_devfs_path(const sysfs::SysfsDir& device) {
    return sysfs::Path(DEV_FS) / device.name;
}


bool is_virtual_device(const sysfs::SysfsDir& device) {
    if (device.path.to_string().find(VIRTUAL) != std::string::npos) {
        log_info("drive-reader", "Device " << get_devfs_path(device) << " is a virtual device.");
        return true;
    }
    return false;
}


bool is_direct_access_device(const sysfs::SysfsDir& device,
                             std::shared_ptr<sysfs::AbstractSysfsInterface> sysfs_interface) {
    try {
        auto type_string = sysfs_interface->get_file(device.path / DEVICE_TYPE).value;

        unsigned long long type = std::stoull(type_string);

        if (type != DIRECT_ACCESS_TYPE) {
            log_info("drive-reader", "Device " << get_devfs_path(device) << " has type: " << std::hex << type);
            return false;
        }
        return true;
    }
    catch (const std::exception& ex) {
        log_warning("drive-reader",
                    "Could not read the type of device " << get_devfs_path(device) << ": " << ex.what());
        return false;
    }
}


bool is_boot_device(const sysfs::SysfsDir& device, const std::vector<sysfs::SysfsMount>& mount_points) {
    for (const auto& mount : mount_points) {
        if (mount.name == get_devfs_path(device)) {
            log_info("drive-reader", "Device " << get_devfs_path(device) << " is a boot device.");
            return true;
        }
    }
    return false;
}

}


LocalDriveReader::~LocalDriveReader() {}


std::vector<std::string> LocalDriveReader::get_drives() const {
    std::vector<sysfs::SysfsMount> mount_points{};

    try {
        mount_points = m_sysfs_interface->get_mounts(sysfs::Path(::MTAB_FILE));
    }
    catch (const std::runtime_error& e) {
        log_error("drive-reader", "Exception while reading mounts: " << e.what());
        // No point of checking drives if we do not know which are used by the system
        return {};
    }

    try {
        sysfs::SysfsDir block_devices{};

        // Path may throw logic_error, get_dir may throw runtime_error
        block_devices = m_sysfs_interface->get_dir(sysfs::Path(::SYSFS_BLOCK_CLASS));

        std::vector<std::string> drives{};
        for (const auto& device_path : block_devices.links) {

            auto link = m_sysfs_interface->get_link(device_path);
            auto path = sysfs::Path(device_path).pop_right();
            auto device = m_sysfs_interface->get_dir(path / link.value);

            if (::is_virtual_device(device) == false &&
                ::is_direct_access_device(device, m_sysfs_interface) &&
                ::is_boot_device(device, mount_points) == false) {

                auto drive_name = device_path.basename();
                drives.emplace_back(std::move(drive_name));
            }
        }
        return drives;
    }
    catch (const std::exception& e) {
        log_error("drive-reader", "Exception while opening " << ::SYSFS_BLOCK_CLASS << " device class: " << e.what());
        return {};
    }
}

