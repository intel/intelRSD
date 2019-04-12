/*!
 * @brief Definition of class representing deleteTask command request
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
 * @file delete_task.hpp
 * */

#pragma once



#include "agent-framework/module/constants/command.hpp"
#include "agent-framework/module/constants/common.hpp"
#include "agent-framework/validators/procedure_validator.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace requests {

class DeleteTask {
public:
    explicit DeleteTask(const std::string& task);


    static std::string get_command() {
        return literals::Command::DELETE_TASK;
    }


    const std::string& get_task() const {
        return m_task;
    }


    void set_task(const std::string& task) {
        m_task = task;
    }


    json::Json to_json() const;


    static DeleteTask from_json(const json::Json& json);


    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
            get_command(),
            jsonrpc::PARAMS_BY_NAME,
            jsonrpc::JSON_OBJECT,
            literals::Task::TASK,
            jsonrpc::JSON_STRING,
            nullptr
        };
        return procedure;
    }


private:
    std::string m_task;
};

}
}
}
