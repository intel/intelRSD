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
 * @file json_to_string_vector.cpp
 * @brief json to vector<string> converting function
 * */

#include "agent-framework/module-ref/utils/json_to_string_vector.hpp"

#include <json/json.h>

namespace agent_framework {
namespace model {
namespace utils {

std::vector<std::string>  json_to_string_vector(const Json::Value& json){
    std::vector<std::string> vec;
    for(const auto& val: json){
        vec.push_back(val.asString());
    }
    return vec;
}

}
}
}
