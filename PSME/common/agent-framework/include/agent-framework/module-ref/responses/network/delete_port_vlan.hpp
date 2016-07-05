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
 * @file responses/network/delete_port_vlan.hpp
 * @brief Definition of network DeletePortVlan response
 * */

#pragma once

#include "agent-framework/module-ref/model/attributes/oem.hpp"

#include <string>

namespace Json {
    class Value;
}

namespace agent_framework {
namespace model {
namespace responses {

/*! DeletePortVlan request */
class DeletePortVlan  {
public:
    using Oem = agent_framework::model::attribute::Oem;

    /*!
     * @brief explicit DeletePortVlan response constructor
     *
     * @param[in] oem OEM specific data
     */
    explicit DeletePortVlan(Oem oem = Oem{});

    /*!
     * @brief Get command name
     *
     * @return Command name
     * */
    static std::string get_command() {
        return "deletePortVlan";
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
    Oem m_oem{};
};

}
}
}
