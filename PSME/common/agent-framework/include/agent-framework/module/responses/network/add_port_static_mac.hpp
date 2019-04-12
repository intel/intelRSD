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
 * @file responses/network/add_port_static_mac.hpp
 * @brief Definition of network AddPortStaticMac response
 * */

#pragma once

#include "agent-framework/module/model/attributes/oem.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace responses {

/*! AddPortStaticMac request */
class AddPortStaticMac  {
public:
    using Oem = agent_framework::model::attribute::Oem;

    /*!
     * @brief explicit AddPortStaticMac response constructor
     *
     * @param[in] static_mac Static MAC identifier
     *
     * @param[in] oem OEM specific data
     * */
    explicit AddPortStaticMac(const std::string& static_mac = {},
                              Oem oem = Oem{});

    /*!
     * @brief Get Static MAC UUID from response
     *
     * @return Static MAC std::string
     * */
    const std::string& get_static_mac() const {
        return m_static_mac;
    }

    /*!
     * @brief set Static MAC UUID
     *
     * @param static_mac Static MAC std::string
     * */
    void set_static_mac(const std::string& static_mac) {
        m_static_mac = static_mac;
    }

    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     * */
    json::Json to_json() const;

    /*!
     * @brief create AddPortStaticMac from Json
     *
     * @param[in] json the input argument
     *
     * @return new AddPortStaticMac
     * */
    static AddPortStaticMac from_json(const json::Json& json);

private:
    std::string m_static_mac{};
    Oem m_oem{};
};

}
}
}
