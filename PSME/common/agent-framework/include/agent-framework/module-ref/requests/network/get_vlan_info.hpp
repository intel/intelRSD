/*!
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file requests/network/get_vlan_info.hpp
 * @brief Network GetVlanInfo request
 * */

#pragma once

#include <string>

namespace Json {
    class Value;
}

namespace agent_framework {
namespace model {
namespace requests {

/*! Generic network command GetVlanInfo */
class GetVlanInfo {
public:
    /*!
     * @brief explicit GetVlanInfo request constructor
     *
     * @param[in] vlan VLAN identifier
     * */
    explicit GetVlanInfo(const std::string& vlan);

    /*!
     * @brief Get command name
     *
     * @return Command name
     * */
    static std::string get_command() {
        return "getVlanInfo";
    }

    /*!
     * @brief Get port vlan uuid
     *
     * @return Port vlan uuid
     * */
    const std::string& get_uuid() const {
        return m_vlan;
    }

    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     */
    Json::Value to_json() const;

    /*!
     * @brief create GetVlanInfo from Json
     *
     * @param[in] json the input argument
     *
     * @return new GetVlanInfo
     */
    static GetVlanInfo from_json(const Json::Value& json);

private:
    std::string m_vlan{};
};

}
}
}
