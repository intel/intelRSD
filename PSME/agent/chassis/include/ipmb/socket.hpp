/*!
 * @copyright
 * Copyright (c) 2016 Intel Corporation
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
 * @file socket.hpp
 * @brief socket
 *
 * Taken from storage, but refactored to be a bit more generic in its usage.
 * Only a minimal implementation has been created, not a fully expansive class.
 * Rather it is assumed to be expanded as needed.
 */

#pragma once
#include <vector>
#include <string>
#include <cstddef>

extern "C" {
#include <arpa/inet.h>
}

namespace agent {
namespace chassis {
namespace ipmb {

typedef std::vector<std::uint8_t> byte_vec_t;

/*! Class represents socket for tgt */
class Socket {
public:

    /*!
     * Redefinitions of INADDR_LOOPBACK as standard library uses old C cast
     *  definitions, which causes compiler to gripe
     */
    static constexpr const in_addr_t LOOPBACK = 0x7f000001;

    /*! create socket, throw exception on error */
    Socket(::sa_family_t domain = AF_INET, int type = SOCK_STREAM, int protocol = 0);

    /*! Virtual destructor */
    virtual ~Socket();

    /*! Disable copy constructor */
    Socket(const Socket&) = delete;

    /*! Disable assignment operator */
    Socket& operator=(const Socket&) = delete;

    /*!
     * @brief Socket connect, throw exception on error
     * @param port of server (optional)
     * @param address of server (optional)
     *
     * [Re]connect/define explicit port
     * if 0 (empty parameters), then use last configured port
     * If last configured port is 0, throw exception
     */
    virtual void connect(::in_port_t port = 0, ::in_addr_t address = LOOPBACK);

    /*!
     * @brief return whether socket is actively connected
     */
    bool is_connected() { return m_connected; }

    /*!
     * @brief Close socket
     */
    void close();

    /*!
     * @brief Get socket descriptior
     * @return Socket descriptor
     */
    int fd() const;

    /*!
     * @brief Send character string to socket
     * @param in stream to send
     * @param flags is optional socket parameters (e.g., NON_BLOCK)
     */
    void send(const std::string& in, int flags = 0);

    /*!
     * @brief Send byte vector to socket
     * @param in byte vector to send
     * @param flags is optional socket parameters (e.g., NON_BLOCK)
     */
    void send(const byte_vec_t& in, int flags = 0);

    /*!
     * @brief Read character string from socket
     * @param size is maximum number of bytes to expect
     * @param flags is optional socket parameters (e.g., NON_BLOCK)
     * @return byte vector
     */
    byte_vec_t recv(std::size_t size, int flags = 0);

    /*!
     * @brief Set nonblocking flag
     * @param nonblocking nonblocking flag
     */
    void set_nonblocking(bool nonblocking);

    /*!
     * @brief Get nonblocking flag
     * @return nonblocking flag
     */
    bool is_nonblocking();

private:
    int             m_sock{-1};
    ::in_addr_t     m_addr{LOOPBACK};
    ::in_port_t     m_port{0};
    ::sa_family_t   m_domain{AF_INET};
    int             m_type{SOCK_STREAM};
    int             m_protocol{0};
    bool            m_connected{false};
    bool            m_nonblocking{false};
};

}
}
}


