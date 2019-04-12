/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * @file requests/network/add_vlan.hpp
 * @brief network AddVlan request
 * */

#pragma once
#include "agent-framework/module/constants/command.hpp"
#include "agent-framework/module/constants/network.hpp"
#include "agent-framework/module/model/attributes/oem.hpp"
#include "agent-framework/validators/procedure_validator.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace requests {

/*! network AddVlan request */
class AddVlan{
public:
    explicit AddVlan(const std::string& switch_id,
            const std::string& vlan_name,
            unsigned int vlan_id,
            agent_framework::model::attribute::Oem oem);

    static std::string get_command() {
        return "addVlan";
    }

    /*!
     * @brief Get switch UUID
     * @return UUID as std::string
     * */
    const std::string& get_switch() const {
        return m_switch;
    }

    /*!
     * @brief Get vlan id from request
     * @return vlan id
     */
    unsigned int get_vlan_id() const {
        return m_vlan_id;
    }

    /*!
     * @brief Get vlan name
     * @return Vlan name
     * */
    const std::string& get_vlan_name() const {
        return m_vlan_name;
    }

    /*!
     * @brief Get OEM data
     * @return OEM data
     */
    const agent_framework::model::attribute::Oem& get_oem() const {
        return m_oem;
    }

    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     */
    json::Json to_json() const;

    /*!
     * @brief create AddVlan from Json
     *
     * @param[in] json the input argument
     *
     * @return new AddVlan
     */
    static AddVlan from_json(const json::Json& json);

    /*!
     * @brief Return procedure schema
     *
     * @return procedure schema
     * */
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
            get_command(), jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_OBJECT,
            literals::Vlan::SWITCH, jsonrpc::JSON_STRING,
            literals::Vlan::VLAN_ID, VALID_JSON(INTEGER),
            literals::Vlan::VLAN_NAME, jsonrpc::JSON_STRING,
            nullptr
        };
        return procedure;
    }
private:
    std::string m_switch{};
    std::string m_vlan_name{};
    unsigned int m_vlan_id{};
    agent_framework::model::attribute::Oem m_oem{};
};

}
}
}
