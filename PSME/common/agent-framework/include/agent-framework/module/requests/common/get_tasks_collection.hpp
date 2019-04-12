/*!
 * @brief GetTasksCollection class declaration
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
 * @file get_tasks_collection.hpp
 * */

#pragma once



#include "agent-framework/module/constants/command.hpp"
#include "agent-framework/validators/procedure_validator.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace requests {

class GetTasksCollection {
public:
    explicit GetTasksCollection();


    /*!
     * Get command name
     *
     * @return Command name
     * */
    static std::string get_command() {
        return model::literals::Command::GET_TASKS_COLLECTION;
    }


    /*!
     * Convert request object to Json:Value
     *
     * @return Json object converted from request object
     * */
    json::Json to_json() const;


    /*!
     * Create request object from Json
     *
     * @param[in] json Json object to construct request from
     * */
    static GetTasksCollection from_json(const json::Json& json);


    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
            get_command(),
            jsonrpc::PARAMS_BY_NAME,
            jsonrpc::JSON_STRING,
            nullptr
        };
        return procedure;
    }
};

}
}
}
