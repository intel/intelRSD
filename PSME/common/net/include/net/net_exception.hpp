//
// NetException.h
//
// $Id: //poco/1.4/Net/include/Poco/Net/NetException.h#4 $
//
// Library: Net
// Package: NetCore
// Module:  NetException
//
// Definition of the NetException class.
//
// Copyright (c) 2005-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//
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
 * @file net_exception.hpp
 *
 * @brief Declaration of NetException.
 * */

#include <stdexcept>

#pragma once

namespace net {

/*!
 * Base class for net exceptions.
 */
class NetException : public std::runtime_error {
public:
    /*!
     * Constructor.
     * @param[in] msg Error message
     * @param[in] code Error code
     */
    explicit NetException(const std::string& msg, int code = 0);

    /*! Destructor */
    ~NetException() override;

    /*! Copy constructor */
    NetException(const NetException&) = default;

    /*! Assignment operator */
    NetException& operator=(const NetException&) = default;

    /*
     * Return error code (usually errno) related to exception.
     * @return Error code of given exception.
     */
    int get_code() const {
        return m_code;
    }

private:
    int m_code{0};
};


/*!
 * Invalid socket exception.
 */
class InvalidSocketException: public NetException {
public:
    /*! Constructor */
    InvalidSocketException();

    /*! Destructor */
    ~InvalidSocketException() override;

    /*! Copy constructor */
    InvalidSocketException(const InvalidSocketException&) = default;

    /*! Assignment operator */
    InvalidSocketException& operator=(const InvalidSocketException&) = default;
};

/*!
 * Timeout exception.
 */
class TimeoutException: public NetException {
public:
    /*!
     * Constructor.
     * @param[in] msg Error message
     * @param[in] code Error code
     */
    explicit TimeoutException(const std::string& msg = "Timeout", int code = 0);

    /*! Destructor */
    ~TimeoutException() override;

    /*! Copy constructor */
    TimeoutException(const TimeoutException&) = default;

    /*! Assignment operator */
    TimeoutException& operator=(const TimeoutException&) = default;
};

}
