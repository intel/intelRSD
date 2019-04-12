/*!
 * @brief TaskEtry class declaration
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
 * @file task_entry.hpp
 * */

#pragma once


#include "json-wrapper/json-wrapper.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace attribute {

/*!
 * Class representing a single entry in Task array
 * */
class TaskEntry {
public:
    explicit TaskEntry();


    explicit TaskEntry(const std::string& task_uuid) : m_task_uuid(task_uuid) { }


    ~TaskEntry();


    // Enable copy and move semantics
    TaskEntry(const TaskEntry&) = default;
    TaskEntry& operator=(const TaskEntry&) = default;
    TaskEntry(TaskEntry&&) = default;
    TaskEntry& operator=(TaskEntry&&) = default;


    /*!
     * Set task UUID
     *
     * @param[in] task_uuid Task UUID
     * */
    void set_task(const std::string& task_uuid) {
        m_task_uuid = task_uuid;
    }


    /*!
     * Get task UUID
     *
     * @return Task UUID
     * */
    const std::string& get_task() const {
        return m_task_uuid;
    }


    /*!
     * Converts TaskEntry class instance to json::Json object
     *
     * @return Json object constructed from TaskEntry object
     * */
    json::Json to_json() const;


    /*!
     * Converts json::Json object to TaskEntry object
     *
     * @param[in] json json::Json object to be converted
     * */
    static TaskEntry from_json(const json::Json& json);


private:
    std::string m_task_uuid{};
};

}
}
}
