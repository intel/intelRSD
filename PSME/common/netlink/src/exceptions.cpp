/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * @file exception.cpp
 *
 * @brief Netlink exception implementation.
 * */

#include "netlink/exceptions.hpp"

#include <cstring>

using namespace netlink_base;

const std::string NetlinkException::MESSAGE_PREFIX = "Netlink message error: ";

NetlinkException::NetlinkException(const int error_code) :
    m_error_code(error_code),
    std::runtime_error(MESSAGE_PREFIX
                       + error_to_string(abs(error_code))
                       + " (" + std::to_string(abs(error_code)) + ")")
    {}

NetlinkException::~NetlinkException() {}

std::string NetlinkException::error_to_string(int errnum) {
    char err_buffer[ERR_MESSAGE_SIZE]{};
#if ((_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && !_GNU_SOURCE)
    strerror_r(errnum, err_buffer, sizeof(err_buffer));
    char* err_msg = err_buffer;
#else
    char* err_msg = strerror_r(errnum, err_buffer, sizeof(err_buffer));
#endif
    return std::string(err_msg);
}


NetlinkInvalidParamsException::NetlinkInvalidParamsException() :
    NetlinkException(-EINVAL)
    {}

NetlinkInvalidParamsException::~NetlinkInvalidParamsException() {}
