/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * @file loader.hpp
 * @brief Module loader interface
 * */

#pragma once
#include "agent-framework/module/loader/loader.hpp"
#include <memory>

namespace agent {
namespace chassis {
    /* Forward declaration */
    class StateMachineAction;

namespace loader {

class StateMachineActionLoader: public agent_framework::module::loader::Loader {
public:

    enum class IpmiImplementation: std::uint8_t {
        OPENIPMI,
        IPMITOOL,
        DEFAULT = OPENIPMI,
    };

    virtual bool load(const json::Json&);

    /*!
     * @brief Returns built StateMachineAction.
     *
     * Before use of this function you need to load configuration. Otherwise
     * this function will throw exception.
     *
     * @see load(const json::Json&);
     * @throw runtime_error if configuration has not been loaded.
     * @return unique pointer to StateMachineAction object with filled data.
     */
    std::unique_ptr<StateMachineAction> get_state_machine() const;

private:
    bool m_is_loaded{false};
};

}
}
}
