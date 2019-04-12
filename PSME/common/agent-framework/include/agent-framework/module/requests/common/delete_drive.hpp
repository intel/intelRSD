/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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
 * @file delete_drive.hpp
 * @brief DeleteDrive request
 * */

#pragma once

#include "agent-framework/module/constants/command.hpp"
#include "agent-framework/module/constants/common.hpp"
#include "agent-framework/validators/procedure_validator.hpp"
#include "agent-framework/module/utils/uuid.hpp"

#include <string>


namespace agent_framework {
namespace model {
namespace requests {

/*! DeleteDrive request */
class DeleteDrive {
public:
    explicit DeleteDrive(const Uuid& drive);

   /*!
    * @brief Get command name
    * @return Command name
    * */
    static std::string get_command() {
        return literals::Command::DELETE_DRIVE;
    }

    /*!
     * @brief Get drive uuid from request
     * @return uuid string
     * */
    const Uuid& get_drive() const {
        return m_drive;
    }

    /*!
     * @brief Set drive uuid
     * @param drive Drive's uuid
     */
    void set_drive(const Uuid& drive) {
        m_drive = drive;
    }

    /*!
     * @brief Transform request to Json
     * @return created Json value
     */
    json::Json to_json() const;

    /*!
     * @brief create DeleteDrive from Json
     * @param[in] json the input argument
     * @return new DeleteDrive
     */
    static DeleteDrive from_json(const json::Json& json);

    /*!
     * @brief Returns procedure scheme
     * @return ProcedureValidator scheme
     */
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
            get_command(),
            jsonrpc::PARAMS_BY_NAME,
            jsonrpc::JSON_OBJECT,
            literals::Drive::DRIVE, VALID_UUID,
            nullptr
        };
        return procedure;
    }

private:
    Uuid m_drive{};
};

}
}
}
