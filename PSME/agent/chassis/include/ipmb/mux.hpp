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
 * @file mux.hpp
 * @brief MUX interface
 *
 */

#pragma once
#include "ipmb/socket.hpp"

namespace agent {
namespace chassis {
namespace ipmb {

/*! General class for multiplex connectivity with I2C device */
class Mux : public Socket {

public:
    static constexpr const ::in_port_t DEFAULT_IPMB_MUX_PORT = 5623;
    static constexpr const ::time_t MUX_TIMEOUT_MSEC = 2000;
    static constexpr const auto TIMEDOUT = "Timed out";

    /*! @brief Default constructo */
    Mux() : Socket() {}

    /*! @brief Copy constructor */
    Mux(const Mux&) = delete;

    /*! @brief Assignment operator */
    Mux& operator=(const Mux&) = delete;

    /*!
     * @brief Mux connect, throw exception on error
     * @param port MUX port
     * @param address MUX ip address
     * */
    void connect(in_port_t port = DEFAULT_IPMB_MUX_PORT, ::in_addr_t address = LOOPBACK);

    /*!
     * @brief Send character string to socket (wrapped with timeout support)
     * @param[in] in stream to send
     */
    void send(const byte_vec_t& in);

    /*!
     * @brief Read character string from socket (wrapped with timeout support)
     * @param size is maximum number of bytes to expect
     * @return byte vector
     */
    byte_vec_t recv(std::size_t size);

    /*!
     * @brief Retrns mode or type of MUX connection (client/responder/etc.)
     *  @return mode or type of MUX connection
     * */
    virtual byte_vec_t mode() const = 0;   // Must be defined by derived class

private:
    struct timeval  m_timeout{(MUX_TIMEOUT_MSEC / 1000), (MUX_TIMEOUT_MSEC % 1000) * 1000};
};

}
}
}


