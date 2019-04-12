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
 * @file unsupported_field.hpp
 *
 * @brief Declaration of Unsupported Field error interface
 * */

#pragma once



#include "agent-framework/exceptions/invalid_field.hpp"



namespace agent_framework {
namespace exceptions {

/*! @brief Unsupported Parameter exception class */
class UnsupportedField final : public InvalidField {
public:

    /*!
     * @brief Constructor
     * @param[in] msg Error message
     * @param[in] field_name Name of field
     * @param[in] field_value Value of field
     * @warning Do not include field name inside message! Use field argument instead.
     * */
    explicit UnsupportedField(const std::string& msg, const std::string& field_name, const json::Json& field_value);


    UnsupportedField(const UnsupportedField&) = default;


    UnsupportedField& operator=(const UnsupportedField&) = default;


    UnsupportedField(UnsupportedField&&) = default;


    UnsupportedField& operator=(UnsupportedField&&) = default;


    /*! @brief Destructor */
    ~UnsupportedField();
};

} // namespace exceptions
} // namespace agent_framework

