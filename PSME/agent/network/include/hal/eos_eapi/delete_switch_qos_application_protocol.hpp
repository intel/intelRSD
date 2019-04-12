/*!
 * @brief Delete QoS application protocol command class declaration.
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
 * @file delete_switch_qos_application_protocol.hpp
 */

#pragma once

#include "agent-framework/module/enum/network.hpp"
#include "hal/eos_eapi/eapi_command.hpp"

namespace agent {
namespace network {
namespace hal {

/*!
 * @brief Class representing eAPI Delete QoS Application Protocol command
 */
class DeleteSwitchQosApplicationProtocol : public EapiCommand {
public:

    using ProtocolId = agent_framework::model::enums::TransportLayerProtocol;

    /*!
     * Constructor
     * @param protocol Protocol id.
     * @param port Port.
     * @param priority Priority.
     */
    DeleteSwitchQosApplicationProtocol(ProtocolId protocol, std::uint32_t port, std::uint32_t priority)
        : EapiCommand{EAPI_COMMAND_JSON},
          m_protocol{protocol}, m_port{port}, m_priority{priority} { }

    /*! Destructor */
    virtual ~DeleteSwitchQosApplicationProtocol();

    /*!
     * Serialize command to a vector of switch commands that need to be called consequently
     * @return vector of switch commands
     */
    virtual std::vector<std::string> serialize() const override;

private:
    ProtocolId m_protocol;
    std::uint32_t m_port{};
    std::uint32_t m_priority{};
};



}
}
}

