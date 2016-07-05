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
 * @file not_found.hpp
 *
 * @brief Declaration of not found exception
 * */

#ifndef AGENT_FRAMEWORK_EXCEPTIONS_NOT_FOUND_HPP
#define AGENT_FRAMEWORK_EXCEPTIONS_NOT_FOUND_HPP

#include "agent-framework/exceptions/exception_base.hpp"

namespace agent_framework {
namespace exceptions {

/*! @brief Not found exception class */
class NotFound final : public ExceptionBase {
public:
    explicit NotFound(const std::string& msg) :
             ExceptionBase{ErrorCode::NOT_FOUND, msg} {}

    NotFound(const NotFound&) = default;
    NotFound& operator=(const NotFound&) = default;

    NotFound(NotFound&&) = default;
    NotFound& operator=(NotFound&&) = default;

    /*! @brief Destructor */
    ~NotFound();
};


} // namespace exceptions
} // namespace agent_framework

#endif	/* AGENT_FRAMEWORK_EXCEPTIONS_NOT_FOUND_HPP */
