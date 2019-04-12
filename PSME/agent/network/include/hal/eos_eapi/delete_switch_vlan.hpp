/*!
 * @brief Delete Switch Vlan command class declaration.
 *
 * @copyright Copyright (c) 2019 Intel Corporation.
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
 * @file delete_switch_vlan.hpp
 */

#pragma once

#include "agent-framework/module/enum/network.hpp"
#include "hal/eos_eapi/eapi_command.hpp"

namespace agent {
namespace network {
namespace hal {

/*!
 * @brief Class representing eAPI Delete Switch Vlan command
 */
class DeleteSwitchVlan : public EapiCommand {
public:

    using VlanId = std::uint32_t;

    /*!
     * Constructor
     * @param vlan_id Vlan id.
     */
    DeleteSwitchVlan(std::uint32_t vlan_id)
        : EapiCommand{EAPI_COMMAND_JSON},
          m_vlan_id{vlan_id} { }

    /*! Destructor */
    virtual ~DeleteSwitchVlan();

    /*!
     * Serialize command to a vector of switch commands that need to be called consequently
     * @return vector of switch commands
     */
    virtual std::vector<std::string> serialize() const override;

private:
    VlanId m_vlan_id{};
};



}
}
}

