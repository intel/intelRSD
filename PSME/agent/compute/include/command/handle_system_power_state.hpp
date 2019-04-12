/*!
 * @brief Declaration of PowerState related commands
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file handle_system_power_state.hpp
 */

#pragma once

#include "agent-framework/command/compute_commands.hpp"
#include "agent-framework/module/compute_components.hpp"
#include "ipmi/command/generic/chassis_control_command.hpp"


namespace agent {
namespace compute {

enum class PowerStateCycle {
    ON,   //!< Leave the platform ON
    OFF,  //!< Leave the platform OFF
    CYCLE //!< Leave the platform in "starting" state, but with changing the state
};

void handle_system_power_state(ipmi::command::generic::request::ChassisControlCommand::PowerState power_state,
                               PowerStateCycle cycle, const std::string& action, const Uuid& system_uuid,
                               std::function<void()> pre_action = []{}, std::function<void()> cycle_on_action = []{});

Uuid process_system_power_state(const Uuid& system_uuid, agent_framework::model::enums::ResetType power_state);

}
}
