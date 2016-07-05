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
 * @file socket.hpp
 * @brief tgt socket
*/

#ifndef ISCSI_TGT_SOCKET_HPP
#define	ISCSI_TGT_SOCKET_HPP

#include <vector>

namespace agent {
namespace storage {
namespace iscsi {
namespace tgt {

/*! Class represents socket for tgt */
class Socket {
public:
    /*! RAII create socket, throw exception on error */
    Socket();
    ~Socket();

    /*! disable copy */
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    /*! Socket connect, throw exception on error */
    void connect();

    /*!
     * @brief Get socket descriptior
     * @return Socket descriptor
     */
    int get_descriptor() const;

    /*!
     * @brief Send data
     * @param in Data to send
     */
    void send(const std::vector<char>& in) const;

    /*!
     * @brief Read data from socket
     * @param out Buffer for storing data
     */
    void read(std::vector<char>& out) const;

    /*!
     * @brief Recive data from socket
     *
     * @param buffer Data buffer
     * @param size Data size to recive
     */
    void recive(char* buffer, const std::size_t size);

private:
    /*!
     * @brief Create socket
     *
     * @param domain Socket domain
     * @param type Socket type
     * @param protocol Socket protocol
     */
    void create_socket(int domain, int type, int protocol);
    /*! Destroy socket */
    void destroy_socket();

private:
    int m_fd{-1};
};

}
}
}
}

#endif	/* ISCSI_TGT_SOCKET_HPP */

