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

#include "status/module_hardware_status.hpp"
#include "agent-framework/logger_ext.hpp"
#include "configuration/configuration.hpp"
#include <chrono>
#include <thread>

/*! SMBIOS MDR Data Update Time */
constexpr const std::uint8_t SMBIOS_UPDATE_TIME_MINUTES = 3;

using namespace agent::compute::status;
using configuration::Configuration;
using Status = agent_framework::status::ModuleStatus::Status;

namespace {
    std::string to_string(agent_framework::status::ModuleStatus::Status status) {
        switch (status) {
            case agent_framework::status::ModuleStatus::Status::NOT_PRESENT:
                return "NOT PRESENT";
            case agent_framework::status::ModuleStatus::Status::PRESENT:
                return "PRESENT";
            case agent_framework::status::ModuleStatus::Status::UNKNOWN:
                return "UNKNOWN";
            case agent_framework::status::ModuleStatus::Status::DETERMINING:
                return "DETERMINING";
            default:
                return "UNKNOWN";
        }
    }
}

bool ModuleHardwareStatus::went_up() {
    if (agent_framework::status::ModuleStatus::Status::NOT_PRESENT == m_last_status &&
        agent_framework::status::ModuleStatus::Status::PRESENT == m_status ) {

        return true;
    }
    return false;
}

bool ModuleHardwareStatus::went_down() {
    if (agent_framework::status::ModuleStatus::Status::PRESENT == m_last_status &&
        agent_framework::status::ModuleStatus::Status::NOT_PRESENT == m_status ) {

            return true;
    }
    return false;
}

void ModuleHardwareStatus::wait_for_smbios() {
    log_debug(GET_LOGGER("agent"), "Module Slot: " << m_slot << " Wait for SMBIOS..");
    std::this_thread::sleep_for(std::chrono::minutes(SMBIOS_UPDATE_TIME_MINUTES));
    log_debug(GET_LOGGER("agent"), "Module Slot: " << m_slot << " Wait for SMBIOS.. Done.");
}

Status ModuleHardwareStatus::read_status() {
    m_status = m_gpio_status.read_from_file();
    log_debug(GET_LOGGER("status"), "Module Slot: " << m_slot << " HW Status: " << to_string(m_status));
    if (went_up()) {
        log_debug(GET_LOGGER("agent"), "Module Slot: " << m_slot << " Transition from NOT PRESENT to PRESENT");
        wait_for_smbios();
    }
    if (went_down()){
        log_debug(GET_LOGGER("agent"), "Module Slot: " << m_slot << " Transition from PRESENT to NOT PRESENT");
    }
    m_last_status = m_status;
    return m_status;
}
