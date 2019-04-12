/*!
 * Copyright (c) 2015-2019 Intel Corporation
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

#include "agent-framework/module/constants/network.hpp"
#include "agent-framework/module/constants/command.hpp"
#include "agent-framework/validators/procedure_validator.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace requests {

/*! Generic network command GetVlanInfo */
class GetVlanInfo {
public:
    explicit GetVlanInfo(const std::string& vlan);

    static std::string get_command() {
        return "getVlanInfo";
    }

    /*!
     * @brief Get port vlan uuid
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
    json::Json to_json() const;

    /*!
     * @brief create GetVlanInfo from Json
     *
     * @param[in] json the input argument
     *
     * @return new GetVlanInfo
     */
    static GetVlanInfo from_json(const json::Json& json);

    /*!
     * @brief Returns procedure scheme
     * @return Procedure scheme
     */
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
            get_command(),
            jsonrpc::PARAMS_BY_NAME,
            jsonrpc::JSON_STRING,
            literals::Vlan::VLAN, jsonrpc::JSON_STRING,
            nullptr};
        return procedure;
    }

private:
    std::string m_vlan{};
};

}
}
}
