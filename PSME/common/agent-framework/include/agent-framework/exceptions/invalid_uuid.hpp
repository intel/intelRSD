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
 * @file invalid_uuid.hpp
 *
 * @brief Declaration of Invalid UUID exception
 * */

#ifndef AGENT_FRAMEWORK_EXCEPTIONS_INVALID_UUID_HPP
#define AGENT_FRAMEWORK_EXCEPTIONS_INVALID_UUID_HPP

#include "agent-framework/exceptions/exception_base.hpp"

namespace agent_framework {
namespace exceptions {

/*! @brief Invalid UUID exception class */
class InvalidUuid final : public ExceptionBase {
public:

    /*!
     * @brief Constructor
     * @param[in] msg Error message
     */
    explicit InvalidUuid(const std::string& msg) :
             ExceptionBase{ErrorCode::INVALID_UUID, msg} {}

    InvalidUuid(const InvalidUuid&) = default;
    InvalidUuid& operator=(const InvalidUuid&) = default;

    InvalidUuid(InvalidUuid&&) = default;
    InvalidUuid& operator=(InvalidUuid&&) = default;

    /*! @brief Destructor */
    ~InvalidUuid();
};

} // namespace exceptions
} // namespace agent_framework

#endif	/* AGENT_FRAMEWORK_EXCEPTIONS_INVALID_UUID_HPP */
