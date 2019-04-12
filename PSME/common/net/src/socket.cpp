//
// Socket.cpp
//
// $Id: //poco/1.4/Net/src/Socket.cpp#3 $
//
// Library: Net
// Package: Sockets
// Module:  Socket
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
 * @file socket.cpp
 *
 * @brief Socket interface
 * */

#include "net/socket.hpp"
#include "net/socket_impl.hpp"
#include "net/stream_socket_impl.hpp"

#include <algorithm>
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

Socket::Socket() : m_impl(new StreamSocketImpl)  { }

Socket::Socket(SocketImpl::Ptr pimpl) : m_impl(std::move(pimpl)) { }

Socket::~Socket() { }

bool Socket::supports_ipv4() {
    return true;
}

bool Socket::supports_ipv6() {
#ifdef NET_HAVE_IPv6
    return true;
#else
    return false;
#endif
}

int Socket::select(List& read_list, List& write_list, List& except_list, const Duration& timeout) {
    fd_set read_set;
    fd_set write_set;
    fd_set except_set;
    int max_fd = 0;
    FD_ZERO(&read_set);
    for (const auto& read_socket: read_list) {
        const auto& fd = read_socket.sockfd();
        if (NET_INVALID_SOCKET != fd) {
            if (fd > max_fd) {
                max_fd = fd;
            }
            FD_SET(fd, &read_set);
        }
    }
    FD_ZERO(&write_set);
    for (const auto& write_socket: write_list) {
        const auto& fd = write_socket.sockfd();
        if (NET_INVALID_SOCKET != fd) {
            if (fd > max_fd) {
                max_fd = fd;
            }
            FD_SET(fd, &write_set);
        }
    }
    FD_ZERO(&except_set);
    for (const auto& except_socket: except_list) {
        const auto& fd = except_socket.sockfd();
        if (NET_INVALID_SOCKET != fd) {
            if (fd > max_fd) {
                max_fd = fd;
            }
            FD_SET(fd, &except_set);
        }
    }

    if (max_fd == 0) {
        return 0;
    }
    Duration remaining_time(timeout);
    int rc;
    do {
        auto tv = to_timeval(remaining_time);
        auto start = std::chrono::steady_clock::now();
        rc = ::select(max_fd + 1, &read_set, &write_set, &except_set, &tv);
        if (rc < 0 && EINTR == SocketImpl::last_error()) {
            auto end = std::chrono::steady_clock::now();
            Duration elapsed = end - start;
            if (elapsed < remaining_time) {
                remaining_time -= elapsed;
            }
            else {
                remaining_time = Duration::zero();
            }
        }
    } while (rc < 0 && EINTR == SocketImpl::last_error());
    if (rc < 0) {
        SocketImpl::throw_error();
    }

    List read_ready;
    for (const auto& read_socket: read_list) {
        const auto& fd = read_socket.sockfd();
        if (NET_INVALID_SOCKET != fd && FD_ISSET(fd, &read_set)) {
            read_ready.emplace_back(read_socket);
        }
    }
    std::swap(read_list, read_ready);
    List write_ready;
    for (const auto& write_socket: read_list) {
        const auto& fd = write_socket.sockfd();
        if (NET_INVALID_SOCKET != fd && FD_ISSET(fd, &write_set)) {
            write_ready.emplace_back(write_socket);
        }
    }
    std::swap(write_list, write_ready);
    List except_ready;
    for (const auto& except_socket: except_list) {
        const auto& fd = except_socket.sockfd();
        if (NET_INVALID_SOCKET != fd && FD_ISSET(fd, &write_set)) {
            except_ready.emplace_back(except_socket);
        }
    }
    std::swap(except_list, except_ready);
    return rc;
}

bool Socket::operator==(const Socket& socket) const {
    return m_impl == socket.m_impl;
}

bool Socket::operator!=(const Socket& socket) const {
    return m_impl != socket.m_impl;
}

bool Socket::operator<(const Socket& socket) const {
    return m_impl < socket.m_impl;
}

bool Socket::operator<=(const Socket& socket) const {
    return m_impl <= socket.m_impl;
}

bool Socket::operator>(const Socket& socket) const {
    return m_impl > socket.m_impl;
}

bool Socket::operator>=(const Socket& socket) const {
    return m_impl >= socket.m_impl;
}

void Socket::close() {
    m_impl->close();
}

bool Socket::poll(const Duration& timeout, int mode) const {
    return m_impl->poll(timeout, mode);
}

int Socket::available() const {
    return m_impl->available();
}

void Socket::set_send_buffer_size(int size) {
    m_impl->set_send_buffer_size(size);
}

int Socket::get_send_buffer_size() const {
    return m_impl->get_send_buffer_size();
}

void Socket::set_receive_buffer_size(int size) {
    m_impl->set_receive_buffer_size(size);
}

int Socket::get_receive_buffer_size() const {
    return m_impl->get_receive_buffer_size();
}

void Socket::set_send_timeout(const Duration& timeout) {
    m_impl->set_send_timeout(timeout);
}

Duration Socket::get_send_timeout() const {
    return m_impl->get_send_timeout();
}

void Socket::set_receive_timeout(const Duration& timeout) {
    m_impl->set_receive_timeout(timeout);
}

Duration Socket::get_receive_timeout() const {
    return m_impl->get_receive_timeout();
}

void Socket::set_option(int level, int option, int value) {
    m_impl->set_option(level, option, value);
}

void Socket::set_option(int level, int option, unsigned value) {
    m_impl->set_option(level, option, value);
}

void Socket::set_option(int level, int option, unsigned char value) {
    m_impl->set_option(level, option, value);
}

void Socket::set_option(int level, int option, const Duration& value) {
    m_impl->set_option(level, option, value);
}

void Socket::set_option(int level, int option, const IpAddress& value) {
    m_impl->set_option(level, option, value);
}

void Socket::get_option(int level, int option, int& value) const {
    m_impl->get_option(level, option, value);
}

void Socket::get_option(int level, int option, unsigned& value) const {
    m_impl->get_option(level, option, value);
}

void Socket::get_option(int level, int option, unsigned char& value) const {
    m_impl->get_option(level, option, value);
}

void Socket::get_option(int level, int option, Duration& value) const {
    m_impl->get_option(level, option, value);
}

void Socket::set_linger(bool enable, int seconds) {
    m_impl->set_linger(enable, seconds);
}

void Socket::get_linger(bool& enabled, int& seconds) const {
    m_impl->get_linger(enabled, seconds);
}

void Socket::set_no_delay(bool flag) {
    m_impl->set_no_delay(flag);
}

bool Socket::get_no_delay() const {
    return m_impl->get_no_delay();
}

void Socket::set_keep_alive(bool flag) {
    m_impl->set_keep_alive(flag);
}

bool Socket::get_keep_alive() const {
    return m_impl->get_keep_alive();
}

void Socket::set_reuse_address(bool flag) {
    m_impl->set_reuse_address(flag);
}

bool Socket::get_reuse_address() const {
    return m_impl->get_reuse_address();
}

void Socket::set_reuse_port(bool flag) {
    m_impl->set_reuse_port(flag);
}

bool Socket::get_reuse_port() const {
    return m_impl->get_reuse_port();
}

void Socket::set_OOB_inline(bool flag) {
    m_impl->set_OOB_inline(flag);
}

bool Socket::get_OOB_inline() const {
    return m_impl->get_OOB_inline();
}

void Socket::set_blocking(bool flag) {
    m_impl->set_blocking(flag);
}

bool Socket::is_blocking() const {
    return m_impl->get_blocking();
}

SocketImpl* Socket::impl() const {
    return m_impl.get();
}

net_socket_t Socket::sockfd() const {
    return m_impl->sockfd();
}

SocketAddress Socket::get_address() const {
    return m_impl->get_address();
}

SocketAddress Socket::peer_address() const {
    return m_impl->get_peer_address();
}

void Socket::init(int af) {
    m_impl->init(af);
}

} // namespace net
