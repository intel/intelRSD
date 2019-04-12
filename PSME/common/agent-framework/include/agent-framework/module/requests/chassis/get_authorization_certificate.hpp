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
 * @file command/chassis/get_authorization_certificate.hpp
 * @brief Generic chassis GetAuthorizationCertificate command
 * */

#pragma once
#include "agent-framework/module/constants/chassis.hpp"
#include "agent-framework/module/constants/command.hpp"
#include "agent-framework/validators/procedure_validator.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace requests {

/*! GetAuthorizationCertificate request */
class GetAuthorizationCertificate {
public:
    explicit GetAuthorizationCertificate(const std::string& certificate);

    static std::string get_command() {
        return literals::Command::GET_AUTHORIZATION_CERTIFICATE;
    }

    /*!
     * @brief Get power certificate UUID from request
     * @return uuid string
     * */
    const std::string& get_certificate() const {
        return m_certificate;
    }

    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     */
    json::Json to_json() const;

    /*!
     * @brief create GetAuthorizationCertificate form Json
     *
     * @param[in] json the input argument
     *
     * @return new GetAuthorizationCertificate
     */
    static GetAuthorizationCertificate from_json(const json::Json& json);

    /*!
     * @brief Returns procedure scheme
     * @return Procedure scheme
     */
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
            get_command(),
            jsonrpc::PARAMS_BY_NAME,
            jsonrpc::JSON_STRING,
            literals::AuthorizationCertificate::TYPE, jsonrpc::JSON_STRING,
            nullptr};
        return procedure;
    }

private:
    std::string m_certificate{};
};

}
}
}
