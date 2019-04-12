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
 * */

#pragma once
#include "psme/rest/server/request.hpp"
#include "psme/rest/endpoints/utils.hpp"


namespace psme {
namespace rest {
namespace utils {

class LagUtils {
public:
    /*!
     * @brief Returns list of port members UUIDs from json with members links
     * @return List of port members UUIDs
     * */
    static std::vector<std::string> get_port_members(const json::Json& json);

    /*!
     * @brief Validates if member Port Class is Physical
     * */
    static void validate_is_physical(const agent_framework::model::enums::PortClass port_class);

    /*!
     * @brief Validates if member Port Class is Logical
     * */
    static void validate_is_logical(const agent_framework::model::enums::PortClass port_class);

    /*!
     * @brief Validates if member Port Type is Upstream
     * */
    static void validate_port_type(const agent_framework::model::enums::PortType port_type);

     /*!
     * @brief Validates if port members collection is not empty
     * */
    static void validate_collection_not_empty(
        const std::vector<std::string> port_members);

    /*!
     * @brief Validates if member is already a member of another logical port
     * */
    static void validate_is_member_of_port(const std::string& port,
        const std::string& parent);

    /*!
     * @brief Validates port members list
     * */
    static void validate_port_members(const std::vector<std::string> port_members,
        const std::string& parent = "");

};
}
}
}

