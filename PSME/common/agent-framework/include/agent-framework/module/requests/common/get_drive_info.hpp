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
 * @file requesrs/common/get_drive_info.hpp
 * @brief Common GetDriveInfo request
 * */

#pragma once
#include "agent-framework/module/constants/common.hpp"
#include "agent-framework/module/constants/command.hpp"
#include "agent-framework/validators/procedure_validator.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace requests {

/*! GetDriveInfo request */
class GetDriveInfo {
public:
    explicit GetDriveInfo(const std::string& drive);

    static std::string get_command() {
        return literals::Command::GET_DRIVE_INFO;
    }

    /*!
     * @brief Get component UUID from request
     * @return UUID of component.
     * */
    const std::string& get_uuid() const {
        return m_drive;
    }

    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     */
    json::Json to_json() const;

    /*!
     * @brief create GetDriveInfo form Json
     *
     * @param[in] json the input argument
     *
     * @return new GetDriveInfo
     */
    static GetDriveInfo from_json(const json::Json& json);

    /*!
     * @brief Returns procedure scheme
     * @return Procedure scheme
     */
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
                get_command(),
                jsonrpc::PARAMS_BY_NAME,
                jsonrpc::JSON_STRING,
                literals::Drive::DRIVE, jsonrpc::JSON_STRING,
                nullptr
        };
        return procedure;
    }
private:
    std::string m_drive{};
};

}
}
}
