/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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

#include "net/stream_socket.hpp"
#include <vector>

namespace agent {
namespace chassis {
namespace ipmb {

using byte_vec_t = std::vector<std::uint8_t>;

/*! General class for multiplex connectivity with I2C device */
class Mux {

public:
    /*! Default timeout value */
    static constexpr const std::uint16_t MUX_TIMEOUT_SEC = 2;

    /*! @brief Default constructor */
    Mux() = default;

    /*! @brief Destructor */
    virtual ~Mux();

    /*! @brief Copy constructor */
    Mux(const Mux&) = delete;

    /*! @brief Assignment operator */
    Mux& operator=(const Mux&) = delete;

    /*!
     * @brief Mux connect, throw exception on error
     * @param mux_address MUX socket address
     * */
    virtual void connect(const net::SocketAddress& mux_address);

    /**
     * @brief Attempt to send data stream to MUX, time out if necessary.
     * @param[in] in stream to send
     * @param[in] timeout timeout value
     */
    virtual void send(const byte_vec_t& in, const net::Duration& timeout = std::chrono::seconds(MUX_TIMEOUT_SEC));

    /**
     * @brief Attempt to receive stream from MUX, time out if necessary.
     *
     * @param size   maximum number of bytes to receive
     * @param[in] timeout timeout value
     * @return byte_vec_t - byte stream read
     */
    virtual byte_vec_t recv(std::size_t size, const net::Duration& timeout = std::chrono::seconds(MUX_TIMEOUT_SEC));

    /*!
     * @brief Returns mode or type of MUX connection (client/responder/etc.)
     * @return mode or type of MUX connection
     **/
    virtual byte_vec_t mode() const = 0;   // Must be defined by derived class

    /*! Closes underlying socket */
    void close();

protected:
    /*! Socket to MUX */
    net::StreamSocket m_socket{};
};

}
}
}
