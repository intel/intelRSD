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
 * @file requests/network/add_port_vlan.hpp
 * @brief network AddPortVlan request
 * */

#pragma once

#include "agent-framework/module-ref/model/attributes/model_attributes.hpp"
#include "agent-framework/module-ref/constants/network.hpp"
#include "agent-framework/module-ref/utils/optional_field.hpp"
#include "agent-framework/validators/procedure_validator.hpp"

#include <string>

namespace Json {
    class Value;
}

namespace agent_framework {
namespace model {
namespace requests {

/*! network AddPortVlan request */
class AddPortVlan {
public:
    using Oem = agent_framework::model::attribute::Oem;

    /*!
     * @brief explicit AddPortVlan request constructor
     *
     * @param[in] port Port identifier
     * @param[in] vlan_name VLAN name
     * @param[in] vlan_id VLAN Id
     * @param[in] tagged Tagged flag
     * @param[in] oem OEM specific data
     * */
    explicit AddPortVlan(const std::string& port,
                         const OptionalField<std::string>& vlan_name,
                         unsigned int vlan_id,
                         bool tagged,
                         Oem oem);

    /*!
     * @brief Get command name
     *
     * @return Command name
     * */
    static std::string get_command() {
        return "addPortVlan";
    }

    /*!
     * @brief Get port UUID
     *
     * @return UUID as std::std::string
     * */
    const std::string& get_port() const {
        return m_port;
    }

    /*!
     * @brief Get vlan id from request
     *
     * @return vlan id
     */
    unsigned int get_vlan_id() const {
        return m_vlan_id;
    }

    /*!
     * @brief Get tagged from request
     *
     * @return tagger
     */
    bool get_tagged() const {
        return m_tagged;
    }

    /*!
     * @brief Get vlan name
     *
     * @return Vlan name
     * */
    const OptionalField<std::string>& get_vlan_name() const {
        return m_vlan_name;
    }

    /*!
     * @brief Get OEM data
     *
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
    Json::Value to_json() const;

    /*!
     * @brief create AddPortVlan from Json
     *
     * @param[in] json the input argument
     *
     * @return new AddPortVlan
     */
    static AddPortVlan from_json(const Json::Value& json);

    /*!
     * @brief Return procedure schema
     *
     * @return procedure schema
     */
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
            get_command(), jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_OBJECT,
            literals::SwitchPort::PORT, jsonrpc::JSON_STRING,
            literals::PortVlan::VLAN_ID, jsonrpc::JSON_INTEGER,
            literals::PortVlan::VLAN_NAME, VALID_NULLABLE(VALID_OPTIONAL(VALID_JSON(STRING))),
            literals::PortVlan::TAGGED, jsonrpc::JSON_BOOLEAN,
            literals::PortVlan::OEM, jsonrpc::JSON_OBJECT,
            nullptr
        };
        return procedure;
    }

private:
    std::string m_port{};
    OptionalField<std::string> m_vlan_name{};
    unsigned int m_vlan_id{};
    bool m_tagged{};
    agent_framework::model::attribute::Oem m_oem{};
};

}
}
}
