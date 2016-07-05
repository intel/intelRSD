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
 * @file string_vector_to_json.hpp
 * @brief vector<string> to json converting function
 * */

#ifndef AGENT_FRAMEWORK_MODULE_UTILS_VECTOR_STRING_TO_JSON_HPP
#define AGENT_FRAMEWORK_MODULE_UTILS_VECTOR_STRING_TO_JSON_HPP

#include <json/json.h>

#include <vector>

namespace agent_framework {
namespace model {
namespace utils {

    /*!
     * @brief convert vector<string> to JSON
     * @param[in] string_vector vector of strings to convert
     * @return Array:String
     * */
Json::Value string_vector_to_json(const std::vector<std::string>& string_vector);

} // namespace utils
} // namespace model
} // namespace agent_framework

#endif /* AGENT_FRAMEWORK_MODULE_UTILS_VECTOR_STRING_TO_JSON_HPP */
