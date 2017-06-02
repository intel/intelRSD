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
 *
 * @section DESCRIPTION
 *
 * @file ipmb/gpio.hpp
 * @brief GPIO reader.
 * */

#include "agent-framework/module/model/chassis.hpp"
#include "agent-framework/module/common_components.hpp"

#include <ipmb/utils.hpp>
#include <ipmb/gpio.hpp>
#include <ipmb/i2c_message.hpp>
#include <ipmb/service.hpp>
#include <ipmb/command/get_sled_presence.hpp>

#include <logger/logger_factory.hpp>
#include <exception>

using namespace agent::chassis::ipmb;
using namespace agent::chassis::ipmb::command;
using namespace agent_framework::module;
using namespace agent_framework::model;

using agent_framework::module::CommonComponents;

Gpio::~Gpio(){}

uint8_t Gpio::get_presence() {
    std::unique_lock<std::mutex> lk(presence_mutex);
    m_presence = read_presence();

    presence_cv.wait_for(lk, std::chrono::milliseconds(GPIO_READ_TIMEOUT_MS));

    return m_presence;
}

void Gpio::set_presence(const uint8_t presence) {
    if (m_presence != presence) {
        log_info(LOGUSR, "GPIO presence changed 0x" << std::hex << int(m_presence) << " -> 0x" << int(presence));
        m_presence = presence;
    }
    presence_cv.notify_all();
}

uint8_t Gpio::read_presence() const {
    auto drawer_manager_key = CommonComponents::get_instance()->
            get_module_manager().get_keys("");
    auto chassis_key = CommonComponents::get_instance()->
            get_chassis_manager().get_keys(drawer_manager_key.front());
    auto platform = CommonComponents::get_instance()->
            get_chassis_manager().get_entry(chassis_key.front()).get_platform();

    log_debug(LOGUSR, "Platform type: " << platform);
    switch (platform) {
        case enums::PlatformType::BDCR : return read_bdc_r_gpio();
        case enums::PlatformType::EDK:
        case enums::PlatformType::MF3:
        case enums::PlatformType::UNKNOWN:
        default:
            throw std::runtime_error("Unsupported platform type: "
                                        + std::string(platform.to_string()));
    }
}

uint8_t Gpio::read_bdc_r_gpio() const {
    IpmiMessage req{};

    GetSledPresence command{};
    command.pack(req);

    Service::send_request_process_response(req);

    return m_presence;
}
