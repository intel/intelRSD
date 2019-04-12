/*!
 * @brief Set port LLDP enabled command class declaration.
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
 * @file set_port_lldp_enabled.hpp
 */

#pragma once

#include "hal/eos_eapi/eapi_command.hpp"

namespace agent {
namespace network {
namespace hal {

/*!
 * @brief Class representing eapi Set Port LLDP command
 */
class SetPortLldpEnabled : public EapiCommand {
public:

    /*!
     * Constructor
     * @param interface ethernet interface identifier (from the switch)
     * @param state whether LLDP is supposed to be enabled (true) or disabled (false)
     */
    SetPortLldpEnabled(std::string interface, bool state)
        : EapiCommand{EAPI_COMMAND_JSON}, m_interface{interface}, m_state{state} { }

    /*! Destructor */
    virtual ~SetPortLldpEnabled();

    /*!
     * Serialize command to a vector of switch commands that need to be called consequently
     * @return vector of switch commands
     */
    virtual std::vector<std::string> serialize() const override;

private:
    std::string m_interface{};
    bool m_state{};
};


}
}
}

