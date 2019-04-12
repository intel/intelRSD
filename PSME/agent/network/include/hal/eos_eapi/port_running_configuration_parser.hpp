/*!
 * @brief Eapi show running port configuration
 * command parser class declaration.
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
 * @file port_active_configuration.hpp
 */

#pragma once

#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/enum/network.hpp"
#include "agent-framework/module/utils/optional_field.hpp"
#include <string>
#include <vector>

namespace agent {
namespace network {
namespace hal {
namespace eapi {

/*!
 * Simple class for information
 * obtained from "configure interface Ethernet X/Y show active" command
 */
class PortRunningConfigurationParser {
public:

    /*!
     * Constructor
     * @param output switch command output
     */
    PortRunningConfigurationParser(const std::string& output) {
        parse(output);
    }

    /*!
     * Default constructor
     */
    PortRunningConfigurationParser() = delete;

    /*!
     * Destructor
     */
    virtual ~PortRunningConfigurationParser() = default;

    /*!
     * Is PFC enabled on the port
     * @return true if PFC on the port is enabled,
     * false otherwise (or when there is no info about PFC)
     */
    bool is_pfc_enabled() const {
        return m_pfc_enabled;
    }

    /*!
     * Get list of enabled priorities on the port
     * @return vector of enabled priorities if set, empty vector otherwise
     */
    const std::vector<std::uint32_t>& get_enabled_priorities() const {
        return m_pfc_priorities;
    }

    /*!
     * @brief Get DCBX state configured on the port
     * @return Enum string : Disabled, CEE or IEEE mode
     */
    agent_framework::model::enums::DcbxState get_dcbx_state() const {
        return  m_dcbx_state;
    }

    /*!
     * @brief Is LLDP enabled
     * @return true if LLDP is enabled, false otherwise
     */
    bool is_lldp_enabled() {
        return m_lldp_enabled;
    }

private:
    void parse(const std::string& output);
    void read_dcbx_state(const std::string& output);
    void read_pfc(const std::string& output);
    void read_lldp(const std::string& output);

    agent_framework::model::enums::DcbxState m_dcbx_state{agent_framework::model::enums::DcbxState::Disabled};
    bool m_pfc_enabled{};
    bool m_lldp_enabled{};
    std::vector<std::uint32_t> m_pfc_priorities{};
};


}
}
}
}
