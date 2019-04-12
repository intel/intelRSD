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
 * @file schema_errors.cpp
 *
 * @brief SchemaErrors implementation
 * */

#include "configuration/schema_errors.hpp"
#include <sstream>

using namespace configuration;

SchemaErrors::Error::Error(const std::string& message, const std::string& path) :
            m_message{message}, m_paths{path} { }

std::string SchemaErrors::Error::to_string() const {
    std::stringstream stream{};
    stream << m_message << " Properties list:" << std::endl;
    for (const auto& path : m_paths) {
        stream << " * " << path << std::endl;
    }
    return stream.str();
}

void SchemaErrors::add_error(const Error& error) {
    bool error_exists{false};
    for (auto& e : m_errors) {
        if (e.get_message() == error.get_message()) {
            error_exists = true;
            e.add_paths(error.get_paths());
            break;
        }
    }

    if (!error_exists) {
        m_errors.push_back(error);
    }
}

std::size_t SchemaErrors::count() const {
    return m_errors.size();
}

bool SchemaErrors::is_valid() const {
    return !m_errors.size();
}

std::string SchemaErrors::to_string() const {
    std::string output{};
    for (const auto& error : m_errors) {
        output += error.to_string();
    }
    return output;
}

const std::vector<SchemaErrors::Error>& SchemaErrors::get_errors() const {
    return m_errors;
}
