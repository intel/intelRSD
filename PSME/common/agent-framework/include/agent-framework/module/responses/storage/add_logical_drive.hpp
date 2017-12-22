/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @file responses/storage/add_logical_drive.hpp
 * @brief Definition of storage AddLogicalDrive response
 * */

#pragma once



#include "agent-framework/module/model/attributes/oem.hpp"
#include "agent-framework/module/constants/command.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace responses {

/*! AddLogicalDrive response */
class AddLogicalDrive {
public:
    using Oem = agent_framework::model::attribute::Oem;


    /*!
     * @brief explicit AddLogicalDrive response constructor
     */
    explicit AddLogicalDrive(const std::string& drive = std::string{}, const std::string& task = std::string{},
                             Oem oem = Oem{});


    static std::string get_command() {
        return literals::Command::ADD_LOGICAL_DRIVE;
    }


    /*!
     * @brief Get drive UUID from response
     * @return drive std::string
     * */
    const std::string& get_drive() const {
        return m_drive;
    }


    /*!
     * @brief Set drive UUID in the response
     * @param[in] drive Drive UUID
     * */
    void set_drive(const std::string& drive) {
        m_drive = drive;
    }


    /*!
     * Set task UUID
     *
     * @param[in] task Task UUID
     * */
    void set_task(const std::string& task) {
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
     * @brief Transform request to Json
     *
     * @return created Json value
     */
    json::Json to_json() const;


    /*!
     * @brief create AddLogicalDrive from Json
     *
     * @param[in] json the input argument
     *
     * @return new AddLogicalDrive
     */
    static AddLogicalDrive from_json(const json::Json& json);


private:
    std::string m_drive;
    std::string m_task{};
    Oem m_oem{};
};

}
}
}
