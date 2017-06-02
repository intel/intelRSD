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
 * */

#include "agent-framework/module/requests/validation/json_check_type.hpp"



namespace agent_framework {
namespace model {
namespace requests {
namespace validation {

void check_number(const Json::Value& value,
                  const std::string& parameter_name,
                  const char* logger_name) {
    if (!value.isNumeric() || value.isBool()) {
        THROW(agent_framework::exceptions::InvalidField,
              logger_name, "Attribute should be a numeric type.", parameter_name, value);
    }
}


void check_boolean(const Json::Value& value,
                   const std::string& parameter_name,
                   const char* logger_name) {
    if (!value.isBool()) {
        THROW(agent_framework::exceptions::InvalidField,
              logger_name, "Attribute should be a boolean type.", parameter_name, value);
    }
}


void check_string(const Json::Value& value,
                  const std::string& parameter_name,
                  const char* logger_name) {
    if (!value.isString()) {
        THROW(agent_framework::exceptions::InvalidField,
              logger_name, "Attribute should be a string type.", parameter_name, value);
    }
}


void check_string_no_whitespace(const Json::Value& value,
                                const std::string& parameter_name,
                                const char* logger_name) {
    check_string(value, parameter_name, logger_name);
    if(!validate_iscsi_name(value.asString())) {
        THROW(agent_framework::exceptions::InvalidValue,
              logger_name, "Value '" + value.asString() + "' should not contain whitespace characters.");
    }
}


void check_nullable_string_no_whitespace(const Json::Value& value,
                                         const std::string& parameter_name,
                                         const char* logger_name) {
    if (!value.isNull()) {
        check_string(value, parameter_name, logger_name);
        if (!validate_iscsi_name(value.asString())) {
            THROW(agent_framework::exceptions::InvalidValue,
                  logger_name, "Value '" + value.asString() + "' should not contain whitespace characters.");
        }
    }
}

bool validate_iscsi_name(const std::string name) {
    for (char c : name) {
        if (isspace(c)) {
            return false;
        }
    }
    return true;
}

}
}
}
}
