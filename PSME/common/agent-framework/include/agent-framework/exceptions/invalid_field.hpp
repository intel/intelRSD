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
 * @file invalid_field.hpp
 *
 * @brief Declaration of Invalid Field exception interface
 * */

#ifndef AGENT_FRAMEWORK_EXCEPTIONS_INVALID_FIELD_HPP
#define AGENT_FRAMEWORK_EXCEPTIONS_INVALID_FIELD_HPP

#include "agent-framework/exceptions/invalid_parameters.hpp"

namespace agent_framework {
namespace exceptions {

/*! @brief Invalid field exception class */
class InvalidField: public InvalidParameters {
public:
    /*!
     * @brief Constructor
     *
     * @param[in] msg Error message
     */
    explicit InvalidField(const std::string& msg, const std::string& field);

    InvalidField(const InvalidField&) = default;
    InvalidField& operator=(const InvalidField&) = default;

    InvalidField(InvalidField&&) = default;
    InvalidField& operator=(InvalidField&&) = default;

    /*! @brief Destructor */
    virtual ~InvalidField();

    virtual const std::string& get_message() const;
    void append(const std::string& field);
    const std::string& get_path() const;

private:
    std::string fields;
    std::string message;
};

} // namespace exceptions
} // namespace agent_framework

#endif	/* AGENT_FRAMEWORK_EXCEPTIONS_INVALID_FIELD_HPP */
