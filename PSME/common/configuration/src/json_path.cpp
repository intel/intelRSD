/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * @file json_path.cpp
 *
 * @brief JsonPath implementation
 * */

#include "configuration/json_path.hpp"

using namespace configuration;

void JsonPath::push_key(const std::string& key) {
    m_keys.push_back(key);
}

void JsonPath::pop_key() {
    if (!m_keys.empty()) {
        m_keys.pop_back();
    }
}

std::string JsonPath::get_path() {
    std::string path{};
    for (const auto& val : m_keys) {
        if (val.empty()) {
            continue;
        }

        if (!path.empty()) {
            path += "/";
        }
        path += val;
    }
    return path;
}
