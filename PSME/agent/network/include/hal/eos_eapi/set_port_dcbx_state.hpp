/*!
 * @brief Set port DCBX mode command class declaration.
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
 * @file set_port_dcbx_state.hpp
 */

#pragma once

#include "agent-framework/module/enum/network.hpp"
#include "hal/eos_eapi/eapi_command.hpp"

namespace agent {
namespace network {
namespace hal {

/*!
 * @brief Class representing eAPI Set Port DCBX mode command
 */
class SetPortDcbxState : public EapiCommand {
public:

    using DcbxState = agent_framework::model::enums::DcbxState;

    /*!
     * Constructor
     * @param interface ethernet interface identifier (from the switch)
     * @param dcbx_state whether DCBX mode is supposed to be EnabledIEEE, EnabledCEE or Disabled
     */
    SetPortDcbxState(std::string interface, DcbxState dcbx_state)
        : EapiCommand{EAPI_COMMAND_JSON}, m_interface{interface}, m_dcbx_state{dcbx_state} { }

    /*! Destructor */
    virtual ~SetPortDcbxState();

    /*!
     * Serialize command to a vector of switch commands that need to be called consequently
     * @return vector of switch commands
     */
    virtual std::vector<std::string> serialize() const override;

private:
    std::string m_interface{};
    DcbxState m_dcbx_state;
};

}
}
}