/*!
 * @brief Add QoS priority to priority group mapping command class declaration.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file add_switch_qos_priority_group_mapping.hpp
 */

#pragma once

#include "hal/eos_eapi/eapi_command.hpp"

namespace agent {
namespace network {
namespace hal {

/*!
 * @brief Class representing eAPI Add QoS Priority to Priority Group Mapping command
 */
class AddSwitchQosPriorityGroupMapping : public EapiCommand {
public:

    /*!
     * Constructor
     * @param priority Priority.
     * @param priority_group Priority group.
     */
    AddSwitchQosPriorityGroupMapping(std::uint32_t priority, std::uint32_t priority_group)
        : EapiCommand{EAPI_COMMAND_JSON},
          m_priority{priority}, m_priority_group{priority_group} { }

    /*! Destructor */
    virtual ~AddSwitchQosPriorityGroupMapping();

    /*!
     * Serialize command to a vector of switch commands that need to be called consequently
     * @return vector of switch commands
     */
    virtual std::vector<std::string> serialize() const override;

private:
    std::uint32_t m_priority{};
    std::uint32_t m_priority_group{};
};



}
}
}

