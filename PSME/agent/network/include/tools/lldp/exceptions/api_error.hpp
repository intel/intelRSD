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
 * @file exception.hpp
 *
 * @brief LLDP API exception
 * */

#pragma once

#include "err_code.hpp"
#include <stdexcept>
#include <string>

namespace agent {
namespace network {
namespace api {
namespace lldp {
namespace exception {

/*!
 * @brief LLDP API exception class
 * */
class ApiError : public std::runtime_error {
public:
    /*!
     * @brief Default constructor
     *
     * @param[in] what Errro description
     *
     * */
    explicit ApiError(const std::string& what);

    ApiError(const ApiError&) = default;
    ApiError& operator=(const ApiError&) = default;

    /*!
     * @brief Default constructor
     *
     * @param[in] code Errro code
     * @param[in] what Errro description
     *
     * */
    explicit ApiError(ErrCode code, const std::string& what);

    /*!
     * @brief Get error code
     *
     * @return Error code
     *
     * */
    ErrCode code() const { return m_code; }

    /*! Default destructor */
    virtual ~ApiError();

private:
    ErrCode m_code{ErrCode::NOT_SPECIFIED};
};

}
}
}
}
}
