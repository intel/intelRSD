/*!
 * @brief MUX implementation.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file mux.cpp
 */

#include "ipmb/mux.hpp"
#include "logger/logger_factory.hpp"
#include "ipmb/utils.hpp"
#include <stdexcept>
#include <cstring>



using namespace agent::compute::ipmb;

constexpr const std::uint16_t Mux::MUX_TIMEOUT_SEC;


Mux::~Mux() {}


void Mux::close() {
    m_socket.close();
}


void Mux::connect(const net::SocketAddress& mux_address) {
    m_socket.connect(mux_address);

    m_socket.set_blocking(false);

    send(mode());

    auto resp = recv(MUX_MAX_ACK_LENGTH); // Should receive "OK" or "NOK"

    // Stringify the [unsigned) bytes received
    std::string rstr{resp.cbegin(), resp.cend()};

    if ("OK" != rstr) {
        throw std::runtime_error("MUX NAK'd connection: (received \"" + rstr + "\")");
    }
}


byte_vec_t Mux::recv(std::size_t size, const net::Duration& timeout) {
    if (!m_socket.poll(timeout, net::Socket::SELECT_READ)) {
        throw net::TimeoutException();
    }
    byte_vec_t buffer;
    buffer.resize(size);
    auto bytes = m_socket.receive_bytes(buffer.data(), size);
    if (0 >= bytes) {
        throw std::runtime_error(" Cannot read MUX data rc: "
                                 + std::to_string(bytes)
                                 + " socket: " + std::strerror(errno));
    }
    buffer.resize(static_cast<unsigned>(bytes));
    return buffer;
}


void Mux::send(const byte_vec_t& in, const net::Duration& timeout) {
    if (!m_socket.poll(timeout, net::Socket::SELECT_WRITE)) {
        throw net::TimeoutException();
    }
    auto write_size = m_socket.send_bytes(in.data(), in.size());
    if (in.size() != static_cast<std::size_t>(write_size)) {
        throw std::runtime_error("Written "
                                 + std::to_string(static_cast<std::size_t> (write_size))
                                 + " bytes out of " + std::to_string(in.size()));
    }
}
