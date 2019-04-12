/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file nvme/nvme_invoker.cpp
 */

#include "nvme/nvme_invoker.hpp"
#include "nvme/nvme_exception.hpp"

#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sstream>



using namespace nvme;
using namespace nvme::commands;

#define NVME_IOCTL_ADMIN_CMD _IOWR('N', 0x41, struct CommandData)
#define NVME_IOCTL_NVM_CMD _IOWR('N', 0x43, struct CommandData)
#define NVME_IOCTL_RESET_CMD _IO('N', 0x44)

namespace {

static constexpr char DEV_PATH[] = "/dev/";


template<unsigned long IOCTL_CODE, typename... Args>
void execute_ioctl_call(const std::string& target, Args... args) {
    std::string path = std::string{DEV_PATH} + target;
    int fd = open(path.c_str(), O_RDONLY);
    if (fd < 0) {
        throw NvmeException(std::string{"Cannot open NVMe device file: "} + strerror(errno));
    }

    int return_code = ioctl(fd, IOCTL_CODE, args...);
    close(fd);

    if (return_code == -1) {
        throw NvmeException(std::string{"Ioctl error: "} + strerror(errno));
    }
    else if (return_code != 0) {
        StatusField sf{uint16_t(return_code)};
        if (!sf.is_successful()) {
            throw NvmeException(sf);
        }
    }
}


void execute_nvme_reset(const std::string& target) {
    execute_ioctl_call<NVME_IOCTL_RESET_CMD>(target);
}


void execute_admin_command(const std::string& target, CommandData* command) {
    execute_ioctl_call<NVME_IOCTL_ADMIN_CMD>(target, command);
}


void execute_nvm_command(const std::string& target, CommandData* command) {
    execute_ioctl_call<NVME_IOCTL_NVM_CMD>(target, command);
}

}


NvmeInvoker::~NvmeInvoker() {}


void NvmeInvoker::execute(const std::string& target, GenericNvmeCommand& command) const {
    switch (command.get_type()) {
        case NvmeCommandType::AdminCommand:
            execute_admin_command(target, &command.get_data());
            break;
        case NvmeCommandType::NvmCommand:
            execute_nvm_command(target, &command.get_data());
            break;
        case NvmeCommandType::ControllerReset:
            execute_nvme_reset(target);
            break;
        default:
            throw NvmeException("Invalid command type: " + std::to_string(int(command.get_type())));
    }
}
