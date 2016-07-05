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
 * @file utils.hpp
 * @brief collection of headers for utility functions
 * */

#ifndef AGENT_FRAMEWORK_MODULE_UTILS_COMPUTE_HASH_HPP
#define AGENT_FRAMEWORK_MODULE_UTILS_COMPUTE_HASH_HPP

#include <string>

namespace Json{
    class Value;
}

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
 * @brief Compute hash of a Json::Value object
 * @param json Json::Value object to compute hash from
 * @return String object containg calculated hash
 */
std::string compute_hash(const Json::Value& json);

}
}
}

#endif /* AGENT_FRAMEWORK_MODULE_UTILS_COMPUTE_HASH_HPP */
