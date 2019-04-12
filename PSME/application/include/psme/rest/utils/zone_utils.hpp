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


#include "json-wrapper/json-wrapper.hpp"
#include "agent-framework/module/utils/optional_field.hpp"

#include <vector>
#include <string>

namespace psme {
namespace rest {
namespace utils {

class ZoneUtils {
public:

    /*!
     * @brief Validates if links to Enpoints in a PATCH json request are correct and
     * returns a list of Endpoint UUIDs
     * @return List of Enpoints UUIDs
     * */
    static std::vector<std::string> validate_patch_links_and_get_endpoint_uuids(const json::Json& json);


    /*!
    * @brief Validates if links to Enpoints in a POST json request are correct and
    * returns a list of Endpoint UUIDs
    * @return List of Enpoints UUIDs
    * */
    static std::vector<std::string> validate_post_links_and_get_endpoint_uuids(const json::Json& json);


    /*!
     * @brief Checks if list of endpoint uuids contains an upstream endpoint
     * @return upstream endpoint in the list, or null Optional
     * */
    static OptionalField<std::string> get_upstream_endpoint(const std::vector<std::string>& endpoint_uuids);
};
}
}
}

