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
 * @file type.cpp
 *
 * @brief TypeValidator implementation
 * */

#include "configuration/validators/type.hpp"
#include "json-wrapper/json-wrapper.hpp"
#include <map>

using namespace configuration;

namespace {

static std::map<std::string, std::string> g_types = {
    {"string", "string"},
    {"int", "integer"},
    {"uint", "unsigned integer"},
    {"bool", "boolean"},
    {"array", "array"},
    {"null", "null"},
    {"object", "object"},
    {"double", "double precision number"},
    {"number", "numeric"}
};

}

bool TypeValidator::is_valid(const json::Json& value) const {
    if ("string" == m_type) {
        return value.is_string();
    }

    if ("int" == m_type) {
        return value.is_number_integer();
    }

    if ("uint" == m_type) {
        return value.is_number_unsigned();
    }

    if ("bool" == m_type) {
        return value.is_boolean();
    }

    if ("array" == m_type) {
        return value.is_array();
    }

    if ("null" == m_type) {
        return value.is_null();
    }

    if ("object" == m_type) {
        return value.is_object();
    }

    if ("double" == m_type) {
        return value.is_number_float();
    }

    if ("number" == m_type) {
        return value.is_number();
    }

    return false;
}

std::string TypeValidator::get_error() const {
    return "Field value is not valid " + ::g_types[m_type] + " type.";
}
