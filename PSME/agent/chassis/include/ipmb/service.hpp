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
 * @file ipmb/service.hpp
 * @brief IPMI IPMB Service.
 * */

#pragma once
#include <ipmb/ipmi_message.hpp>
#include <ipmb/responder.hpp>

#include "generic/worker_thread.hpp"
#include <agent-framework/threading/thread.hpp>
#include <mutex>

/*! Agent namespace */
namespace agent {
/*! Chassis namespace */
namespace chassis {
/*! IPMB namespace */
namespace ipmb {

using namespace agent_framework::generic;

/*! IpmiService interface for IPMB. */
class Service final : public agent_framework::threading::Thread {
public:
    /*! Default MUX socket address */
    static const net::SocketAddress MUX_SOCKET_ADDRESS;

    /*! Default timeout on recv from MUX, after which we assume connection is lost. */
    static constexpr const std::uint16_t MUX_RECV_TIMEOUT_SEC = 30;

    /*!
     * Sends request, waits for and process response.
     * @param[in] request IPMI request message.
     */
    static void send_request_process_response(IpmiMessage& request);

    /*!
     * @brief Returns SocketAddress cyMUX is listening on.
     * @return SocketAddress cyMUX is listening on.
     */
    static net::SocketAddress get_mux_address();

private:

    void execute() override;

    void init_mux_connection(Responder& socket, const net::SocketAddress& mux_address);

    /*!
     * @brief Slave read from MUX
     * */
    void slave_read_and_dispatch();

    /*!
     * @brief Master write to MUX
     * @param msg IPMI message
     * */
    void master_write(IpmiMessage& msg);

    /*! Socket connection to cyMUX */
    Responder m_mux{};

    static bool verify_chcksum(const IpmiMessage &msg, const byte_vec_t &read_buffer);
    static IpmiMessage message_builder(const byte_vec_t& read_buffer);
};

}
}
}
