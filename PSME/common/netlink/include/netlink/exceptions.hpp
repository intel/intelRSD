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
 * @file exception.hpp
 *
 * @brief Netlink exception class
 * */

#pragma once

#include <stdexcept>

namespace netlink_base {

/*!
 * @brief Netlink exception.
 */
class NetlinkException : public std::runtime_error {
public:
    /*!
     * @brief Explicit constructor.
     */
    explicit NetlinkException(const int error_code);

    /*!
     * @brief Default destructor.
     */
    virtual ~NetlinkException();

    NetlinkException(const NetlinkException&) = default;
    NetlinkException& operator=(const NetlinkException&) = default;
    NetlinkException(NetlinkException&&) = default;
    NetlinkException& operator=(NetlinkException&&) = default;

    /*!
     * @brief Get netlink error code.
     *
     * @return netlink error code.
     */
    int get_error_code() const {
        return m_error_code;
    }

protected:
    static constexpr uint16_t ERR_MESSAGE_SIZE = 256;
    static const std::string MESSAGE_PREFIX;

    const std::string m_message{};
    const int m_error_code{};

    /*!
     * @brief Convert error number into string.
     *
     * @param[in] errnum Error number.
     *
     * @return String describing the error number.
     */
    static std::string error_to_string(int errnum);
};

/*!
 * @brief Netlink invalid parameters exception.
 */
class NetlinkInvalidParamsException : public NetlinkException {
public:
    /*!
     * @brief Explicit constructor.
     */
    explicit NetlinkInvalidParamsException();

    /*!
     * @brief Default destructor.
     */
    virtual ~NetlinkInvalidParamsException();

    NetlinkInvalidParamsException(const NetlinkInvalidParamsException&) = default;
    NetlinkInvalidParamsException& operator=(const NetlinkInvalidParamsException&) = default;
    NetlinkInvalidParamsException(NetlinkInvalidParamsException&&) = default;
    NetlinkInvalidParamsException& operator=(NetlinkInvalidParamsException&&) = default;
};

}
