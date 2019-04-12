//
// SocketImpl.cpp
//
// $Id: //poco/1.4/Net/src/SocketImpl.cpp#6 $
//
// Library: Net
// Package: Sockets
// Module:  SocketImpl
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
 * @file socket_impl.cpp
 *
 * @brief Socket implementation
 * */

#include "net/socket_impl.hpp"
#include "net/stream_socket_impl.hpp"
#include <chrono>
#include <cstring>

namespace {
template<typename Duration>
timeval to_timeval(Duration&& d) {
    timeval tv;
    std::chrono::seconds const sec = std::chrono::duration_cast<std::chrono::seconds>(d);
    tv.tv_sec = long(sec.count());
    tv.tv_usec = long(std::chrono::duration_cast<std::chrono::microseconds>(d - sec).count());
    return tv;
}
}

namespace net {

SocketImpl::SocketImpl() { }

SocketImpl::SocketImpl(net_socket_t socketfd) : m_sockfd(socketfd) { }

SocketImpl::~SocketImpl() {
    close();
}

SocketImpl::Ptr SocketImpl::accept(SocketAddress& client_address) {
    if (NET_INVALID_SOCKET == m_sockfd) {
        throw InvalidSocketException();
    }

    SockaddrStorage address_storage;
    net_socket_t sd;
    do {
        sd = ::accept(m_sockfd, address_storage.addr, &address_storage.addr_len);
    } while (sd == NET_INVALID_SOCKET && last_error() == EINTR);
    if (sd != NET_INVALID_SOCKET) {
        client_address = SocketAddress(address_storage);
        return SocketImpl::Ptr(new StreamSocketImpl(sd));
    }
    throw_error();
}

void SocketImpl::connect(const SocketAddress& socket_address) {
    if (NET_INVALID_SOCKET == m_sockfd) {
        init(socket_address.af());
    }
    int rc;
    do {
        rc = ::connect(m_sockfd, socket_address.addr(), socket_address.length());
    } while (rc != 0 && last_error() == EINTR);
    if (rc) {
        throw_error();
    }
}

void SocketImpl::connect(const SocketAddress& socket_address, const Duration& timeout) {
    if (NET_INVALID_SOCKET == m_sockfd) {
        init(socket_address.af());
    }
    set_blocking(false);
    try {
        int rc = ::connect(m_sockfd, socket_address.addr(), socket_address.length());
        if (rc != 0) {
            int err = last_error();
            if (err != EINPROGRESS && err != EWOULDBLOCK) {
                throw_error(err);
            }
            if (!poll(timeout, Socket::SELECT_READ | Socket::SELECT_WRITE | Socket::SELECT_ERROR)) {
                throw TimeoutException("connect timed out to:" + socket_address.to_string());
            }
            err = socket_error();
            if (err) {
                throw_error(err);
            }
        }
    }
    catch (std::exception&) {
        set_blocking(true);
        throw;
    }
    set_blocking(true);
}

void SocketImpl::connect_non_blocking(const SocketAddress& socket_address) {
    if (NET_INVALID_SOCKET == m_sockfd) {
        init(socket_address.af());
    }
    set_blocking(false);
    if (::connect(m_sockfd, socket_address.addr(), socket_address.length())) {
        const auto err = last_error();
        if (EINPROGRESS != err && EWOULDBLOCK != err) {
            throw_error(err);
        }
    }
}

void SocketImpl::bind(const SocketAddress& address, bool reuse_address) {
    if (NET_INVALID_SOCKET == m_sockfd) {
        init(address.af());
    }
    if (reuse_address) {
        set_reuse_address(true);
        set_reuse_port(true);
    }

    if (::bind(m_sockfd, address.addr(), address.length())) {
        throw_error();
    }
}

#ifdef NET_HAVE_IPv6
void SocketImpl::bind6(const SocketAddress& address, bool reuse_address, bool ipv6only) {
    if (address.family() != AddressFamily::IPv6) {
        throw NetException("SocketAddress must be an IPv6 address");
    }

    if (NET_INVALID_SOCKET == m_sockfd) {
        init(address.af());
    }
#ifdef IPV6_V6ONLY
    set_option(IPPROTO_IPV6, IPV6_V6ONLY, ipv6only ? 1 : 0);
#else
    if (ipv6only) {
        throw NetException("IPV6_V6ONLY not defined.");
    }
#endif
    if (reuse_address) {
        set_reuse_address(true);
        set_reuse_port(true);
    }
    if (::bind(m_sockfd, address.addr(), address.length())) {
        throw_error();
    }
}
#endif

void SocketImpl::listen(int backlog) {
    if (NET_INVALID_SOCKET == m_sockfd) {
        throw InvalidSocketException();
    }

    if (::listen(m_sockfd, backlog)) {
        throw_error();
    }
}

void SocketImpl::close() {
    if (m_sockfd != NET_INVALID_SOCKET) {
        ::close(m_sockfd);
        m_sockfd = NET_INVALID_SOCKET;
    }
}

void SocketImpl::shutdown_receive() {
    if (NET_INVALID_SOCKET == m_sockfd) {
        throw InvalidSocketException();
    }

    if (::shutdown(m_sockfd, SHUT_RD)) {
        throw_error();
    }
}

void SocketImpl::shutdown_send() {
    if (NET_INVALID_SOCKET == m_sockfd) {
        throw InvalidSocketException();
    }

    if (::shutdown(m_sockfd, SHUT_WR)) {
        throw_error();
    }
}

void SocketImpl::shutdown() {
    if (NET_INVALID_SOCKET == m_sockfd) {
        throw InvalidSocketException();
    }

    if (::shutdown(m_sockfd, SHUT_RDWR)) {
        throw_error();
    }
}

long SocketImpl::send_bytes(const void* buffer, size_t length, int flags) {
    long rc{0};
    do {
        if (NET_INVALID_SOCKET == m_sockfd) {
            throw InvalidSocketException();
        }
        rc = ::send(m_sockfd, reinterpret_cast<const char*> (buffer), length, flags);
    } while (m_blocking && rc < 0 && last_error() == EINTR);
    if (rc < 0) {
        throw_error();
    }
    return rc;
}

long SocketImpl::receive_bytes(void* buffer, size_t length, int flags) {
    long rc{0};
    do {
        if (NET_INVALID_SOCKET == m_sockfd) {
            throw InvalidSocketException();
        }
        rc = ::recv(m_sockfd, reinterpret_cast<char*>(buffer), length, flags);
    } while (m_blocking && rc < 0 && last_error() == EINTR);
    if (rc < 0) {
        const auto err = last_error();
        if (EAGAIN == err  && !m_blocking) {
            ;
        }
        else if (EAGAIN == err || ETIMEDOUT == err) {
            throw TimeoutException("Timeout", err);
        }
        else {
            throw_error(err);
        }
    }
    return rc;
}

long SocketImpl::send_to(const void* buffer, size_t length, const SocketAddress& rAddress, int flags) {
    long rc{0};
    do {
        if (NET_INVALID_SOCKET == m_sockfd) {
            throw InvalidSocketException();
        }
        rc = ::sendto(m_sockfd, reinterpret_cast<const char*> (buffer), length, flags, rAddress.addr(), rAddress.length());
    } while (m_blocking && rc < 0 && last_error() == EINTR);
    if (rc < 0) {
        throw_error();
    }
    return rc;
}

long SocketImpl::receive_from(void* buffer, size_t length, SocketAddress& address, int flags) {
    SockaddrStorage address_storage;
    long rc{0};
    do {
        if (NET_INVALID_SOCKET == m_sockfd) {
            throw InvalidSocketException();
        }
        rc = ::recvfrom(m_sockfd, buffer, length, flags, address_storage.addr, &address_storage.addr_len);
    } while (m_blocking && rc < 0 && last_error() == EINTR);
    if (rc >= 0) {
        address = SocketAddress(address_storage);
    }
    else {
        const auto err = last_error();
        if (err == EAGAIN && !m_blocking) {
            ;
        }
        else if (EAGAIN == err || ETIMEDOUT == err) {
            throw TimeoutException("Timeout", err);
        }
        else {
            throw_error(err);
        }
    }
    return rc;
}

void SocketImpl::send_urgent(unsigned char data) {
    if (NET_INVALID_SOCKET == m_sockfd ) {
        throw InvalidSocketException();
    }

    if (::send(m_sockfd, reinterpret_cast<const char*> (&data), sizeof (data), MSG_OOB) < 0) {
        throw_error();
    }
}

int SocketImpl::available() {
    int result;
    ioctl(FIONREAD, result);
    return result;
}

bool SocketImpl::poll(const Duration& timeout, int mode) {
    net_socket_t socketfd = m_sockfd;
    if (NET_INVALID_SOCKET == socketfd) {
        throw InvalidSocketException();
    }

    fd_set read_set;
    fd_set write_set;
    fd_set except_set;
    FD_ZERO(&read_set);
    FD_ZERO(&write_set);
    FD_ZERO(&except_set);
    if (mode & Socket::SELECT_READ) {
        FD_SET(socketfd, &read_set);
    }
    if (mode & Socket::SELECT_WRITE) {
        FD_SET(socketfd, &write_set);
    }
    if (mode & Socket::SELECT_ERROR) {
        FD_SET(socketfd, &except_set);
    }
    Duration remaining_time(timeout);
    int error_code = NET_ENOERR;
    int rc;
    do {
        auto tv = to_timeval(remaining_time);
        auto start = std::chrono::steady_clock::now();
        rc = ::select(socketfd + 1, &read_set, &write_set, &except_set, &tv);
        if (rc < 0 && EINTR == (error_code = last_error())) {
            auto end = std::chrono::steady_clock::now();
            Duration elapsed = end - start;
            if (elapsed < remaining_time) {
                remaining_time -= elapsed;
            }
            else {
                remaining_time = Duration::zero();
            }
        }
    } while (rc < 0 && EINTR == error_code);
    if (rc < 0) {
        throw_error(error_code);
    }
    return rc > 0;
}

void SocketImpl::set_send_buffer_size(int size) {
    set_option(SOL_SOCKET, SO_SNDBUF, size);
}

int SocketImpl::get_send_buffer_size() {
    int result;
    get_option(SOL_SOCKET, SO_SNDBUF, result);
    return result;
}

void SocketImpl::set_receive_buffer_size(int size) {
    set_option(SOL_SOCKET, SO_RCVBUF, size);
}

int SocketImpl::get_receive_buffer_size() {
    int result;
    get_option(SOL_SOCKET, SO_RCVBUF, result);
    return result;
}

void SocketImpl::set_send_timeout(const Duration& timeout) {
    set_option(SOL_SOCKET, SO_SNDTIMEO, timeout);
}

Duration SocketImpl::get_send_timeout() {
    Duration result;
    get_option(SOL_SOCKET, SO_SNDTIMEO, result);
    return result;
}

void SocketImpl::set_receive_timeout(const Duration& timeout) {
    set_option(SOL_SOCKET, SO_RCVTIMEO, timeout);
}

Duration SocketImpl::get_receive_timeout() {
    Duration result;
    get_option(SOL_SOCKET, SO_RCVTIMEO, result);
    return result;
}

SocketAddress SocketImpl::get_address() {
    if (NET_INVALID_SOCKET == m_sockfd) {
        throw InvalidSocketException();
    }

    SockaddrStorage address_storage;
    if (::getsockname(m_sockfd, address_storage.addr, &address_storage.addr_len) < 0) {
        throw_error();
    }
    return SocketAddress(address_storage);
}

SocketAddress SocketImpl::get_peer_address() {
    if (NET_INVALID_SOCKET == m_sockfd) {
        throw InvalidSocketException();
    }

    SockaddrStorage address_storage;
    if (::getpeername(m_sockfd, address_storage.addr, &address_storage.addr_len) < 0) {
        throw_error();
    }
    return SocketAddress(address_storage);
}

void SocketImpl::set_option(int level, int option, int value) {
    set_raw_option(level, option, &value, sizeof (value));
}

void SocketImpl::set_option(int level, int option, unsigned value) {
    set_raw_option(level, option, &value, sizeof (value));
}

void SocketImpl::set_option(int level, int option, unsigned char value) {
    set_raw_option(level, option, &value, sizeof (value));
}

void SocketImpl::set_option(int level, int option, const IpAddress& value) {
    set_raw_option(level, option, value.addr(), value.length());
}

void SocketImpl::set_option(int level, int option, const Duration& value) {
    auto tv = to_timeval(value);
    set_raw_option(level, option, &tv, sizeof (tv));
}

void SocketImpl::set_raw_option(int level, int option, const void* value, net_socklen_t length) {
    if (NET_INVALID_SOCKET == m_sockfd) {
        throw InvalidSocketException();
    }

    if (-1 == ::setsockopt(m_sockfd, level, option, reinterpret_cast<const char*> (value), length)) {
        throw_error();
    }
}

void SocketImpl::get_option(int level, int option, int& value) {
    net_socklen_t len = sizeof (value);
    get_raw_option(level, option, &value, len);
}

void SocketImpl::get_option(int level, int option, unsigned& value) {
    net_socklen_t len = sizeof (value);
    get_raw_option(level, option, &value, len);
}

void SocketImpl::get_option(int level, int option, unsigned char& value) {
    net_socklen_t len = sizeof (value);
    get_raw_option(level, option, &value, len);
}

void SocketImpl::get_option(int level, int option, Duration& value) {
    struct timeval tv;
    net_socklen_t len = sizeof (tv);
    get_raw_option(level, option, &tv, len);
    value = std::chrono::seconds(tv.tv_sec) + std::chrono::microseconds(tv.tv_usec);
}

void SocketImpl::get_raw_option(int level, int option, void* value, net_socklen_t& length) {
    if (NET_INVALID_SOCKET == m_sockfd) {
        throw InvalidSocketException();
    }

    if (-1 == ::getsockopt(m_sockfd, level, option, reinterpret_cast<char*>(value), &length)) {
        throw_error();
    }
}

void SocketImpl::set_linger(bool enable, int seconds) {
    struct linger l;
    l.l_onoff = enable ? 1 : 0;
    l.l_linger = seconds;
    set_raw_option(SOL_SOCKET, SO_LINGER, &l, sizeof(l));
}

void SocketImpl::get_linger(bool& enabled, int& seconds) {
    struct linger l;
    net_socklen_t len = sizeof (l);
    get_raw_option(SOL_SOCKET, SO_LINGER, &l, len);
    enabled = l.l_onoff != 0;
    seconds = l.l_linger;
}

void SocketImpl::set_no_delay(bool flag) {
    int value = flag ? 1 : 0;
    set_option(IPPROTO_TCP, TCP_NODELAY, value);
}

bool SocketImpl::get_no_delay() {
    int value(0);
    get_option(IPPROTO_TCP, TCP_NODELAY, value);
    return value != 0;
}

void SocketImpl::set_keep_alive(bool flag) {
    int value = flag ? 1 : 0;
    set_option(SOL_SOCKET, SO_KEEPALIVE, value);
}

bool SocketImpl::get_keep_alive() {
    int value(0);
    get_option(SOL_SOCKET, SO_KEEPALIVE, value);
    return value != 0;
}

void SocketImpl::set_reuse_address(bool flag) {
    int value = flag ? 1 : 0;
    set_option(SOL_SOCKET, SO_REUSEADDR, value);
}

bool SocketImpl::get_reuse_address() {
    int value(0);
    get_option(SOL_SOCKET, SO_REUSEADDR, value);
    return value != 0;
}

#ifdef SO_REUSEPORT
void SocketImpl::set_reuse_port(bool flag) {
    try {
        int value = flag ? 1 : 0;
        set_option(SOL_SOCKET, SO_REUSEPORT, value);
    }
    catch (std::exception&) {
        // ignore error, since not all implementations
        // support SO_REUSEPORT, even if the macro
        // is defined.
    }
#else
void SocketImpl::set_reuse_port(bool) {
#endif
}

bool SocketImpl::get_reuse_port() {
#ifdef SO_REUSEPORT
    int value(0);
    get_option(SOL_SOCKET, SO_REUSEPORT, value);
    return value != 0;
#else
    return false;
#endif
}

void SocketImpl::set_OOB_inline(bool flag) {
    int value = flag ? 1 : 0;
    set_option(SOL_SOCKET, SO_OOBINLINE, value);
}

bool SocketImpl::get_OOB_inline() {
    int value(0);
    get_option(SOL_SOCKET, SO_OOBINLINE, value);
    return value != 0;
}

void SocketImpl::set_broadcast(bool flag) {
    int value = flag ? 1 : 0;
    set_option(SOL_SOCKET, SO_BROADCAST, value);
}

bool SocketImpl::get_broadcast() {
    int value(0);
    get_option(SOL_SOCKET, SO_BROADCAST, value);
    return value != 0;
}

void SocketImpl::set_blocking(bool flag) {
    const int arg = fcntl(F_GETFL);
    long flags = arg & ~O_NONBLOCK;
    if (!flag) {
        flags |= O_NONBLOCK;
    }
    (void) fcntl(F_SETFL, flags);
    m_blocking = flag;
}

int SocketImpl::socket_error() {
    int result(0);
    get_option(SOL_SOCKET, SO_ERROR, result);
    return result;
}

void SocketImpl::init(int af) {
    init_socket(af, SOCK_STREAM);
}

void SocketImpl::init_socket(int af, int type, int protocol) {
    if (NET_INVALID_SOCKET != m_sockfd) {
        throw NetException("Socket is already initialized.");
    }

    m_sockfd = ::socket(af, type, protocol);
    if (NET_INVALID_SOCKET == m_sockfd) {
        throw_error();
    }
}

void SocketImpl::ioctl(net_ioctl_request_t request, int& arg) {
    if (::ioctl(m_sockfd, request, &arg) < 0) {
        throw_error();
    }
}

void SocketImpl::ioctl(net_ioctl_request_t request, void* arg) {
    if (::ioctl(m_sockfd, request, arg) < 0) {
        throw_error();
    }
}


#if defined(NET_OS_FAMILY_UNIX)

int SocketImpl::fcntl(net_fcntl_cmd_t request) {
    const int rc = ::fcntl(m_sockfd, request);
    if (rc == -1) {
        throw_error();
    }
    return rc;
}

int SocketImpl::fcntl(net_fcntl_cmd_t request, long arg) {
    const int rc = ::fcntl(m_sockfd, request, arg);
    if (rc == -1) {
        throw_error();
    }
    return rc;
}
#endif

int SocketImpl::last_error() {
    return errno;
}

[[noreturn]] void SocketImpl::throw_error(int err) {
    throw net::NetException(::strerror(err), err);
}

[[noreturn]] void SocketImpl::throw_error() {
    throw_error(last_error());
}

} // namespace net
