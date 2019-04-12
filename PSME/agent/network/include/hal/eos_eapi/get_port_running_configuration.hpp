/*!
 * @brief Get port running configuration class declaration.
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
 * @file get_port_running_configuration.hpp
 */

#pragma once

#include "hal/eos_eapi/eapi_command.hpp"

namespace agent {
namespace network {
namespace hal {

/*!
 * @brief Class representing eapi Port Running Configuration command
 */
class GetPortRunningConfiguration : public EapiCommand {
public:

    /*!
     * Constructor
     * @param interface ethernet interface identifier (from the switch)
     */
    GetPortRunningConfiguration(const std::string& interface)
        : EapiCommand{EAPI_COMMAND_JSON_TEXT}, m_interface{interface} { }

    /*! Destructor */
    virtual ~GetPortRunningConfiguration();

    /*!
     * Serialize command to a vector of switch commands that need to be called consequently
     * @return vector of switch commands
     */
    virtual std::vector<std::string> serialize() const override;

private:
    std::string m_interface{};
};

}
}
}