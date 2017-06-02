/*!
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
 *
 * @file state_thread_entry.hpp
 * @brief State thread entry
 * */

#pragma once



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


    void update_module(const std::string& module) {
        m_module = module;
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


    /*!
     * @brief Determine if the resources under a module has already been discovered.
     * @return True if module has been discovered, false otherwise.
     * */
    bool was_discovered() const {
        return m_was_discovered;
    }


    /*!
     * @brief Set module discovery status.
     * @param[in] discovered Module discovery status.
     * */
    void set_discovered(bool discovered) {
        m_was_discovered = discovered;
    }


private:
    std::string m_module{};
    std::uint32_t m_starting_counter{0};
    StatusManagerSharedPtr m_manager;
    state_machine::StateMachine m_state_machine{};
    bool m_was_discovered{false};
};

}
}

