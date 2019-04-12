/*!
 * @brief Definition of class representing getTasKResultInfo command response
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
 * @file get_task_result_info.hpp
 * */

#pragma once



#include "agent-framework/module/constants/command.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <string>



/*!
 * Class representing getTaskResultInfo GAMI response
 * */
namespace agent_framework {
namespace model {
namespace responses {

class GetTaskResultInfo {
public:
    /*!
     * Get command name
     *
     * @return Command name
     * */
    static std::string get_command() {
        return literals::Command::GET_TASK_RESULT_INFO;
    }


    /*!
     * Get task result.
     *
     * @return Task result
     * */
    const json::Json& get_result() const {
        return m_task_result;
    }


    /*!
     * Set task result.
     *
     * @param[in] result Task result to set
     * */
    void set_result(const json::Json& result) {
        m_task_result = result;
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
    static GetTaskResultInfo from_json(const json::Json& json);


private:
    // As the exact form of the result is not known before runtime, the result must be
    // held as a generic json::Json object
    json::Json m_task_result{};
};

}
}
}
