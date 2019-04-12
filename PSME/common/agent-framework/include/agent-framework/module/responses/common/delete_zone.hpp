/*!
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
 *
 * @file responses/common/delete_zone.hpp
 * @brief Definition of DeleteZone response
 * */

#pragma once

#include "agent-framework/module/model/attributes/oem.hpp"
#include "json-wrapper/json-wrapper.hpp"
#include "agent-framework/module/utils/uuid.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace responses {

/*! DeleteZone response */
class DeleteZone  {
public:
    /*!
     * @brief explicit DeleteZone response constructor
     */
    explicit DeleteZone(const attribute::Oem& oem = attribute::Oem{});

    /*!
     * @brief Transform request to Json
     * @return created Json value
     */
    json::Json to_json() const;

    /*!
     * @brief create DeleteZone from Json
     * @param[in] json the input argument
     * @return new DeleteZone
     */
    static DeleteZone from_json(const json::Json& json);


    /*!
     * @brief Set task UUID
     * @param[in] task_uuid Task UUID
     * */
    void set_task(const Uuid& task_uuid) {
        m_task = task_uuid;
    }


    /*!
     * @brief Get task UUID
     * @return Task UUID
     * */
    const Uuid& get_task() const {
        return m_task;
    }


private:
    Uuid m_task{};
    attribute::Oem m_oem{};
};

}
}
}
