/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * */

#include "loader/state_machine_action_loader.hpp"
#include "logger/logger_factory.hpp"

#include "status/state_machine_action.hpp"

#include "json-wrapper/json-wrapper.hpp"

#include <memory>
#include <stdexcept>

using namespace agent::chassis::loader;

bool StateMachineActionLoader::load(const json::Json&) {
    m_is_loaded = true;
    return true;
}

using agent::chassis::StateMachineAction;
using StateMachineActionUnique = std::unique_ptr<StateMachineAction>;

StateMachineActionUnique StateMachineActionLoader::get_state_machine() const {
    if (!m_is_loaded) {
        throw std::runtime_error("StateMachineLoader is not loaded.");
    }

    auto machine_action = new StateMachineAction();
    StateMachineActionUnique state_machine_action(machine_action);

    return state_machine_action;
}

