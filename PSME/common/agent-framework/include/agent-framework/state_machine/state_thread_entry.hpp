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
 *
 * @file state_thread_entry.hpp
 * @brief State thread entry
 * */

#ifndef AGENT_FRAMEWORK_STATE_MACHINE_STATE_THREAD_ENTRY_HPP
#define AGENT_FRAMEWORK_STATE_MACHINE_STATE_THREAD_ENTRY_HPP

#include "agent-framework/state_machine/state_machine.hpp"

/*! Psme namespace */
namespace agent_framework {
/*! Generic namespace */
namespace status {
class StatusManager;
}
namespace state_machine {

class StateThreadEntry {
public:
    using StatusManagerSharedPtr = std::shared_ptr<StatusManager>;

    explicit StateThreadEntry(const std::string& module,
                              const StatusManagerSharedPtr& manager);
    ~StateThreadEntry();

    const std::string& get_module() const {
        return m_module;
    }

    /*!
     * @brief Get module state.
     * @return Integer value of module state.
     * */
    state_machine::enums::State get_state() const {
        return m_state_machine.get_state();
    }

    state_machine::enums::Transition get_transition() const {
        return m_state_machine.get_transition();
    }

    void next_state();

    bool is_state_changed() const {
        return m_state_machine.is_state_changed();
    }

private:
    std::string m_module{};
    std::uint32_t m_starting_counter{0};
    StatusManagerSharedPtr m_manager;
    state_machine::StateMachine m_state_machine{};
};

}
}

#endif /* AGENT_FRAMEWORK_STATE_MACHINE_STATE_THREAD_ENTRY_HPP */
