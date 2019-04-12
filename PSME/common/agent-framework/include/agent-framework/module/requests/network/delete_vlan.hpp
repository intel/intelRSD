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
 * @file requests/network/delete_vlan.hpp
 * @brief network DeleteVlan request
 * */

#pragma once
#include "agent-framework/module/model/attributes/oem.hpp"
#include "agent-framework/module/constants/command.hpp"
#include "agent-framework/module/constants/network.hpp"
#include "agent-framework/validators/procedure_validator.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace requests {

/*! Network DeleteVlan request */
class DeleteVlan {
public:
    explicit DeleteVlan(const std::string& vlan,
            agent_framework::model::attribute::Oem oem);

    static std::string get_command() {
        return "deleteVlan";
    }

    /*!
     * @brief Get port vlan from request
     * @return uuid string
     * */
    const std::string& get_vlan() const {
        return m_vlan;
    }

    /*!
     * @brief Get OEM specific data
     * @return DeleteVlan OEM specific data
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
     * @brief create DeleteVlan from Json
     *
     * @param[in] json the input argument
     *
     * @return new DeleteVlan
     */
    static DeleteVlan from_json(const json::Json& json);

    /*!
     * @brief Return procedure schema
     *
     * @return procedure schema
     * */
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
            get_command(), jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_OBJECT,
            literals::Vlan::VLAN, jsonrpc::JSON_STRING,
            nullptr
        };
        return procedure;
    }
private:
    std::string m_vlan{};
    agent_framework::model::attribute::Oem m_oem{};
};

}
}
}
