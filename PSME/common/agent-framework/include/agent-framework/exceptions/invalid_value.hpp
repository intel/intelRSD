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
 * @file invalid_value.hpp
 * @brief Declaration of Invalid Value class
 * */

#pragma once



#include "agent-framework/exceptions/gami_exception.hpp"



namespace agent_framework {
namespace exceptions {

/*! @brief Invalid value exception class */
class InvalidValue : public GamiException {
public:

    /*!
     * @brief Constructor
     * @param[in] msg Error message
     * */
    explicit InvalidValue(const std::string& msg) :
        GamiException(ErrorCode::INVALID_VALUE, msg) {}


    InvalidValue(const InvalidValue&) = default;


    InvalidValue& operator=(const InvalidValue&) = default;


    InvalidValue(InvalidValue&&) = default;


    InvalidValue& operator=(InvalidValue&&) = default;


    /*! @brief Destructor */
    ~InvalidValue();


protected:

    /*!
     * @brief Create exception object
     * @param[in] error_code error code
     * @param[in] msg error message
     * */
    explicit InvalidValue(const ErrorCode error_code, const std::string& msg) :
        GamiException(error_code, msg) {}
};

}
}
