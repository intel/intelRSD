//
// SocketImpl.h
//
// $Id: //poco/1.4/Net/include/Poco/Net/SocketImpl.h#4 $
//
// Library: Net
// Package: Sockets
// Module:  SocketImpl
//
// Definition of the SocketImpl class.
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
 * @file socket_impl.hpp
 *
 * @brief Socket implementation
 * */

#pragma once

#include "socket.hpp"
#include "socket_address.hpp"

#include <chrono>

namespace net {

/*!
 * Sockets API implementation class.
 *
 * Intended to be inherited to implement specific socket types like
 * StreamSocket, DatagramSocket.
 */
class SocketImpl {
public:
    using Ptr = std::unique_ptr<SocketImpl>;

    /*! Constructor */
    SocketImpl();

    /*!
     * Destructor.
     * Closes the underlying socket descriptor if opened.
     */
    virtual ~SocketImpl();

    SocketImpl(const SocketImpl&) = delete;
    SocketImpl& operator=(const SocketImpl&) = delete;

    /*!
     * Accepts a connection on a listening socket.
     * It extracts the first connection request on the queue of pending
     * connections for the listening socket, creates and returns new connected socket.
     * If the queue is empty, waits until a connection request completes.
     *
     * @param[out] client_addr Client's SocketAddress.
     * @return New TCP socket for the connection with the client.
     */
    virtual SocketImpl::Ptr accept(SocketAddress& client_addr);

    /*!
     * Initializes the socket (if not initialized) and connects
     * to the server at given address.
     * In case of DatagramSocket, no connection is established, but incoming
     * and outgoing traffic is restricted to the specified address.
     * @param[in] address Peer address we try to connect to.
     */
    virtual void connect(const SocketAddress& address);

    /*!
     * Initializes the socket (if not initialized) and connects
     * to the server at given address.
     * @param[in] address Peer address we try to connect to.
     * @param[in] timeout Interval connect is waiting to establish connection.
     */
    virtual void connect(const SocketAddress& address, const Duration& timeout);

    /*!
     * Initializes the socket (if not initialized) and connects
     * to the server at given address.
     * Before opening the connection the socket is set to nonblocking mode.
     * @param[in] address Peer address we try to connect to.
     */
    virtual void connect_non_blocking(const SocketAddress& address);

    /*!
     * Assigns the address to the socket.
     *
     * It is normally necessary to assign a local address using bind() to a
     * server socket, before it may receive connections.
     *
     * @param[in] address SocketAddress to be bound to this socket.
     * @param[in] reuse_address If true, sets the SO_REUSEADDR socket option.
     */
    virtual void bind(const SocketAddress& address, bool reuse_address = false);

#ifdef NET_HAVE_IPv6
    /*!
     * Assigns the IPv6 address to the socket.
     *
     * It is normally necessary to assign a local address using bind() to a
     * server socket, before it may receive connections.
     *
     * @param[in] address IPv6 SocketAddress to be bound to this socket.
     * @param[in] reuse_address If true, sets the SO_REUSEADDR socket option.
     * @param[in] ipv6only If true, sets IPPROTO_IPV6/IPV6_V6ONLY socket option.
     */
    virtual void bind6(const SocketAddress& address, bool reuse_address = false, bool ipv6only = false);
#endif

    /*!
     * Listens for connections on a Socket.
     *
     * @param[in] backlog the maximum length to which the queue
     * of pending connections for Socket may grow.
     */
    virtual void listen(int backlog = 64);

    /*! Closes socket */
    virtual void close();

    /*!
     * Wraps shutdown system call.
     * Further receptions on socket will be disallowed.
     */
    virtual void shutdown_receive();

    /*!
     * Wraps shutdown system call.
     * Further transmissions on socket will be disallowed.
     */
    virtual void shutdown_send();

    /*!
     * Wraps shutdown system call.
     * Further receptions and transmissions on socket will be disallowed.
     */
    virtual void shutdown();

    /*!
     * Sends the contents of the given buffer to the socket.
     *
     * @param buffer Data to be send.
     * @param length Length of data to be send.
     * @param flags Optional flags.
     * @return The number of bytes send (may be less than specified in length).
     */
    virtual long send_bytes(const void* buffer, size_t length, int flags = 0);

    /*!
     * Receives data from socket and stores it in buffer.
     * Up to length bytes are received.
     *
     * @param buffer Buffer for data.
     * @param length Length of buffer.
     * @param flags Optional flags.
     * @return The number of bytes received.
     */
    virtual long receive_bytes(void* buffer, size_t length, int flags = 0);

    /*!
     * Sends the contents of the given buffer to the given address.
     *
     * @param buffer Data to be send.
     * @param length Length of data to be send.
     * @param address Address of the target.
     * @param flags Optional flags.
     * @return The number of bytes send (may be less than specified in length).
     */
    virtual long send_to(const void* buffer, size_t length, const SocketAddress& address, int flags = 0);

    /*!
     * Receives data from socket and stores it in buffer.
     * Up to length bytes are received.
     * The address of sender is stored in address parameter.
     *
     * @param buffer Buffer for data.
     * @param length Length of buffer.
     * @param address Sender address.
     * @param flags Optional flags.
     * @return The number of bytes received.
     */
    virtual long receive_from(void* buffer, size_t length, SocketAddress& address, int flags = 0);

    /*!
     * Sends one byte of urgent data through the socket.
     * The preferred way for a socket to receive urgent data
     * is by enabling the SO_OOBINLINE option.
     *
     * @param[in] data Byte sent with the MSG_OOB flag set.
     */
    virtual void send_urgent(unsigned char data);

    /*!
     * Tests if next read on Socket will not block.
     * @return Number of bytes available that can be read
     * without causing the socket to block.
     */
    virtual int available();

    /*!
     * Determines the I/O operation readiness for this Socket.
     * Uses a select system call.
     *
     * @param[in] timeout Interval that poll() should block waiting
     * for a Socket to become ready.
     * @param[in] mode A value indicating which I/O operations to check for readiness,
     * constructed by combining the values of the SelectMode enumeration.
     *
     * @return true if the next I/O operation corresponding to
     * mode will not block, false otherwise.
     */
    virtual bool poll(const Duration& timeout, int mode);

    /*!
     * Sets the size of the send buffer for the Socket.
     * Sets SO_SNDBUF socket option.
     * @param[in] size Send buffer size.
     */
    virtual void set_send_buffer_size(int size);

    /*!
     * Gets the size of the send buffer of the Socket.
     * Gets SO_SNDBUF socket option value.
     * The returned value may be different than the one set by set_send_buffer_size
     * as the system may adjust the value.
     * @return Size of the send buffer of the Socket.
     */
    virtual int get_send_buffer_size();

    /*!
     * Sets the size of the receive buffer for the Socket.
     * Sets SO_RCVBUF socket option.
     * @param[in] size Receive buffer size.
     */
    virtual void set_receive_buffer_size(int size);

    /*!
     * Gets the size of the receive buffer of the Socket.
     * Gets SO_RCVBUF socket option value.
     * The returned value may be different than the one set by set_receive_buffer_size
     * as the system may adjust the value.
     * @return Size of the receive buffer of the Socket.
     */
    virtual int get_receive_buffer_size();

    /*!
     * Sets the send timeout value of the Socket.
     * Sets SO_SNDTIMEO socket option.
     * @param[in] timeout Send timeout value.
     */
    virtual void set_send_timeout(const Duration& timeout);

    /*!
     * Gets the send timeout value of the Socket.
     * Gets SO_SNDTIMEO socket option value.
     * The returned value may be different than the one set by set_send_timeout
     * as the system may adjust the value.
     * @return Send timeout socket option value.
     */
    virtual Duration get_send_timeout();

    /*!
     * Sets the receive timeout value of the Socket.
     * Sets SO_RCVTIMEO socket option.
     * @param[in] timeout Receive timeout value.
     */
    virtual void set_receive_timeout(const Duration& timeout);

    /*!
     * Gets the receive timeout value of the Socket.
     * Gets SO_RCVTIMEO socket option value.
     * The returned value may be different than the one set by set_receive_timeout
     * as the system may adjust the value.
     * @return Receive timeout socket option value.
     */
    virtual Duration get_receive_timeout();

    /*!
     * Gets SocketAddress of this Socket.
     * @return SocketAddress of this Socket.
     */
    virtual SocketAddress get_address();

    /*!
     * Gets SocketAddress of peer.
     * @return SocketAddress of the peer.
     */
    virtual SocketAddress get_peer_address();

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
     * Sets the value of socket option specified by level and option.
     * @param[in] level the level at which the option resides.
     * @param[in] option name of the option.
     * @param[in] value Pointer on value to be set.
     * @param[in] length Length of data pointed to by value.
     */
    virtual void set_raw_option(int level, int option, const void* value, net_socklen_t length);

    /*!
     * Gets the value of socket option specified by level and option.
     * @param[in] level the level at which the option resides.
     * @param[in] option name of the option.
     * @param[out] value Value of specified option.
     */
    void get_option(int level, int option, int& value);

    /*!
     * Gets the value of socket option specified by level and option.
     * @param[in] level the level at which the option resides.
     * @param[in] option name of the option.
     * @param[out] value Value of specified option.
     */
    void get_option(int level, int option, unsigned& value);

    /*!
     * Gets the value of socket option specified by level and option.
     * @param[in] level the level at which the option resides.
     * @param[in] option name of the option.
     * @param[out] value Value of specified option.
     */
    void get_option(int level, int option, unsigned char& value);

    /*!
     * Gets the value of socket option specified by level and option.
     * @param[in] level the level at which the option resides.
     * @param[in] option name of the option.
     * @param[out] value Value of specified option.
     */
    void get_option(int level, int option, Duration& value);

    /*!
     * Gets the value of socket option specified by level and option.
     * @param[in] level the level at which the option resides.
     * @param[in] option name of the option.
     * @param[out] value Pointer to place where option value should be stored.
     * @param[out] length Length of data pointed by value.
     */
    virtual void get_raw_option(int level, int option, void* value, net_socklen_t& length);

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
    void get_linger(bool& enabled, int& seconds);

    /*!
     * Enable/disable the TCP_NODELAY socket option.
     * @param[in] flag true if TCP_NODELAY should be enabled, false otherwise.
     */
    void set_no_delay(bool flag);

    /*!
     * Tests if TCP_NODELAY is enabled.
     * @return Value of TCP_NODELAY socket option.
     */
    bool get_no_delay();

    /*!
     * Enable/disable the SO_KEEPALIVE socket option.
     * @param[in] flag true if SO_KEEPALIVE should be enabled, false otherwise.
     */
    void set_keep_alive(bool flag);

    /*!
     * Tests if SO_KEEPALIVE is enabled.
     * @return Value of SO_KEEPALIVE socket option.
     */
    bool get_keep_alive();

    /*!
     * Enable/disable the SO_REUSEADDR socket option.
     * @param[in] flag true if SO_REUSEADDR should be enabled, false otherwise.
     */
    void set_reuse_address(bool flag);

    /*!
     * Tests if SO_REUSEADDR is enabled.
     * @return Value of SO_REUSEADDR socket option.
     */
    bool get_reuse_address();

    /*!
     * Enable/disable the SO_REUSEPORT socket option.
     * @param[in] flag true if SO_REUSEPORT should be enabled, false otherwise.
     */
    void set_reuse_port(bool flag);

    /*!
     * Tests if SO_REUSEPORT is enabled.
     * @return Value of SO_REUSEPORT socket option.
     */
    bool get_reuse_port();

    /*!
     * Enable/disable the SO_OOBINLINE socket option.
     * @param[in] flag true if SO_OOBINLINE should be enabled, false otherwise.
     */
    void set_OOB_inline(bool flag);

    /*!
     * Tests if SO_OOBINLINE is enabled.
     * @return Value of SO_OOBINLINE socket option.
     */
    bool get_OOB_inline();

    /*!
     * Enable/disable the SO_BROADCAST socket option.
     * @param[in] flag true if SO_BROADCAST should be enabled, false otherwise.
     */
    void set_broadcast(bool flag);

    /*!
     * Tests if SO_BROADCAST is enabled.
     * @return Value of SO_BROADCAST socket option.
     */
    bool get_broadcast();

    /*!
     * Disable/Enable blocking mode of Socket.
     * @param[in] flag true if blocking mode should be enabled, false otherwise.
     */
    virtual void set_blocking(bool flag);

    /*!
     * Returns the blocking mode of the Socket.
     * This method returns internal boolean value and will work only if blocking mode
     * is manipulated via set_blocking method.
     *
     * @return blocking mode of the socket.
     */
    virtual bool get_blocking() const {
        return m_blocking;
    }

    /*!
     * Gets value of SO_ERROR socket option.
     *
     * @return Value of SO_ERROR socket option.
     */
    int socket_error();

    /*!
     * Returns socket descriptor.
     *
     * @return Socket descriptor.
     */
    net_socket_t sockfd() const {
        return m_sockfd;
    }

    /*!
     * Wraps ioctl system call.
     * @param[in] request ioctl request code.
     * @param arg Reference to memory.
     */
    void ioctl(net_ioctl_request_t request, int& arg);

    /*!
     * Wraps ioctl system call.
     * @param[in] request ioctl request code.
     * @param arg Pointer to memory.
     */
    void ioctl(net_ioctl_request_t request, void* arg);


#if defined(NET_OS_FAMILY_UNIX)
    /*!
     * Wraps fcntl system call.
     * @param[in] cmd operation to be performed on socket.
     * @return fcntl return value.
     */
    int fcntl(net_fcntl_cmd_t cmd);

    /*!
     * Wraps fcntl system call.
     * @param[in] request operation to be performed on socket.
     * @param arg argument of operation.
     * @return fcntl return value.
     */
    int fcntl(net_fcntl_cmd_t request, long arg);
#endif

    /*!
     * Tests initialization state of socket.
     * @return true if the underlying socket is initialized, false otherwise.
     */
    bool initialized() const {
        return m_sockfd != NET_INVALID_SOCKET;
    }

protected:

    /*!
     * Constructor. Creates SocketImpl using the given socket descriptor.
     * @param[in] sockfd Socket descriptor.
     */
    SocketImpl(net_socket_t sockfd);

    /*!
     * Creates the underlying system native socket.
     *
     * Subclasses must implement this method so
     * that it calls init_socket() with the
     * appropriate arguments.
     *
     * Default implementation creates a stream socket.
     *
     * @param[in] af Address family of created socket.
     */
    virtual void init(int af);

    /*!
     * Creates the underlying system native socket.
     * Wraps socket system call.
     *
     * @param[in] af Address family of created socket.
     * @param[in] type Type of socket (SOCK_STREAM, SOCK_DGRAM, SOCK_RAW).
     * @param[in] protocol Protocol to be used with the socket. Normally only
     * a single protocol exists to support a particular socket type within
     * a given protocol family, in which case  protocol  can be specified as 0.
     */
    void init_socket(int af, int type, int protocol = 0);

    /*!
     * Gets last error value (errno).
     * @return Last error value.
     */
    static int last_error();

    /*! @brief Throws an exception for the last error. */
    [[noreturn]] static void throw_error();

    /*!
     * @brief Throws an exception for given error code.
     * @param err_code Error code.
     */
    [[noreturn]] static void throw_error(int err_code);

private:
    net_socket_t m_sockfd{NET_INVALID_SOCKET};
    Duration m_recv_timeout{};
    Duration m_snd_timeout{};
    bool m_blocking{true};

    friend class Socket;
};

} // namespace net
