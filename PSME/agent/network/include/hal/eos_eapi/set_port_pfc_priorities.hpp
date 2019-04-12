/*!
 * @brief Set port PFC priorities command class declaration.
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
 * @file set_port_pfc_priorities.hpp
 */

#pragma once


#include "hal/eos_eapi/eapi_command.hpp"

namespace agent {
namespace network {
namespace hal {

/*!
 * @brief Class representing eapi Set Port PFC priorities command
 */
class SetPortPfcPriorities : public EapiCommand {
public:

    /*!
     * Constructor
     * @param interface ethernet interface identifier (from the switch)
     * @param to_set list of priorities to be set on port
     * @param to_remove list of priorities to be removed from port
     */
    SetPortPfcPriorities(const std::string& interface,
                         const std::vector<std::uint32_t>& to_set,
                         const std::vector<std::uint32_t>& to_remove)
        : EapiCommand{EAPI_COMMAND_JSON_TEXT},
          m_interface{interface},
          m_priorities_to_set{to_set},
          m_priorities_to_remove{to_remove} { }

    /*! Destructor */
    virtual ~SetPortPfcPriorities();

    /*!
     * Serialize command to a vector of switch commands that need to be called consequently
     * @return vector of switch commands
     */
    virtual std::vector<std::string> serialize() const override;

private:
    std::string m_interface{};
    std::vector<std::uint32_t> m_priorities_to_set;
    std::vector<std::uint32_t> m_priorities_to_remove;
};



}
}
}
