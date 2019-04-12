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
 * @file responses/common/set_component_attributes.hpp
 * @brief Definition of common SetComponentAttributes response
 * */

#pragma once

#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/constants/command.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace responses {


/*! SetComponentAttributes response */
class SetComponentAttributes  {
public:
    using Statuses = model::attribute::Array<model::attribute::ResultStatus>;

    static std::string get_command() {
        return literals::Command::SET_COMPONENT_ATTRIBUTES;
    }

    /*!
     * @brief Constructor for task-based response
     * @param[in] task Task uuid
     * @param[in] oem Oem data
     * */
    explicit SetComponentAttributes(const std::string& task,
            const attribute::Oem& oem)
        : m_task(task), m_oem(oem) { }

    /*!
     * @brief Constructor for statuses-based response
     * @param[in] statuses Array of SetComponentAttributes execution results
     * */
    explicit SetComponentAttributes(const Statuses& statuses = Statuses{})
        : m_statuses(statuses) { }

    /*!
     * @brief Set result of command execution
     * @param[in] status Result of execution containing error code and message
     * */
    void add_status(const model::attribute::ResultStatus& status);

    /*!
     * @brief Get array of execution results
     * @return Statuses of SetComponentAttributes execution results
     * */
    const Statuses& get_statuses() const {
        return m_statuses;
    }

    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     */
    json::Json to_json() const;

    /*!
     * Set task UUID
     *
     * @param[in] task Task UUID
     * */
    void set_task(const Uuid& task) {
        m_task = task;
    }


    /*!
     * Get task UUID
     *
     * @return Task UUID
     * */
    const std::string& get_task() const {
        return m_task;
    }

    /*!
     * @brief create SetComponentAttributes from Json
     *
     * @param[in] json the input argument
     *
     * @return new SetComponentAttributes
     */
    static SetComponentAttributes from_json(const json::Json& json);

private:
    Uuid m_task{};
    attribute::Oem m_oem{};
    Statuses m_statuses{};
};

}
}
}
