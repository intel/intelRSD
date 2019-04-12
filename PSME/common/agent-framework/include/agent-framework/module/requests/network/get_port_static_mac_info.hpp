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
 * @file requests/network/get_port_static_mac_info.hpp
 * @brief Network GetPortStaticMacInfo request
 * */

#pragma once

#include "agent-framework/module/constants/network.hpp"
#include "agent-framework/module/constants/command.hpp"
#include "agent-framework/validators/procedure_validator.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace requests {

/*! Generic network command GetPortStaticMacInfo */
class GetPortStaticMacInfo {
public:
    /*!
     * @brief explicit GetPortStaticMacInfo request constructor
     *
     * @param[in] port_static_mac Port Static MAC identifier
     * */
    explicit GetPortStaticMacInfo(const std::string& port_static_mac);

    /*!
     * @brief Get command name
     *
     * @return Command name
     * */
    static std::string get_command() {
        return "getPortStaticMacInfo";
    }

    /*!
     * @brief Get static MAC uuid
     *
     * @return static MAC uuid
     * */
    const std::string& get_uuid() const {
        return m_static_mac;
    }

    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     * */
    json::Json to_json() const;

    /*!
     * @brief create GetPortStaticMacInfo from Json
     *
     * @param[in] json the input argument
     *
     * @return new GetPortStaticMacInfo
     * */
    static GetPortStaticMacInfo from_json(const json::Json& json);

    /*!
     * @brief Return procedure schema
     *
     * @return procedure schema
     * */
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
                get_command(), jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_OBJECT,
                literals::StaticMac::STATIC_MAC, jsonrpc::JSON_STRING,
                nullptr
        };
        return procedure;
    }

private:
    std::string m_static_mac{};
};

}
}
}
