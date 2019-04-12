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
 * @file responses/network/add_ethernet_switch_port.hpp
 * @brief Definition of network AddEthernetSwitchPort response
 * */

#pragma once



#include "agent-framework/module/model/attributes/oem.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace responses {

/*! AddEthernetSwitchPort response */
class AddEthernetSwitchPort {
public:
    using Oem = agent_framework::model::attribute::Oem;


    /*!
     * @brief explicit AddEthernetSwitchPort response constructor
     */
    explicit AddEthernetSwitchPort(const std::string& port = {}, Oem oem = Oem{});

    /*!
     * @brief Get port UUID from response
     * @return port std::string
     * */
    const std::string& get_port() const {
        return m_port;
    }


    /*!
     * @brief set port UUID
     *
     * @param port std::string
     * */
    void set_port(const std::string& port) {
        m_port = port;
    }


    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     */
    json::Json to_json() const;


    /*!
     * @brief create AddEthernetSwitchPort from Json
     *
     * @param[in] json the input argument
     *
     * @return new AddEthernetSwitchPort
     */
    static AddEthernetSwitchPort from_json(const json::Json& json);


private:
    std::string m_port;
    Oem m_oem{};
};

}
}
}
