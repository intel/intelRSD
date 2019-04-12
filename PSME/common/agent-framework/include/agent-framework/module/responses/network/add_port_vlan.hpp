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
 * @file responses/network/add_port_vlan.hpp
 * @brief Definition of network AddPortVlan response
 * */

#pragma once

#include "agent-framework/module/model/attributes/oem.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace responses {

/*! AddPortVlan response */
class AddPortVlan  {
public:
    using Oem = agent_framework::model::attribute::Oem;

    /*!
     * @brief explicit AddPortVlan response constructor
     *
     * @param[in] port_vlan Port VLAN identifier
     * @param[in] oem OEM specific data
     */
    explicit AddPortVlan(const std::string& port_vlan = {},
                         Oem oem = Oem{});

    /*!
     * @brief Get port vlan UUID from response
     * @return port std::string
     * */
    const std::string& get_port_vlan() const {
        return m_port_vlan;
    }

    /*!
     * @brief Set port vlan UUID for response
     *
     * @param[in] port_vlan port's UUID
     * */
    void set_port_vlan(const std::string& port_vlan) {
        m_port_vlan = port_vlan;
    }

    /*!
     * @brief Set OEM data for response
     *
     * @param[in] oem OEM data
     * */
    void set_oem(const Oem& oem) {
        m_oem = oem;
    }

    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     */
    json::Json to_json() const;

    /*!
     * @brief create AddPortVlan from Json
     *
     * @param[in] json the input argument
     *
     * @return new AddPortVlan
     */
    static AddPortVlan from_json(const json::Json& json);

private:
    std::string m_port_vlan{};
    Oem m_oem{};
};

}
}
}
