/*!
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
 *
 * @file invalid_field.cpp
 *
 * @brief Invalid JSON field exception implementation
 * */

#include "agent-framework/exceptions/invalid_field.hpp"



using namespace agent_framework::exceptions;

const constexpr char InvalidField::FIELD_NAME[];
const constexpr char InvalidField::FIELD_VALUE[];


InvalidField::InvalidField(const std::string& msg, const std::string& field_name, const json::Json& field_value) :
    GamiException(ErrorCode::INVALID_FIELD, msg, create_json_data_from_field(field_name, field_value)) {
}

InvalidField::InvalidField(const ErrorCode error_code, const std::string& msg, const json::Json& json_data) :
    GamiException(error_code, msg, json_data) { }


InvalidField::~InvalidField() {}


std::string InvalidField::get_field() const {
    const auto& json_data = get_data();
    return (json_data.count(FIELD_NAME) > 0) ? json_data[FIELD_NAME].get<std::string>() : std::string{};
}

json::Json InvalidField::create_json_data_from_field(const std::string& field_name, const json::Json& field_value) {
    json::Json json_data{};
    json_data[InvalidField::FIELD_NAME] = field_name;
    json_data[InvalidField::FIELD_VALUE] = field_value;
    return json_data;
}

std::string InvalidField::get_field_name_from_json_data(const json::Json& data, bool should_return_empty) {
    const auto& field_name = get_string_from_data(data, InvalidField::FIELD_NAME);
    if(field_name.empty() && !should_return_empty) {
        return NOT_SPECIFIED;
    }
    return field_name;
}

std::string InvalidField::get_field_value_from_json_data(const json::Json& data, bool should_return_empty) {
    const auto& field_value = get_styled_string_from_data(data, InvalidField::FIELD_VALUE);
    if(field_value.empty() && !should_return_empty) {
        return NOT_SPECIFIED;
    }
    return field_value;
}

json::Json InvalidField::get_field_value_as_json_from_json_data(const json::Json& data) {
    return get_json_from_data(data, InvalidField::FIELD_VALUE);
}
