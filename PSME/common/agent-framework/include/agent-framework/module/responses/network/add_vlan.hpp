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
 * @file responses/network/add_vlan.hpp
 * @brief Definition of network AddVlan response
 * */

#pragma once

#include "agent-framework/module/model/attributes/oem.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace responses {

/*! AddVlan request */
class AddVlan  {
public:
    using Oem = agent_framework::model::attribute::Oem;

    /*!
     * @brief explicit AddVlan response constructor
     * */
    explicit AddVlan();

    /*!
     * @brief Get VLAN UUID from response
     *
     * @return std::string
     * */
    const std::string& get_vlan() const {
        return m_vlan;
    }

    /*!
     * @brief set VLAN UUID
     *
     * @param uuid VLAN UUID
     * */
    void set_vlan(const std::string& uuid) {
        m_vlan = uuid;
    }

    /*!
     * @brief set OEM
     *
     * @param oem OEM
     * */
    void set_oem(const Oem& oem) {
        m_oem = oem;
    }

    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     * */
    json::Json to_json() const;

    /*!
     * @brief create AddVlan from Json
     *
     * @param[in] json the input argument
     *
     * @return new AddVlan
     * */
    static AddVlan from_json(const json::Json& json);

private:
    std::string m_vlan{};
    Oem m_oem{};
};

}
}
}
