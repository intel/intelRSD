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
 * @file requests/common/get_system_info.hpp
 * @brief Compute GetSystemInfo request
 * */

#pragma once
#include "agent-framework/module/constants/compute.hpp"
#include "agent-framework/module/constants/command.hpp"
#include "agent-framework/validators/procedure_validator.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace requests {

/*! GetSystemInfo request */
class GetSystemInfo {
public:
    explicit GetSystemInfo(const std::string& system);

    static std::string get_command() {
        return literals::Command::GET_SYSTEM_INFO;
    }

    /*!
     * @brief Get system from request
     * @return uuid string
     * */
    const std::string& get_uuid() const {
        return m_system;
    }

    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     */
    json::Json to_json() const;

    /*!
     * @brief create GetSystemInfo form Json
     *
     * @param[in] json the input argument
     *
     * @return new GetSystemInfo
     */
    static GetSystemInfo from_json(const json::Json& json);

    /*!
     * @brief Returns procedure scheme
     * @return Procedure scheme
     */
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
                get_command(),
                jsonrpc::PARAMS_BY_NAME,
                jsonrpc::JSON_OBJECT,
                literals::System::SYSTEM, jsonrpc::JSON_STRING,
                nullptr
        };
        return procedure;
    }
private:
    std::string m_system{};
};

}
}
}
