/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file requests/network/delete_acl.hpp
 * @brief network DeleteAcl request
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

/*! network DeleteAcl request */
class DeleteAcl{
public:
    /*!
     * @brief explicit DeleteAcl request constructor
     *
     * @param[in] acl ACL identifier
     * @param[in] oem OEM specific data
     * */
    explicit DeleteAcl(const std::string& acl,
                       agent_framework::model::attribute::Oem oem = agent_framework::model::attribute::Oem{});

    /*!
     * @brief Get command name
     *
     * @return Command name
     * */
    static std::string get_command() {
        return literals::Command::DELETE_ACL;
    }

    /*!
     * @brief Get ACL UUID
     *
     * @return UUID as std::string
     * */
    const std::string& get_acl() const {
        return m_acl;
    }

    /*!
     * @brief Get OEM data
     *
     * @return OEM data
     * */
    const agent_framework::model::attribute::Oem& get_oem() const {
        return m_oem;
    }

    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     * */
    json::Json to_json() const;

    /*!
     * @brief create DeleteAcl from Json
     *
     * @param[in] json the input argument
     *
     * @return new DeleteAcl
     * */
    static DeleteAcl from_json(const json::Json& json);

    /*!
     * @brief Return procedure schema
     *
     * @return procedure schema
     * */
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
            get_command(), jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_OBJECT,
            literals::Acl::ACL, jsonrpc::JSON_STRING,
            literals::Acl::OEM, jsonrpc::JSON_OBJECT,
            nullptr
        };
        return procedure;
    }

private:
    std::string m_acl{};
    agent_framework::model::attribute::Oem m_oem{};
};

}
}
}
