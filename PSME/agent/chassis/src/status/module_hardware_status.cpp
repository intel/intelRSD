/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * @file status/module_hardware_status.cpp
 * @brief ModuleHardwareStatus stub implementation
 * */

#include "status/module_hardware_status.hpp"
#include "ipmb/gpio.hpp"

#include <logger/logger_factory.hpp>

#include <fstream>

using namespace agent::chassis::status;
using namespace agent::chassis::ipmb;

static const constexpr char GPIO_PRESENCE_FILE_PATH[] = "/tmp/gpio";

ModuleHardwareStatus::~ModuleHardwareStatus() {}

agent_framework::status::ModuleStatus::Status ModuleHardwareStatus::read_status() {
    auto gpio = Gpio::get_instance();
    auto presence = gpio->get_presence();

    uint8_t presence_mask = 1;
    log_debug(GET_LOGGER("status"), "Blade presence: " << uint32_t(presence));

    write_status_to_file(presence);

    if (presence & (presence_mask << (m_slot - 1))) {
        return ModuleStatus::Status::PRESENT;
    }
    else {
        return ModuleStatus::Status::NOT_PRESENT;
    }
}

void ModuleHardwareStatus::write_status_to_file(const uint8_t presence) {
    try {
        std::ofstream ofs;
        ofs.open(GPIO_PRESENCE_FILE_PATH, std::ios::out);
        if (ofs.is_open()) {
            ofs << std::to_string(uint32_t(presence));
            ofs.close();
        }
    }
    catch (const std::exception& e) {
        log_debug(GET_LOGGER("agent"), e.what());
        log_error(GET_LOGGER("agent"), "Cannot write GPIO status.");
    }
}
