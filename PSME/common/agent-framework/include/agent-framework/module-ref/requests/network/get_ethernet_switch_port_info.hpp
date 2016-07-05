/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 * @file requests/get_switch_port_info.hpp
 * @brief network GetEthernetSwitchPortInfo request
 * */

#pragma once

#include <string>

namespace Json {
    class Value;
}

namespace agent_framework {
namespace model {
namespace requests {

/*! Network GetEthernetSwitchPortInfo request */
class GetEthernetSwitchPortInfo{
public:
    /*!
     * @brief explicit GetEthernetSwitchPortInfo request constructor
     *
     * @param[in] port Port identifier
     * */
    explicit GetEthernetSwitchPortInfo(const std::string& port);

    /*!
     * @brief Get command name
     *
     * @return Command name
     * */
    static std::string get_command() {
        return "getEthernetSwitchPortInfo";
    }

    /*!
     * @brief Get port UUID
     *
     * @return uuid string
     * */
    const std::string& get_uuid() const {
        return m_port;
    }

    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     */
    Json::Value to_json() const;

    /*!
     * @brief create GetEthernetSwitchPortInfo from Json
     *
     * @param[in] json the input argument
     *
     * @return new GetEthernetSwitchPortInfo
     */
    static GetEthernetSwitchPortInfo from_json(const Json::Value& json);

private:
    std::string m_port{};
};

}
}
}
