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
 * @file requests/network/add_switch_port_members.hpp
 * @brief Network request AddEthernetSwitchPortMembers
 * */

#pragma once

#include "agent-framework/module-ref/model/attributes/oem.hpp"

#include <string>
#include <vector>

namespace Json {
    class Value;
}

namespace agent_framework {
namespace model {
namespace requests {

/*! Network AddEthernetSwitchPortMembers request */
class AddEthernetSwitchPortMembers {
public:
    /*!
     * @brief explicit AddEthernetSwitchPortMembers request constructor
     *
     * @param[in] members Port members
     * @param[in] port Port identifier
     * @param[in] oem OEM specific data
     * */
    explicit AddEthernetSwitchPortMembers(std::vector<std::string> members,
                                          const std::string& port,
                                          agent_framework::model::attribute::Oem oem);

    /*!
     * @brief Get command name
     *
     * @return Command name
     * */
    static std::string get_command() {
        return "addEthernetSwitchPortMembers";
    }

     /*!
     * @brief Get port UUID from request
      *
     * @return UUID string
     * */
    const std::string& get_port() const {
        return m_port;
    }

    /*!
     * @brief Get OEM specific data
     *
     * @return AddEthernetSwitchPortMembers OEM specific data
     * */
    const agent_framework::model::attribute::Oem& get_oem() {
        return m_oem;
    }

    /*!
     * @brief The UUIDs of the members (physical switch ports).
     *
     * @return Reference to vector of member ports.
     * */
    const std::vector<std::string>& get_members() const {
        return m_members;
    }

    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     */
    Json::Value to_json() const;

    /*!
     * @brief create AddEthernetSwitchPortMembers from Json
     *
     * @param[in] json the input argument
     *
     * @return new AddEthernetSwitchPortMembers
     */
    static AddEthernetSwitchPortMembers from_json(const Json::Value& json);

private:
    std::vector<std::string> m_members{};
    std::string m_port{};
    agent_framework::model::attribute::Oem m_oem{};
};

}
}
}
