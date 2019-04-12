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
 * @file requests/network/delete_port_static_mac.hpp
 * @brief network DeletePortStaticMac request
 * */

#pragma once

#include "agent-framework/module/model/attributes/oem.hpp"
#include "agent-framework/module/enum/network.hpp"
#include "agent-framework/module/constants/command.hpp"
#include "agent-framework/module/constants/network.hpp"
#include "agent-framework/module/utils/optional_field.hpp"
#include "agent-framework/validators/procedure_validator.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace requests {

/*! Network DeletePortStaticMac request */
class DeletePortStaticMac {
public:
    using Oem = agent_framework::model::attribute::Oem;

    /*!
     * @brief explicit DeletePortStaticMac request constructor
     *
     * @param[in] static_mac Static MAC identifier
     * @param[in] oem OEM specific data
     * */
    explicit DeletePortStaticMac(const std::string& static_mac,
                                 Oem oem = Oem{});

    /*!
     * @brief Get command name
     *
     * @return Command name
     * */
    static std::string get_command() {
        return "deletePortStaticMac";
    }

    /*!
     * @brief Get port static MAC from request
     *
     * @return uuid string
     * */
    const std::string& get_static_mac() const {
        return m_static_mac;
    }

    /*!
     * @brief Get OEM specific data
     *
     * @return DeletePortStaticMac OEM specific data
     * */
    const Oem& get_oem() const {
        return m_oem;
    }

    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     * */
    json::Json to_json() const;

    /*!
     * @brief create DeletePortStaticMac from Json
     *
     * @param[in] json the input argument
     *
     * @return new DeletePortStaticMac
     * */
    static DeletePortStaticMac from_json(const json::Json& json);

    /*!
     * @brief Return procedure schema
     *
     * @return procedure schema
     * */
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
                get_command(), jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_OBJECT,
                literals::StaticMac::STATIC_MAC, jsonrpc::JSON_STRING,
                literals::StaticMac::OEM, jsonrpc::JSON_OBJECT,
                nullptr
        };
        return procedure;
    }

private:
    std::string m_static_mac{};
    Oem m_oem{};
};

}
}
}
