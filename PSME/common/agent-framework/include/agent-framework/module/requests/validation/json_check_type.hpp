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
 * @file json_check_type.hpp
 * @brief Validate JSON types
 * */

#pragma once



#include "agent-framework/exceptions/exception.hpp"



namespace agent_framework {
namespace model {
namespace requests {
namespace validation {

/*!
 * @brief Check if JSON value is number type
 *
 * @param[in] value JSON value to check
 * @param[in] parameter_name Name of JSON field
 * @param[in] logger_name Logger name
 *
 * @throw agent_framework::exceptions::InvalidField
 * when value is another type
 * */
void check_number(const json::Json& value,
                  const std::string& parameter_name,
                  const char* logger_name);


/*!
 * @brief Check if JSON value is boolean type
 *
 * @param[in] value JSON value to check
 * @param[in] parameter_name Name of JSON field
 * @param[in] logger_name Logger name
 *
 * @throw agent_framework::exceptions::InvalidField
 * when value is another type
 * */
void check_boolean(const json::Json& value,
                   const std::string& parameter_name,
                   const char* logger_name);


/*!
 * @brief Check if JSON value is string type
 *
 * @param[in] value JSON value to check
 * @param[in] parameter_name Name of JSON field
 * @param[in] logger_name Logger name
 *
 * @throw agent_framework::exceptions::InvalidField
 * when value is another type
 * */
void check_string(const json::Json& value,
                  const std::string& parameter_name,
                  const char* logger_name);


/*!
 * @brief Check if JSON value is string type or null
 *
 * @param[in] value JSON value to check
 * @param[in] parameter_name Name of JSON field
 * @param[in] logger_name Logger name
 *
 * @throw agent_framework::exceptions::InvalidField when value is another type
 * */
void check_nullable_string(const json::Json& value,
                           const std::string& parameter_name,
                           const char* logger_name);


/*!
 * @brief Check if JSON value is string type and has no whitespace characters
 *
 * @param[in] value JSON value to check
 * @param[in] parameter_name Name of JSON field
 * @param[in] logger_name Logger name
 *
 * @throw agent_framework::exceptions::InvalidField
 * when value is another type
 * */
void check_string_no_whitespace(const json::Json& value,
                                const std::string& parameter_name,
                                const char* logger_name);


/*!
 * @brief Check if JSON value is null or string type and has no whitespace characters
 *
 * @param[in] value JSON value to check
 * @param[in] parameter_name Name of JSON field
 * @param[in] logger_name Logger name
 *
 * @throw agent_framework::exceptions::InvalidField
 * when value is another type
 * */
void check_nullable_string_no_whitespace(const json::Json& value,
                                         const std::string& parameter_name,
                                         const char* logger_name);


/*!
* @brief Check if string has no whitespace characters
*
* @param[in] name string value to check
* @return bool false if string has whitespace characters
* */
bool validate_iscsi_name(const std::string name);


/*!
 * @brief Check if JSON value could be converted from string to enum
 *
 * @tparam E JSON value will be converted to this type. Type has to
 * contain 'from_string' method e.g agent_framework::enums types.
 *
 * @param[in] value JSON value to check
 * @param[in] parameter_name Name of JSON field
 * @param[in] logger_name Logger name
 *
 * @throw agent_framework::exceptions::InvalidField
 * when value could not be converted from string type
 *
 * @throw agent_framework::exceptions::InvalidValue
 * when value could not be converted to enum value
 * */
template<typename E>
void check_enum(const json::Json& value,
                const std::string& parameter_name,
                const char* logger_name) {
    check_string(value, parameter_name, logger_name);
    try {
        E::from_string(value.get<std::string>());
    }
    catch (...) {
        log_error(logger_name,
                  "Invalid value for " << parameter_name << " attribute: '" << value.dump() << "'.");
        throw agent_framework::exceptions::InvalidValue("Invalid attribute value: '" + value.dump() + "'.");
    }
}


/*!
 * @brief Check if JSON value is null or could be converted from string to enum
 *
 * @tparam E JSON value will be converted to this type. Type has to
 * contain 'from_string' method e.g agent_framework::enums types.
 *
 * @param[in] value JSON value to check
 * @param[in] parameter_name Name of JSON field
 * @param[in] logger_name Logger name
 *
 * @throw agent_framework::exceptions::InvalidField
 * when value could not be converted from string type
 *
 * @throw agent_framework::exceptions::InvalidValue
 * when value could not be converted to enum value
 * */
template<typename E>
void check_nullable_enum(const json::Json& value,
                         const std::string& parameter_name,
                         const char* logger_name) {
    if (!value.is_null()) {
        check_string(value, parameter_name, logger_name);
        try {
            E::from_string(value.get<std::string>());
        }
        catch (...) {
            log_error(logger_name,
                      "Invalid value for " << parameter_name << " attribute: '" << value.dump() << "'.");
            throw agent_framework::exceptions::InvalidValue("Invalid attribute value: '" + value.dump() + "'.");
        }
    }
}

}
}
}
}
