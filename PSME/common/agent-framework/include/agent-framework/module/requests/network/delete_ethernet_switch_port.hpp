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
 * @file requests/network/delete_ethernet_switch_port.hpp
 * @brief Network DeleteEthernetSwitchPort request
 * */

#pragma once

#include "agent-framework/module/constants/network.hpp"
#include "agent-framework/module/constants/command.hpp"
#include "agent-framework/module/model/attributes/oem.hpp"
#include "agent-framework/validators/procedure_validator.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace requests {

using Oem = agent_framework::model::attribute::Oem;

/*! Network DeleteEthernetSwitchPort request */
class DeleteEthernetSwitchPort {
public:
    explicit DeleteEthernetSwitchPort(const std::string& port, Oem oem = Oem{});

    static std::string get_command() {
        return "deleteEthernetSwitchPort";
    }

    /*!
     * @brief Get port UUID from request
     * @return UUID string
     * */
    const std::string& get_port() const {
        return m_port;
    }

    /*!
     * @brief Get OEM specific data
     * @return DeleteEthernetSwitchPort OEM specific data
     */
    const Oem& get_oem() const {
        return m_oem;
    }

    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     */
    json::Json to_json() const;

    /*!
     * @brief create DeleteEthernetSwitchPort from Json
     *
     * @param[in] json the input argument
     *
     * @return new DeleteEthernetSwitchPort
     */
    static DeleteEthernetSwitchPort from_json(const json::Json& json);

    /*!
     * @brief Return procedure schema
     *
     * @return procedure schema
     * */
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
                get_command(), jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_OBJECT,
                literals::EthernetSwitchPort::PORT, jsonrpc::JSON_STRING,
                literals::EthernetSwitchPort::OEM, jsonrpc::JSON_OBJECT,
                nullptr
        };
        return procedure;
    }

private:
    std::string m_port{};
    Oem m_oem{};
};

}
}
}
