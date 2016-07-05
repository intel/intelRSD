/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 * @file mux.cpp
 * @brief Perform basic communication with MUX server for shared I2C communication
 */

#include "ipmb/mux.hpp"
#include "logger/logger_factory.hpp"
#include "ipmb/utils.hpp"
#include <stdexcept>
#include <cstring>

using namespace agent::chassis::ipmb;

void Mux::connect(::in_port_t port, ::in_addr_t address) {

    // Perform basic socket connection
    Socket::connect(port, address);

    // keep the connection as non-blocking so we can time out on reads during
    // negotiation
    set_nonblocking(true);

    // Issue to MUX what kind of connection (client/responder/etc.)
    send(mode());

    // Check for an acknowledgement from the MUX for this type of connection
    // Don't use possibly overridden recv for negotiation.
    auto response = Mux::recv(MUX_MAX_ACK_LENGTH);   // Should receive "OK" or "NOK"

    if (0 == response.size()) {
        close();
        throw std::runtime_error("MUX returned no data during negotiation");
    }

    // Stringify the [unsigned) bytes received
    std::string rstr{};
    for (auto ch : response) rstr.push_back(static_cast<char>(ch));

    if ("OK" != rstr) {
        close();
        throw std::runtime_error("MUX NAK'd connection: (received \"" + rstr + "\")");
    }

    // return the socket back to the default blocking mode.
    set_nonblocking(false);
}

byte_vec_t Mux::recv(std::size_t size) {
    if (is_nonblocking()) {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(fd(), &rfds);

        // Sleep on the socket, until data is received or timeout
        auto retval = ::select(fd()+1, &rfds, nullptr, nullptr, &m_timeout);
        if (0 == retval) {
            throw std::runtime_error(TIMEDOUT);
        }
        else if (retval < 0) {
            throw std::runtime_error(std::strerror(errno));
        }
    }
    return Socket::recv(size);
}

void Mux::send(const byte_vec_t& in) {
    if (is_nonblocking()) {
        fd_set wfds;
        FD_ZERO(&wfds);
        FD_SET(fd(), &wfds);

        // Sleep on the socket, until data is ready to send or timeout
        auto retval = ::select(fd()+1, nullptr, &wfds, nullptr, &m_timeout);
        if (0 == retval) {
            throw std::runtime_error(TIMEDOUT);
        }
        else if (retval < 0) {
            throw std::runtime_error(std::strerror(errno));
        }
    }
    return Socket::send(in);
}

