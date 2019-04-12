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
 * @file nl_exception.hpp
 *
 * @brief Netlink Exception
 * */

#pragma once

#include <stdexcept>

namespace netlink_base {

/*! @brief Netlink exception class */
class NlException : public std::runtime_error {
public:
    /*! @brief Constructor */
    explicit NlException();

    /*!
     * @brief Constructor
     *
     * @param[in] msg Error message
     * */
    explicit NlException(const std::string& msg) :
        runtime_error{msg} { }

    /*! Copy constructors */
    NlException(const NlException&) = default;
    NlException(NlException&&) = default;

    /*! Assign constructors */
    NlException& operator=(const NlException&) = default;
    NlException& operator=(NlException&&) = default;

    /*! @brief Destructor */
    virtual ~NlException();
};

}
