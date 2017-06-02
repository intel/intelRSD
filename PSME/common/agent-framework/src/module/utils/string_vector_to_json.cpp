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
 * @file vector_string_to_json.cpp
 * @brief vector<string> to json converting function
 * */

#include "agent-framework/module/utils/string_vector_to_json.hpp"
#include <json/json.h>

namespace agent_framework {
namespace model {
namespace utils {

Json::Value string_vector_to_json(const std::vector<std::string>& string_vector) {
    Json::Value collection(Json::arrayValue);
    for (const auto& str : string_vector) {
        collection.append(Json::Value(str));
    }

    return collection;
}

} // namespace utils
} // namespace model
} // namespace agent_framework
