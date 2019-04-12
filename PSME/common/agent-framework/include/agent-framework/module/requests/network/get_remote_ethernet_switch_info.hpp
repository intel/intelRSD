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
 * @file requests/network/get_remote_switch_info.hpp
 * @brief Network GetRemoteEthernetSwitchInfo request
 * */

#pragma once



#include "agent-framework/module/constants/network.hpp"
#include "agent-framework/module/constants/command.hpp"
#include "agent-framework/validators/procedure_validator.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace requests {

/*! Network GetRemoteEthernetSwitchInfo request */
class GetRemoteEthernetSwitchInfo {
public:
    explicit GetRemoteEthernetSwitchInfo(const std::string& switch_v);


    static std::string get_command() {
        return "getRemoteEthernetSwitchInfo";
    }


    /*!
     * @brief Get switch UUID from request
     * @return EthernetSwitch UUID
     * */
    const std::string& get_uuid() const {
        return m_switch;
    }


    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     */
    json::Json to_json() const;


    /*!
     * @brief create GetRemoteEthernetSwitchInfo from Json
     *
     * @param[in] json the input argument
     *
     * @return new GetRemoteEthernetSwitchInfo
     */
    static GetRemoteEthernetSwitchInfo from_json(const json::Json& json);


    /*!
     * @brief Return procedure schema
     *
     * @return procedure schema
     * */
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
            get_command(), jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_OBJECT,
            literals::RemoteEthernetSwitch::SWITCH, VALID_UUID,
            nullptr
        };
        return procedure;
    }


private:
    std::string m_switch{};
};

}
}
}
