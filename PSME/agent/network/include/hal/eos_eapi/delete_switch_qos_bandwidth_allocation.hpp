/*!
 * @brief Delete QoS bandwidth allocation command class declaration.
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
 * @file delete_switch_qos_bandwidth_allocation.hpp
 */

#pragma once

#include "hal/eos_eapi/eapi_command.hpp"

namespace agent {
namespace network {
namespace hal {

/*!
 * @brief Class representing eAPI Delete QoS Bandwidth Allocation command
 */
class DeleteSwitchQosBandwidthAllocation : public EapiCommand {
public:

    /*!
     * Constructor
     * @param bandwidth Bandwidth percent allocated for the priority group
     * @param group Priority group.
     */
    DeleteSwitchQosBandwidthAllocation(std::uint32_t bandwidth_percent, std::uint32_t priority_group)
        : EapiCommand{EAPI_COMMAND_JSON},
          m_bandwidth_percent{bandwidth_percent}, m_priority_group{priority_group} { }

    /*! Destructor */
    virtual ~DeleteSwitchQosBandwidthAllocation();

    /*!
     * Serialize command to a vector of switch commands that need to be called consequently
     * @return vector of switch commands
     */
    virtual std::vector<std::string> serialize() const override;

private:
    std::uint32_t m_bandwidth_percent{};
    std::uint32_t m_priority_group{};
};



}
}
}

