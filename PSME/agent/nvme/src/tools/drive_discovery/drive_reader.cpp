/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation
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
 * @file drive_reader.cpp
 */

#include "tools/drive_discovery/drive_reader.hpp"
#include "logger/logger_factory.hpp"

#include <cstring>
#include <fstream>
#include <algorithm>
#include <list>
#include <iterator>
#include <mntent.h>

using namespace agent::nvme::tools;
using namespace sysfs;

namespace {

static const char PATH_BLOCK[] = "/sys/block";
static const char PATH_MOUNTS[] = "/proc/mounts";
static const char NVME_PREFIX[] = "nvme";

bool is_drive_mounted(const std::string& drive, const std::vector<SysfsMount>& mounts) {
    /* This condition is true only for single namespace drives. */
    for (const auto& mount : mounts) {
        if (mount.name.find(drive) != std::string::npos) {
            return true;
        }
    }
    return false;
}

}

DriveReader::~DriveReader() {
}

std::vector<std::string> DriveReader::get_drives() const {

    std::vector<SysfsMount> mounts{};

    // read mounts in the system
    try {
        mounts = m_sysfs_interface->get_mounts(Path(PATH_MOUNTS));
    }
    catch (const std::runtime_error& e) {
        log_error("drive-reader", "Exception while reading mounts: " << e.what());
        // no point of checking drives if we do not know which are used by the system
        return {};
    }

    SysfsDir nvme_class_dir{};
    try {
        // path may throw logic_error, get_dir may throw runtime_error
        nvme_class_dir = m_sysfs_interface->get_dir(Path(::PATH_BLOCK));

        std::vector<std::string> drives{};
        for (const auto& drive_path : nvme_class_dir.links) {
            auto drive_name = drive_path.basename();

	    if (drive_name.rfind(NVME_PREFIX, 0) != 0){
	      continue;
	    }
            // ignore mounted drives
            if (!is_drive_mounted(drive_name, mounts)) {
                drives.emplace_back(std::move(drive_name));
	    }
        }
        return drives;
    }
    catch (const std::exception& e) {
        log_error("drive-reader", "Exception while opening " << ::PATH_BLOCK
            << " device class: " << e.what());
        return {};
    }
}
