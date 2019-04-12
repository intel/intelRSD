//
// Socket.h
//
// $Id: //poco/1.4/Net/include/Poco/Net/Socket.h#2 $
//
// Library: Net
// Package: Sockets
// Module:  Socket
//
// Definition of the Socket class.
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
 * @file socket.hpp
 *
 * @brief Socket interface
 * */

#pragma once

#include "socket_defs.hpp"

#include <memory>
#include <vector>
#include <chrono>


namespace net {

class SocketImpl;
class SocketAddress;
class IpAddress;

using Duration = std::chrono::steady_clock::duration;

/*!
 * @brief Common base class for different socket types.
 */
class Socket {
public:
    /*! @brief I/O operation mode */
    enum SelectMode {
        SELECT_READ = 1,
        SELECT_WRITE = 2,
        SELECT_ERROR = 4
    };

    /*! List of Sockets */
    using List = std::vector<Socket>;

    /*!
     * @brief Constructor
     */
    Socket();

    /*!
     * @brief Copy constructor.
     */
    Socket(const Socket& socket) = default;

    /*!
     * @brief Assignment operator
     */
    Socket& operator=(const Socket& socket) = default;

    /*!
     * @brief Destructor
     */
    virtual ~Socket();

    /*!
     * @brief Equal operator
     * Compares SocketImpl pointers.
     */
    bool operator==(const Socket& socket) const;

    /*!
     * @brief Inequality operator
     * Compares SocketImpl pointers.
     */
    bool operator!=(const Socket& socket) const;

    /*!
     * @brief Less-than operator
     * Compares SocketImpl pointers.
     */
    bool operator<(const Socket& socket) const;

    bool operator<=(const Socket& socket) const;

    bool operator>(const Socket& socket) const;

    bool operator>=(const Socket& socket) const;

    /*!
     * @brief Closes socket.
     */
    void close();

    /*!
     * Monitors multiple Sockets, waiting until one or more of sockets become "ready"
     * for some class of I/O operation.
     * A Socket is considered ready if it is possible to perform a corresponding I/O operation
     * without blocking.
     *
     * After return, read_list contains sockets ready for reading,
     * write_list contains sockets ready for writing,
     * except_list contains sockets with a pending exception.
     *
     * If the total number of sockets passed in read_list, write_list and
     * except_list is zero, select() will return immediately and the
     * return value will be 0.
     *
     * @param[in,out] read_list list of sockets which should be
     * checked for read ready.
     * @param[in,out] write_list list of sockets which should be
     * checked for write ready.
     * @param[in,out] except_list list of sockets which should be
     * checked for a pending exception.
     * @param timeout interval that select() should block waiting
     * for a Socket to become ready.
     * @return Number of sockets ready.
     */
    static int select(List& read_list, List& write_list, List& except_list, const Duration& timeout);

    /*!
     * Determines the I/O operation readiness for this Socket.
     *
     * @param[in] timeout Interval that poll() should block waiting
     * for a Socket to become ready.
     * @param[in] mode A value indicating which I/O operations to check for readiness,
     * constructed by combining the values of the SelectMode enumeration.
     *
     * @return true if the next I/O operation corresponding to
     * mode will not block, false otherwise.
     */
    bool poll(const Duration& timeout, int mode) const;

    /*!
     * Tests if next read on Socket will not block.
     * @return Number of bytes available that can be read
     * without causing the socket to block.
     */
    int available() const;

    /*!
     * Sets the size of the send buffer for the Socket.
     * Sets SO_SNDBUF socket option.
     * @param[in] size Send buffer size.
     */
    void set_send_buffer_size(int size);

    /*!
     * Gets the size of the send buffer of the Socket.
     * Gets SO_SNDBUF socket option value.
     * The returned value may be different than the one set by set_send_buffer_size
     * as the system may adjust the value.
     * @return Size of the send buffer of the Socket.
     */
    int get_send_buffer_size() const;

    /*!
     * Sets the size of the receive buffer for the Socket.
     * Sets SO_RCVBUF socket option.
     * @param[in] size Receive buffer size.
     */
    void set_receive_buffer_size(int size);

    /*!
     * Gets the size of the receive buffer of the Socket.
     * Gets SO_RCVBUF socket option value.
     * The returned value may be different than the one set by set_receive_buffer_size
     * as the system may adjust the value.
     * @return Size of the receive buffer of the Socket.
     */
    int get_receive_buffer_size() const;

    /*!
     * Sets the send timeout value of the Socket.
     * Sets SO_SNDTIMEO socket option.
     * @param[in] timeout Send timeout value.
     */
    void set_send_timeout(const Duration& timeout);

    /*!
     * Gets the send timeout value of the Socket.
     * Gets SO_SNDTIMEO socket option value.
     * The returned value may be different than the one set by set_send_timeout
     * as the system may adjust the value.
     * @return Send timeout socket option value.
     */
    Duration get_send_timeout() const;

    /*!
     * Sets the receive timeout value of the Socket.
     * Sets SO_RCVTIMEO socket option.
     * @param[in] timeout Receive timeout value.
     */
    void set_receive_timeout(const Duration& timeout);

    /*!
     * Gets the receive timeout value of the Socket.
     * Gets SO_RCVTIMEO socket option value.
     * The returned value may be different than the one set by set_receive_timeout
     * as the system may adjust the value.
     * @return Receive timeout socket option value.
     */
    Duration get_receive_timeout() const;

    /*!
     * Sets the value of socket option specified by level and option.
     * @param[in] level the level at which the option resides.
     * @param[in] option name of the option.
     * @param[in] value Value of specified option.
     */
    void set_option(int level, int option, int value);

    /*!
     * Sets the value of socket option specified by level and option.
     * @param[in] level the level at which the option resides.
     * @param[in] option name of the option.
     * @param[in] value Value of specified option.
     */
    void set_option(int level, int option, unsigned value);

    /*!
     * Sets the value of socket option specified by level and option.
     * @param[in] level the level at which the option resides.
     * @param[in] option name of the option.
     * @param[in] value Value of specified option.
     */
    void set_option(int level, int option, unsigned char value);

    /*!
     * Sets the value of socket option specified by level and option.
     * @param[in] level the level at which the option resides.
     * @param[in] option name of the option.
     * @param[in] value Value of specified option.
     */
    void set_option(int level, int option, const Duration& value);

    /*!
     * Sets the value of socket option specified by level and option.
     * @param[in] level the level at which the option resides.
     * @param[in] option name of the option.
     * @param[in] value Value of specified option.
     */
    void set_option(int level, int option, const IpAddress& value);

    /*!
     * Gets the value of socket option specified by level and option.
     * @param[in] level the level at which the option resides.
     * @param[in] option name of the option.
     * @param[out] value Value of specified option.
     */
    void get_option(int level, int option, int& value) const;

    /*!
     * Gets the value of socket option specified by level and option.
     * @param[in] level the level at which the option resides.
     * @param[in] option name of the option.
     * @param[out] value Value of specified option.
     */
    void get_option(int level, int option, unsigned& value) const;

    /*!
     * Gets the value of socket option specified by level and option.
     * @param[in] level the level at which the option resides.
     * @param[in] option name of the option.
     * @param[out] value Value of specified option.
     */
    void get_option(int level, int option, unsigned char& value) const;

    /*!
     * Gets the value of socket option specified by level and option.
     * @param[in] level the level at which the option resides.
     * @param[in] option name of the option.
     * @param[out] value Value of specified option.
     */
    void get_option(int level, int option, Duration& value) const;

    /*!
     * Enable/disable the SO_LINGER socket option.
     * @param[in] enable true if SO_LINGER should be enabled, false otherwise.
     * @param[in] seconds value of SO_LINGER socket option.
     */
    void set_linger(bool enable, int seconds);

    /*!
     * Tests if SO_LINGER is enabled.
     * @param[out] enabled true if SO_LINGER socket option is enabled.
     * @param[out] seconds value of SO_LINGER socket option.
     */
    void get_linger(bool& enabled, int& seconds) const;

    /*!
     * Enable/disable the TCP_NODELAY socket option.
     * @param[in] flag true if TCP_NODELAY should be enabled, false otherwise.
     */
    void set_no_delay(bool flag);

    /*!
     * Tests if TCP_NODELAY is enabled.
     * @return Value of TCP_NODELAY socket option.
     */
    bool get_no_delay() const;

    /*!
     * Enable/disable the SO_KEEPALIVE socket option.
     * @param[in] flag true if SO_KEEPALIVE should be enabled, false otherwise.
     */
    void set_keep_alive(bool flag);

    /*!
     * Tests if SO_KEEPALIVE is enabled.
     * @return Value of SO_KEEPALIVE socket option.
     */
    bool get_keep_alive() const;

    /*!
     * Enable/disable the SO_REUSEADDR socket option.
     * @param[in] flag true if SO_REUSEADDR should be enabled, false otherwise.
     */
    void set_reuse_address(bool flag);

    /*!
     * Tests if SO_REUSEADDR is enabled.
     * @return Value of SO_REUSEADDR socket option.
     */
    bool get_reuse_address() const;

    /*!
     * Enable/disable the SO_REUSEPORT socket option.
     * @param[in] flag true if SO_REUSEPORT should be enabled, false otherwise.
     */
    void set_reuse_port(bool flag);

    /*!
     * Tests if SO_REUSEPORT is enabled.
     * @return Value of SO_REUSEPORT socket option.
     */
    bool get_reuse_port() const;

    /*!
     * Enable/disable the SO_OOBINLINE socket option.
     * @param[in] flag true if SO_OOBINLINE should be enabled, false otherwise.
     */
    void set_OOB_inline(bool flag);

    /*!
     * Tests if SO_OOBINLINE is enabled.
     * @return Value of SO_OOBINLINE socket option.
     */
    bool get_OOB_inline() const;

    /*!
     * Disable/Enable blocking mode of Socket.
     * @param[in] flag true if blocking mode should be enabled, false otherwise.
     */
    void set_blocking(bool flag);

    /*!
     * Returns the blocking mode of the Socket.
     * This method returns internal boolean value and will work only if blocking mode
     * is manipulated via set_blocking method.
     *
     * @return blocking mode of the socket.
     */
    bool is_blocking() const;

    /*!
     * Gets SocketAddress of this Socket.
     * @return SocketAddress of this Socket.
     */
    SocketAddress get_address() const;

    /*!
     * Gets SocketAddress of peer.
     * @return SocketAddress of the peer.
     */
    SocketAddress peer_address() const;

    /*!
     * Gets SocketImpl of this Socket.
     * @return SocketImpl of this Socket.
     */
    SocketImpl* impl() const;

    /*!
     * Tests if system supports IPv4.
     * @return true if the system supports IPv4.
     */
    static bool supports_ipv4();

    /*!
     * Tests if system supports IPv6.
     * @return true if the system supports IPv6.
     */
    static bool supports_ipv6();

    /*!
     * @brief Creates the socket of given address family.
     * @param af address family of created socket.
     */
    void init(int af);

protected:

    /*!
     * @brief Constructor
     * Creates socket from SocketImpl pointer.
     *
     * @param impl SocketImpl.
     */
    explicit Socket(std::unique_ptr<SocketImpl> impl);

    /*!
     * @return Socket's descriptor.
     */
    net_socket_t sockfd() const;

private:
    std::shared_ptr<SocketImpl> m_impl{};
};

} // namespace net
