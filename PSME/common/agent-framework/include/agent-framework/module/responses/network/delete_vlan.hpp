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
 * @file responses/network/delete_vlan.hpp
 * @brief Definition of network DeleteVlan response
 * */

#pragma once
#include "agent-framework/module/model/attributes/oem.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace responses {

using Oem = agent_framework::model::attribute::Oem;

/*! DeleteVlan response */
class DeleteVlan  {
public:
    /*!
     * @brief explicit DeleteVlan response constructor
     */
    explicit DeleteVlan(Oem oem = Oem{});

    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     */
    json::Json to_json() const;

    /*!
     * @brief create DeleteVlan from Json
     *
     * @param[in] json the input argument
     *
     * @return new DeleteVlan
     */
    static DeleteVlan from_json(const json::Json& json);

    /*!
     * @brief Set OEM data for response
     *
     * @param[in] oem OEM data
     * */
    void set_oem(const Oem& oem) {
        m_oem = oem;
    }

private:
    Oem m_oem{};
};

}
}
}
