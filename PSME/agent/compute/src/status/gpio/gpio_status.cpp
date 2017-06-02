/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * */

#include "status/gpio/gpio_status.hpp"
#include "agent-framework/logger_ext.hpp"
#include "configuration/configuration.hpp"

#include <fstream>

using namespace agent::compute::status;
using configuration::Configuration;
using Status = agent_framework::status::ModuleStatus::Status;

static const constexpr char GPIO_PRESENCE_FILE_PATH[] = "/tmp/gpio";

Status GpioStatus::read_from_file() {
    Status status{agent_framework::status::ModuleStatus::Status::NOT_PRESENT};
    try {
        std::ifstream ifs;
        std::string content{};
        uint32_t presence_mask = 1;

        ifs.open(GPIO_PRESENCE_FILE_PATH);
        if (ifs.is_open() && getline(ifs, content)) {
            auto presence = std::stoul(content, nullptr, 10);
            if (presence & (presence_mask << (m_slot - 1))) {
                status = Status::PRESENT;
            }
            else {
                status = Status::NOT_PRESENT;
            }
            ifs.close();
        }
    }
    catch (const std::exception& e) {
        log_debug(GET_LOGGER("agent"), e.what());
        log_error(GET_LOGGER("agent"), "Cannot read GPIO status.");
    }
    return status;
}
