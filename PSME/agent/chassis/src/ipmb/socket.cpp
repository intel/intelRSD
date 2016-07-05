/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
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
 * @file socket.cpp
 * @brief Encapsulates system INET socket management
 * @details
 * C++ standard libraries do not provide socket objects.  Moreover, some of the
 * system interfaces require care as header files or other macros/defines may
 * throw compiler errors (e.g., use of old style cast).  This provides a
 * minimalistic interface with an OOP management to those system calls.
 */

#include "ipmb/socket.hpp"
#include "logger/logger_factory.hpp"

#include <stdexcept>
#include <cstring>

extern "C" {
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
}

using namespace agent::chassis::ipmb;

Socket::Socket(::sa_family_t domain, int type, int protocol) {
    m_domain = domain;
    m_type = type;
    m_protocol = protocol;
}

Socket::~Socket() {
    close();
}

void Socket::close() {
    if (m_connected) {
        ::close(m_sock);
    }
    m_connected = false;
}

void Socket::connect(::in_port_t port, ::in_addr_t address) {
    if (0 == port) {
        if (0 == m_port) {
            throw std::runtime_error("Port not defined");
        }
        // reconnect to last configured port
    }
    else m_port = port; // [Re]define what port to connect to
    m_addr = address;   // [Re]define what address to connect to

    union sock_address {
        sockaddr    sa;
        sockaddr_in sa_in;
    } server{};

    // Work around gcc old cast style in macros htons/htonl
    // see: http://stackoverflow.com/questions/24629788/old-style-cast-while-using-htons
    server.sa_in.sin_port = (htons)(m_port);
    server.sa_in.sin_addr.s_addr = (htonl)(m_addr);
    server.sa_in.sin_family = m_domain;

    m_sock = socket(m_domain, m_type, m_protocol);
    if (0 > m_sock) {
        throw std::runtime_error("Cannot create socket");
    }

    // Perform connection to socket on parameters and throw exception as needed
    if(0 > ::connect(m_sock, &server.sa, sizeof(server.sa_in))) {
        close();
        throw std::runtime_error(
            "Cannot connect to port " +
            std::to_string(static_cast<unsigned>(m_port)) +
            ": " + std::strerror(errno));
    }
    m_connected = true;
}

void Socket::send(const std::string& in, int flags) {
    if (!m_connected) {
        throw std::runtime_error("Unable to send data: not connected");
    }
    if (0 < in.size()) {
        auto write_size = ::send(m_sock, in.c_str(), in.size(), flags);
        if (0 > write_size || in.size() != static_cast<std::size_t>(write_size)) {
            close();
            throw std::runtime_error("Cannot write data to port " +
                                     std::to_string(static_cast<unsigned>(m_port)) +
                                     " socket: " + std::strerror(errno));
        }
    }
}

void Socket::send(const byte_vec_t& in, int flags) {
    if (!m_connected) {
        throw std::runtime_error("Unable to send data: not connected");
    }
    if (0 < in.size()) {
        auto write_size = ::send(m_sock, &in[0], in.size(), flags);
        if (0 > write_size || in.size() != static_cast<std::size_t>(write_size)) {
            close();
            throw std::runtime_error("Cannot write data to port " +
                                     std::to_string(static_cast<unsigned>(m_port)) +
                                     " socket: " + std::strerror(errno));
        }
    }
}

int Socket::fd() const {
    return m_sock;
}

byte_vec_t Socket::recv(std::size_t size, int flags) {
    if (!m_connected) {
        throw std::runtime_error("Unable to receive data: not connected");
    }

    // Create a buffer of the requested length.  Note, we need to resize() and
    // NOT reserve() as the buffer won't be updated by the system recv() call
    // (which is just writing the received data to the pointer passed).
    // Worse, when we resize() the vector back down to the actual number of bytes
    // received, since it thought it was 0 before, it will size it up and
    // overwrite our fetched buffer with zeroes.
    byte_vec_t  buffer;
    buffer.resize(size);

    auto bytes = ::recv(m_sock, &buffer[0], size, flags);
    if (0 >= bytes) {
        m_connected = false;
        throw std::runtime_error("Cannot read data from port " +
                                 std::to_string(static_cast<unsigned>(m_port)) +
                                 " socket: " + std::strerror(errno));
    }

    buffer.resize(static_cast<unsigned>(bytes));
    return buffer;
}

void Socket::set_nonblocking(bool nonblocking) {
    if (m_nonblocking != nonblocking) {
        // Get the current flags for the socket
        auto sflags = ::fcntl(fd(), F_GETFL, 0);

        if (nonblocking) {
            ::fcntl(fd(), F_SETFL, sflags | O_NONBLOCK);
        }
        else {
            ::fcntl(fd(), F_SETFL, sflags & (~O_NONBLOCK));
        }
        m_nonblocking = nonblocking;
    }
    // else no change in state
}

bool Socket::is_nonblocking() {
    return m_nonblocking;
}
