/*!
 * @brief
 *
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
 * @file get_task_info.hpp
 * */

#pragma once



#include "agent-framework/module/constants/common.hpp"
#include "agent-framework/module/constants/command.hpp"
#include "agent-framework/validators/procedure_validator.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace requests {

/*!
 * Class representing getTaskInfo GAMI request
 * */
class GetTaskInfo {
public:
    explicit GetTaskInfo(const std::string& task);


    /*!
     * Get command name
     *
     * @return Command name
     * */
    static std::string get_command() {
        return literals::Command::GET_TASK_INFO;
    }


    /*!
     * Get task UUID
     *
     * @return Task UUID
     * */
    const std::string& get_uuid() const {
        return m_task;
    }


    /*!
     * Convert request object to json::Json
     *
     * @return Converted json::Json object
     * */
    json::Json to_json() const;


    /*!
     * Construct request object from json::Json object
     *
     * @param[in] json json::Json object used for construction
     * */
    static GetTaskInfo from_json(const json::Json& json);


    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
            get_command(),
            jsonrpc::PARAMS_BY_NAME,
            jsonrpc::JSON_STRING,
            literals::Task::TASK, VALID_UUID,
            nullptr
        };
        return procedure;
    }


private:
    std::string m_task{};
};

}
}
}
