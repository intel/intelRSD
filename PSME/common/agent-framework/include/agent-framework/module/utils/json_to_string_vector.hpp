/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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

#pragma once
#include <vector>
#include <string>

namespace Json {
    /* Forward declaration */
    class Value;
}

namespace agent_framework {
namespace model {
namespace utils {

/*!
 * @brief convert JSon array to vector<string>
 *
 * @param[in] json the json Array
 *
 * @return vector<string>
 */
std::vector<std::string> json_to_string_vector(const Json::Value& json);

} // namespace utils
} // namespace model
} // namespace agent_framework

