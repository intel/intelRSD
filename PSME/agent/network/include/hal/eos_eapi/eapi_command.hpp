/*!
 * @brief eAPI Command class declaration.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file eapi_command.hpp
 */

#pragma once

#include "agent-framework/module/utils/optional_field.hpp"

#include <cstdint>
#include <vector>
#include <string>


namespace agent {
namespace network {
namespace hal {

constexpr char EAPI_COMMAND_JSON[] = "json";
constexpr char EAPI_COMMAND_JSON_TEXT[] = "text";

/*!
 * @brief eAPI command abstract class
 * */
class EapiCommand {
public:

    /*!
     * Default constructor
     * @param format eAPI command format
     */
    EapiCommand(const std::string& format) : m_format(format) {}

    virtual ~EapiCommand();

    /*!
     * @brief Serialize eAPI commands.
     * @return List of eAPI commands that need to be called.
     * */
    virtual std::vector<std::string> serialize() const = 0;

    /*!
     * @brief Get command format
     * @return format in which the command needs to be called.
     */
    const std::string& get_format() const {
        return m_format;
    }

protected:
    std::string m_format{EAPI_COMMAND_JSON};
};

}
}
}

