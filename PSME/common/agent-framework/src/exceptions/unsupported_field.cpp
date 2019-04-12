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
 * @file unsupported_parameter.cpp
 *
 * @brief Unsupported Parameter Error implementation
 * */

#include "agent-framework/exceptions/unsupported_field.hpp"



using namespace agent_framework::exceptions;


UnsupportedField::UnsupportedField(const std::string& msg, const std::string& field_name, const json::Json& field_value) :
    InvalidField(ErrorCode::UNSUPPORTED_FIELD, msg,
                 InvalidField::create_json_data_from_field(field_name, field_value)) {
}


UnsupportedField::~UnsupportedField() {}

