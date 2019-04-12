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
 * @file requests/compute/get_trusted_module_info.hpp
 * @brief Compute GetTrustedModuleInfo request
 * */

#pragma once



#include "agent-framework/module/constants/compute.hpp"
#include "agent-framework/module/constants/command.hpp"
#include "agent-framework/validators/procedure_validator.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace requests {

/*! GetTrustedModuleInfo request */
class GetTrustedModuleInfo {
public:
    explicit GetTrustedModuleInfo(const std::string& trusted_module);


    static std::string get_command() {
        return literals::Command::GET_TRUSTED_MODULE_INFO;
    }


    /*!
     * @brief Get requested trusted module
     * @return Trusted module UUID
     * */
    std::string get_uuid() const {
        return m_trusted_module;
    }


    /*!
     * @brief Transform request to Json
     * @return created Json value
     */
    json::Json to_json() const;


    /*!
     * @brief create GetTrustedModuleInfo from Json
     *
     * @param[in] json the input argument
     *
     * @return new GetTrustedModuleInfo
     */
    static GetTrustedModuleInfo from_json(const json::Json& json);


    /*!
     * @brief Returns procedure scheme
     * @return Procedure scheme
     */
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
            get_command(),
            jsonrpc::PARAMS_BY_NAME,
            jsonrpc::JSON_OBJECT,
            literals::TrustedModule::TRUSTED_MODULE, jsonrpc::JSON_STRING,
            nullptr
        };
        return procedure;
    }


private:
    std::string m_trusted_module{};
};

}
}
}
