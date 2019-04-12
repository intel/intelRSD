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
 * @file nvme_secure_erase.cpp
 * @brief NVMe drive secure erase implementation
 * */
extern "C" {
#include <linux/nvme_ioctl.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
}



#include <stdexcept>

#include "nvme/nvme_secure_erase.hpp"



using namespace agent::pnc::nvme;

namespace {
    constexpr uint32_t ERASE_ALL_NAMESPACES = 0xFFFFFFFF;
    constexpr uint32_t DEFAULT_TIMEOUT_MS = 120000;
}

void agent::pnc::nvme::secure_erase(const std::string& device_path) {

    /* Opening device */
    int device_file = open(device_path.c_str(), O_RDONLY);
    if (device_file < 0) {
        throw (std::runtime_error("Cannot open device. Error code: " + std::to_string(device_file)));
    }

    /* Prepare command struct */
    struct nvme_admin_cmd cmd;
    memset(&cmd, 0, sizeof(cmd));
    cmd.opcode = nvme_admin_format_nvm;
    cmd.nsid = ::ERASE_ALL_NAMESPACES;
    cmd.timeout_ms = ::DEFAULT_TIMEOUT_MS;
    /***************** Command dword10 structure ********************/
    //  Bit   | Description
    //  -----------------------
    //  31:12 | Reserved
    //  11:09 | Secure Erase Settings
    //     08 | Protection Information Location
    //  07:05 | Protection Information
    //     04 | Metadata Settings
    //  03:00 | LBA Format
    //
    // Protection information settings are currently not supported and thus not set
    cmd.cdw10 = static_cast<uint32_t>(SecureEraseSettings::NO_SECURE_ERASE);

    /* Ioctl call */
    int format_error = ioctl(device_file, NVME_IOCTL_ADMIN_CMD, &cmd);
    if (0 != format_error) {
        close(device_file);
        throw (std::runtime_error("Cannot format drive. Error code: " + std::to_string(format_error)));
    }
    /* Closing device */
    if (close(device_file) < 0) {
        throw (std::runtime_error("Cannot close device. Error code: " + std::to_string(device_file)));
    }
}
