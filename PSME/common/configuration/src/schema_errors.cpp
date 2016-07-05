/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * @file schema_errors.cpp
 *
 * @brief SchemaErrors implementation
 * */

#include "configuration/schema_errors.hpp"
#include <sstream>

using namespace configuration;

SchemaErrors::Error::Error(const std::string& property,
                           const error_list_t& errors_list) :
            m_property{property},
            m_messages{errors_list} {}

void SchemaErrors::Error::add_error_message(const std::string& error_message) {
    m_messages.push_back(error_message);
}

std::size_t SchemaErrors::Error::count() const {
    return m_messages.size();
}

void SchemaErrors::Error::set_value(const std::string& value) {
    m_value = value;
}

const std::string& SchemaErrors::Error::get_value() const {
    return m_value;
}

void SchemaErrors::Error::set_path(const std::string& path) {
    m_path = path;
}

const std::string& SchemaErrors::Error::get_path() const {
    return m_path;
}

std::string SchemaErrors::Error::to_string() const {
    std::stringstream str{};
    for (const auto& error : m_messages) {
        str << " * " << error << std::endl;
    }
    return str.str();
}

const std::vector<std::string>& SchemaErrors::Error::get_messages() const {
    return m_messages;
}

void SchemaErrors::add_error(const Error& error) {
    m_errors.push_back(error);
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
