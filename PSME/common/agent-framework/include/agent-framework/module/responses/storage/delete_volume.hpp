/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * @file delete_volume.hpp
 * @brief Definition of storage DeleteVolume response
 * */

#pragma once



#include "agent-framework/module/utils/optional_field.hpp"
#include "agent-framework/module/utils/uuid.hpp"
#include "agent-framework/module/model/attributes/oem.hpp"
#include "agent-framework/module/constants/command.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <string>



namespace agent_framework {
namespace model {
namespace responses {

/*! DeleteVolume response */
class DeleteVolume {
public:

    /*!
     * @brief explicit DeleteVolume response constructor
     */
    explicit DeleteVolume(attribute::Oem oem = attribute::Oem{});


    static std::string get_command() {
        return literals::Command::DELETE_VOLUME;
    }


    /*!
     * @brief Get task UUID
     * @return task UUID
     * */
    const OptionalField<Uuid>& get_task() const {
        return m_task;
    }


    /*!
     * @brief Set task UUID
     * @param[in] task the task UUID
     * */
    void set_task(const OptionalField<Uuid>& task) {
        m_task = task;
    }


    /*!
     * @brief Transform request to JSON
     * @return Created JSON value
     */
    json::Json to_json() const;


    /*!
     * @brief Create DeleteVolume from JSON
     * @param[in] json The input argument
     * @return DeleteVolume
     */
    static DeleteVolume from_json(const json::Json& json);


private:
    OptionalField<Uuid> m_task{};
    attribute::Oem m_oem{};
};

}
}
}
