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
 * @file nvme_drive_reader.cpp
 */

#include "drive_handling/nvme_drive_reader.hpp"
#include "logger/logger.hpp"

using namespace agent::storage;

namespace {

static const char PATH_NVME_CLASS[] = "/sys/class/nvme";
static const char PATH_MOUNTS[] = "/proc/mounts";

bool is_drive_mounted(const std::string& drive, const std::vector<sysfs::SysfsMount>& mounts) {
    /*
     * Without 'n' we would have false matches if we had enough drives
     * i.e. for 11 drives, there will be entries like nvme0n1p1, nvme1n1p1, ..., nvme10n1p1
     * if the file has the entry for nvme10, then nvme1 would be also matched,
     * 'n' is added to make sure this does not happen
     */
    for (const auto& mount : mounts) {
        if (mount.name.find(drive + "n") != std::string::npos) {
            return true;
        }
    }
    return false;
}

}

NvmeDriveReader::~NvmeDriveReader() { }

std::vector<std::string> NvmeDriveReader::get_drives() const {

    std::vector<sysfs::SysfsMount> mounts{};

    try {
        // Read mounts in the system
        mounts = m_sysfs_interface->get_mounts(sysfs::Path(::PATH_MOUNTS));
    }
    catch (const std::runtime_error& e) {
        log_error("drive-reader", "Exception while reading mounts: " << e.what());
        // No point of checking drives if we do not know which are used by the system
        return {};
    }

    sysfs::SysfsDir nvme_class_dir{};
    try {
        // Path may throw logic_error, get_dir may throw runtime_error
        nvme_class_dir = m_sysfs_interface->get_dir(sysfs::Path(::PATH_NVME_CLASS));

        std::vector<std::string> drives{};
        for (const auto& drive_path : nvme_class_dir.links) {
            auto drive_name = drive_path.basename();

            // Ignore mounted drives
            if (!is_drive_mounted(drive_name, mounts)) {
                drives.emplace_back(std::move(drive_name));
            }
        }
        return drives;
    }
    catch (const std::exception& e) {
        log_error("drive-reader", "Exception while opening " << ::PATH_NVME_CLASS << " device class: " << e.what());
        return {};
    }
}
