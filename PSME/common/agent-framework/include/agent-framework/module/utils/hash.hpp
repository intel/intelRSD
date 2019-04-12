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
 * @file utils.hpp
 * @brief collection of headers for utility functions
 * */

#pragma once



#include "json-wrapper/json-wrapper.hpp"

#include <string>



namespace agent_framework {
namespace model {
namespace utils {

/*!
 * @brief Compute hash of a string
 * @param input String to compute hash from
 * @return String object containing calculated hash
 */
std::string compute_hash(const std::string& input);


/*!
 * @brief Compute hash of a json::Json object
 * @param json json::Json object to compute hash from
 * @return String object containing calculated hash
 */
std::string compute_hash(const json::Json& json);


struct Hash {
    std::string status{};
    std::string resource_without_status{};


    /*!
     * @brief Get resource hash value
     * @tparam T Resource type
     * @param resource Resource object
     * @return Hash of the resource
     * */
    template<typename T>
    static Hash from_resource(T resource) {
        static const char STATUS_KEY[] = "status";
        Hash hash{};

        json::Json json = resource.to_json();
        if (json.count(STATUS_KEY)) {
            json::Json status = json[STATUS_KEY];
            json::Json all_but_status = json;
            all_but_status.erase(STATUS_KEY);

            hash.status = compute_hash(status);
            hash.resource_without_status = compute_hash(all_but_status);
        }
        else {
            hash.status = std::string{};
            hash.resource_without_status = compute_hash(json);
        }

        return hash;
    }
};

}
}
}
