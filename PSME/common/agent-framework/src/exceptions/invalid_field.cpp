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
 * @file invalid_uuid.cpp
 *
 * @brief Invalid JSON field exception implementation
 * */

#include "agent-framework/exceptions/invalid_field.hpp"

using namespace agent_framework::exceptions;

InvalidField::InvalidField(const std::string& msg, const std::string& field):
    InvalidParameters{msg},
    fields{""},
    message{}
{
    append(field);
}

InvalidField::~InvalidField() {}

const std::string& InvalidField::get_message() const {
    return message;
}

const std::string& InvalidField::get_path() const {
    return fields;
}

void InvalidField::append(const std::string& field) {
    if ((!fields.empty()) && fields.substr(0, 1) != "[") {
        fields.insert(0, ".");
    }
    fields.insert(0, field);
    message = InvalidParameters::get_message() + " (" + fields + ")";
}

