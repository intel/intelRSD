/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
*/

#include "agent-framework/state_machine/state_thread_entry.hpp"
#include "agent-framework/status/status_manager.hpp"

using namespace agent_framework::state_machine;

static const constexpr uint32_t STARTING_INTERVALS_MAX_NUMBER = 5;

StateThreadEntry::StateThreadEntry(const std::string& module,
                                   const StatusManagerSharedPtr& manager) :
    m_module{module},
    m_manager{manager} {
    log_debug(GET_LOGGER("state-machine"), "Module " << module << " State Machine initialized.");
    m_state_machine.start();
}

StateThreadEntry::~StateThreadEntry() {
    m_state_machine.stop();
}

void StateThreadEntry::next_state() {
    auto hw_status = m_manager->get_hw_status();
    auto sw_status = m_manager->get_sw_status();

    if (enums::State::ENABLED == m_state_machine.get_state() ||
        enums::State::OFFLINE == m_state_machine.get_state() ||
        enums::State::UNKNOWN == m_state_machine.get_state() ) {
        m_starting_counter = STARTING_INTERVALS_MAX_NUMBER;
    }
    if (enums::State::ABSENT == m_state_machine.get_state()) {
        m_starting_counter = 0;
    }

    if (status::ModuleStatus::Status::PRESENT == hw_status &&
        status::ModuleStatus::Status::NOT_PRESENT == sw_status &&
            STARTING_INTERVALS_MAX_NUMBER > m_starting_counter) {
        m_starting_counter++;
        log_debug(GET_LOGGER("agent"), "Module:" << m_module
                         << " HW status: " << std::uint32_t(status::ModuleStatus::Status::DETERMINING)
                         << " SW status: " << std::uint32_t(sw_status));
        m_state_machine.set_next_state(status::ModuleStatus::Status::DETERMINING, sw_status);
    }
    else {
        log_debug(GET_LOGGER("agent"), "Module:" << m_module
                         << " HW status: " << std::uint32_t(hw_status)
                         << " SW status: " << std::uint32_t(sw_status));
        m_state_machine.set_next_state(hw_status, sw_status);
        m_starting_counter = 0;
    }
}
