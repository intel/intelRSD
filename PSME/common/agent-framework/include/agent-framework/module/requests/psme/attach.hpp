/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * @file command/psme/attach.hpp
 * @brief Generic PSME Attach command
 * */

#pragma once

#include "agent-framework/module/constants/psme.hpp"
#include "agent-framework/module/constants/command.hpp"
#include "agent-framework/validators/procedure_validator.hpp"
#include "agent-framework/module/model/attributes/array.hpp"

namespace agent_framework {
namespace model {
namespace requests {

/*! Attach request */
class Attach {
public:
    explicit Attach();

    using Capabilities = attribute::Array<std::string>;

    /*!
     * @brief Get command name
     * @return Command name
     * */
    static std::string get_command() {
        return literals::Command::ATTACH;
    }

    /*!
     * @brief Get agent identifier from request
     * @return agent identifier
     * */
    const std::string& get_gami_id() const {
        return m_gami_id;
    }

    /*!
     * @brief Get agent version from request
     * @return agent version
     * */
    const std::string& get_version() const {
        return m_version;
    }
    /*!
     * @brief Get agent`s RPC Server IPv4 address from request
     * @return IPv4 address
     * */
    const std::string& get_ipv4_address() const {
        return m_ipv4_address;
    }
    /*!
     * @brief Get agent`s RPC Server TCP port from request
     * @return access port
     * */
    int get_port() const {
        return m_port;
    }

    /*!
     * @brief Get agent vendor from request
     * @return agent vendor
     * */
    const std::string& get_vendor() const {
        return m_vendor;
    }

    /*!
     * @brief Get agent capabilities from request
     * @return agent capabilities
     */
    const Capabilities& get_capabilities() const {
        return m_capabilities;
    }

    /*!
     * @brief Check is request is valid
     * @return On successed true is returned, otherwise false
     */
    bool is_valid() const {
        if (m_gami_id.empty() ||
            m_capabilities.size() <= 0 ||
            m_ipv4_address.empty() ||
            m_version.empty() ||
            m_vendor.empty() ||
            m_port <= 0) {
            return false;
        }
        return true;
    }

    /*!
     * @brief Transform request to Json
     * @return created Json value
     */
    json::Json to_json() const;

    /*!
     * @brief create Attach form Json
     * @param[in] json the input argument
     * @return new Attach
     */
    static Attach from_json(const json::Json& json);

    /*!
     * @brief Returns procedure scheme
     * @return Procedure scheme
     */
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
            get_command(),
            jsonrpc::PARAMS_BY_NAME,
            jsonrpc::JSON_OBJECT,
            literals::Attach::GAMI_ID, jsonrpc::JSON_STRING,
            literals::Attach::VERSION, jsonrpc::JSON_STRING,
            literals::Attach::VENDOR, jsonrpc::JSON_STRING,
            literals::Attach::IPV4_ADDRESS, jsonrpc::JSON_STRING,
            literals::Attach::PORT, jsonrpc::JSON_INTEGER,
            literals::Attach::CAPABILITIES, jsonrpc::JSON_ARRAY,
            nullptr};
        return procedure;
    }

private:
    std::string m_gami_id{};
    std::string m_version{};
    std::string m_vendor{};
    std::string m_ipv4_address{};
    int m_port{};
    Capabilities m_capabilities{};
};

}
}
}
