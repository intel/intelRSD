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
 * @file invalid_field.hpp
 *
 * @brief Declaration of Invalid Field exception interface
 * */

#pragma once



#include "agent-framework/exceptions/gami_exception.hpp"



namespace agent_framework {
namespace exceptions {

/*! @brief Invalid field exception class */
class InvalidField : public GamiException {
public:

    /*!
     * @brief Constructor
     * @param[in] msg Error message
     * @param[in] field_name Name of field
     * @param[in] field_value Value of field
     * @warning Do not include field name inside message! Use field argument instead.
     * */
    explicit InvalidField(const std::string& msg, const std::string& field_name, const json::Json& field_value);


    InvalidField(const InvalidField&) = default;


    InvalidField& operator=(const InvalidField&) = default;


    InvalidField(InvalidField&&) = default;


    InvalidField& operator=(InvalidField&&) = default;


    /*! @brief Destructor */
    ~InvalidField();


    /*!
     * @brief Gets invalid field name (or path)
     * @return Field name or its path as std::string
     * */
    std::string get_field() const;


    /*!
     * @brief Build JSON object with field name property.
     * @param field_name Name of invalid field.
     * @param field_value Value of invalid field.
     * @return JSON object with invalid field
     * */
    static json::Json create_json_data_from_field(const std::string& field_name, const json::Json& field_value);


    /*!
     * @brief Get field name from JSON data.
     * @param data Exception's JSON data.
     * @param should_return_empty Should method return empty string or "not specified" text.
     * @return Field name as string or empty string if data does not contain field name.
     * */
    static std::string get_field_name_from_json_data(const json::Json& data, bool should_return_empty = true);


    /*!
     * @brief Get field value from JSON data.
     * @param data Exception's JSON data.
     * @param should_return_empty Should method return empty string or "not specified" text.
     * @return Field value as string representing JSON object.
     * */
    static std::string get_field_value_from_json_data(const json::Json& data, bool should_return_empty = true);


    /*!
     * @brief Get field value from JSON data as a JSON object.
     * @param data Exception's JSON data.
     * @return Field value as JSON object.
     * */
    static json::Json get_field_value_as_json_from_json_data(const json::Json& data);


protected:
    /*! @brief Exception JSON field */
    static const constexpr char FIELD_NAME[] = "fieldName";

    /*! @brief Exception JSON field value*/
    static const constexpr char FIELD_VALUE[] = "fieldValue";


    /*!
     * @brief Create exception object
     * @param[in] code error code
     * @param[in] msg error message
     * @param[in] data JSON object containing additional data
     * */
    explicit InvalidField(const ErrorCode code, const std::string& msg, const json::Json& data);
};

} // namespace exceptions
} // namespace agent_framework

