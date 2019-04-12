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
 * @file requests/network/add_port_static_mac.hpp
 * @brief network AddPortStaticMac request
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

/*! network AddPortStaticMac request */
class AddPortStaticMac {
public:
    using Oem = agent_framework::model::attribute::Oem;

    /*!
     * @brief explicit AddPortStaticMac request constructor
     *
     * @param[in] port Port identifier
     * @param[in] mac_address MAC Address
     * @param[in] vlan_id VLAN Id
     * @param[in] oem OEM specific data
     * */
    explicit AddPortStaticMac(const OptionalField<std::string>& port,
                              const OptionalField<std::string>& mac_address,
                              const OptionalField<uint32_t>& vlan_id,
                              const Oem oem);

    /*!
     * @brief Get command name
     *
     * @return Command name
     * */
    static std::string get_command() {
        return "addPortStaticMac";
    }

    /*!
     * @brief Get port UUID
     *
     * @return UUID as std::std::string
     * */
    const OptionalField<std::string>& get_port() const {
        return m_port;
    }

    /*!
     * @brief Get MAC address
     *
     * @return MAC address
     * */
    const OptionalField<std::string>& get_mac_address() const {
        return m_mac_address;
    }

    /*!
     * @brief Get vlan id from request
     *
     * @return vlan id
     * */
    const OptionalField<uint32_t>& get_vlan_id() const {
        return m_vlan_id;
    }

    /*!
     * @brief Get OEM data
     *
     * @return OEM data
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
     * @brief create AddPortStaticMac from Json
     *
     * @param[in] json the input argument
     *
     * @return new AddPortStaticMac
     * */
    static AddPortStaticMac from_json(const json::Json& json);

    /*!
     * @brief Return procedure schema
     *
     * @return procedure schema
     * */
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
                get_command(), jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_OBJECT,
                literals::StaticMac::PORT, jsonrpc::JSON_STRING,
                literals::StaticMac::ADDRESS, jsonrpc::JSON_STRING,
                literals::StaticMac::VLAN_ID, VALID_NULLABLE(VALID_OPTIONAL(VALID_JSON(INTEGER))),
                literals::StaticMac::OEM, jsonrpc::JSON_OBJECT,
                nullptr
        };
        return procedure;
    }

private:
    OptionalField<std::string> m_port{};
    OptionalField<std::string> m_mac_address{};
    OptionalField<uint32_t> m_vlan_id{};
    Oem m_oem{};
};

}
}
}
