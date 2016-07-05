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
 * @file requests/network/add_ethernet_switch_port.hpp
 * @brief network AddEthernetSwitchPort request
 * */

#pragma once

#include "agent-framework/module-ref/model/attributes/oem.hpp"
#include "agent-framework/module-ref/enum/network.hpp"

#include <string>
#include <vector>

namespace Json {
    class Value;
}

namespace agent_framework {
namespace model {
namespace requests {

/*! network AddEthernetSwitchPort request */
class AddEthernetSwitchPort {
public:
    /*!
     * @brief explicit AddEthernetSwitchPort request constructor
     *
     * @param[in] switch_v Switch identifier
     * @param[in] port_identifier Port identifier
     * @param[in] mode Port mode
     * @param[in] members Port members
     * @param[in] oem OEM specific data
     * */
    explicit AddEthernetSwitchPort(const std::string& switch_v,
                                   const std::string& port_identifier,
                                   const agent_framework::model::enums::PortMode mode,
                                   std::vector<std::string> members,
                                   agent_framework::model::attribute::Oem oem);

    /*!
     * @brief Get command name
     *
     * @return Command name
     * */
    static std::string get_command() {
        return "addEthernetSwitchPort";
    }

    /*!
     * @brief Get switch UUID from request
     *
     * @return UUID std::string
     * */
    const std::string& get_switch() const {
        return m_switch;
    }

    /*!
     * @brief Get port identifier from request
     *
     * @return port identifier std::string
     * */
    const std::string& get_port_identifier() const {
        return m_port_identifier;
    }

    /*!
     * @brief Get mode from request
     *
     * @return mode std::string
     * */
    const agent_framework::model::enums::PortMode get_mode() const {
        return m_mode;
    }

    /*!
     * @brief The UUIDs of the members.
     *
     * @return Reference to vector of member ports.
     * */
    const std::vector<std::string>& get_members() const {
        return m_members;
    }

    /*!
     * @brief Get OEM specific data
     *
     * @return AddEthernetSwitchPort OEM specific data
     */
    const agent_framework::model::attribute::Oem& get_oem() {
        return m_oem;
    }

    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     */
    Json::Value to_json() const;

    /*!
     * @brief create AddEthernetSwitchPort from Json
     *
     * @param[in] json the input argument
     *
     * @return new AddEthernetSwitchPort
     */
    static AddEthernetSwitchPort from_json(const Json::Value& json);

private:
    std::string m_switch{};
    std::string m_port_identifier{};
    agent_framework::model::enums::PortMode m_mode{
        agent_framework::model::enums::PortMode::Unknown};
    std::vector<std::string> m_members{};
    agent_framework::model::attribute::Oem m_oem{};
};

}
}
}
