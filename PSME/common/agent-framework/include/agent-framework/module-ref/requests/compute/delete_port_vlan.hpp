/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * @file requests/compute/delete_port_vlan.hpp
 * @brief compute DeletePortVlan request
 * */

#ifndef AGENT_FRAMEWORK_MODEL_REQUESTS_COMPUTE_DELETE_PORT_VLAN_HPP
#define AGENT_FRAMEWORK_MODEL_REQUESTS_COMPUTE_DELETE_PORT_VLAN_HPP

#include "agent-framework/module-ref/model/attributes/oem.hpp"
#include <string>

namespace Json {
    class Value;
}

namespace agent_framework {
namespace model {
namespace requests {

/*! Compute DeletePortVlan request */
class DeletePortVlan {
public:
    explicit DeletePortVlan(const std::string& port_vlan,
            agent_framework::model::attribute::Oem oem);

    static std::string get_command() {
        return "deletePortVlan";
    }

    /*!
     * @brief Get port vlan from request
     * @return uuid string
     * */
    const std::string& get_port_vlan() const {
        return m_port_vlan;
    }

    /*!
     * @brief Get OEM specific data
     * @return DeletePortVlan OEM specific data
     */
    const agent_framework::model::attribute::Oem& get_oem() {
        return m_oem;
    }

    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     */
    Json::Value to_json() const;

    /*!
     * @brief create DeletePortVlan from Json
     *
     * @param[in] json the input argument
     *
     * @return new DeletePortVlan
     */
    static DeletePortVlan from_json(const Json::Value& json);

private:
    std::string m_port_vlan{};
    agent_framework::model::attribute::Oem m_oem{};
};

}
}
}

#endif /* AGENT_FRAMEWORK_COMMAND_COMPUTE_DELETE_PORT_VLAN_HPP */
