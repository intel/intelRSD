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
 * @file responses/common/add_zone.hpp
 * @brief Definition of common AddZone response
 * */

#pragma once



#include "agent-framework/module/utils/uuid.hpp"
#include "agent-framework/module/model/attributes/oem.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace responses {

/*! AddZone response */
class AddZone  {
public:
    /*!
     * @brief explicit AddZone response constructor
     */
    explicit AddZone(const Uuid& zone = Uuid{},
                     const Uuid& task = Uuid{},
                     const attribute::Oem& oem = attribute::Oem{});

    /*!
     * @brief Get PCIe zone from response
     * @return zone std::string
     * */
    const Uuid& get_zone() const {
        return m_zone;
    }

    /*!
     * @brief Set response PCIe zone
     * @param[in] zone std::string created zone uuid
     * */
    void set_zone(const Uuid& zone) {
        m_zone = zone;
    }

    /*!
     * @brief Set task UUID
     * @param[in] task Task UUID
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

    /*!
     * @brief Transform response to Json
     * @return created Json value
     */
    json::Json to_json() const;

    /*!
     * @brief create AddZone from Json
     * @param[in] json the input argument
     * @return new AddZone
     */
    static AddZone from_json(const json::Json& json);

private:
    Uuid m_zone{};
    Uuid m_task{};
    attribute::Oem m_oem{};
};

}
}
}
