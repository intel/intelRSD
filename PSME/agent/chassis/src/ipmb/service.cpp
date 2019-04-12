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
 * @file service.cpp
 * @brief IPMI IPMB Service implementation.
 * */

#include <ipmb/service.hpp>
#include <ipmb/dispatcher.hpp>
#include <ipmb/patcher.hpp>
#include <ipmb/client.hpp>
#include <logger/logger_factory.hpp>
#include "configuration/configuration.hpp"
#include "net/stream_socket.hpp"

#include <chrono>

using namespace agent::chassis::ipmb;

const net::SocketAddress Service::MUX_SOCKET_ADDRESS = net::SocketAddress{"127.0.0.1", 5623};
constexpr const std::uint16_t Service::MUX_RECV_TIMEOUT_SEC;

void Service::init_mux_connection(Responder& socket, const net::SocketAddress& mux_address) {
    static constexpr std::chrono::seconds NAP_TIME{2};
    while (is_running()) {
        log_debug(LOGUSR, "Attempting to connect to MUX on " << mux_address);
        try {
            socket.connect(mux_address);
            log_info(LOGUSR, "Connection to MUX (" << mux_address << ") established successfully");
            return;
        }
        catch(const std::exception& e) {
            socket.close();
            log_error(LOGUSR, "MUX (" << mux_address << ") connect failure: " << e.what());
        }
        std::this_thread::sleep_for(NAP_TIME);
    }
}

net::SocketAddress Service::get_mux_address() {
    auto mux_socket_address = Service::MUX_SOCKET_ADDRESS;
    try {
        auto config = configuration::Configuration::get_instance().to_json();
        std::uint16_t mux_port = config.at("cyMux").at("port").get<std::uint16_t>();
        auto mux_ip = net::IpAddress::from_string(config["cyMux"].at("ipv4").get<std::string>());
        mux_socket_address = net::SocketAddress(mux_ip, mux_port);
    }
    catch(const std::exception& e) {
        log_warning("agent", "Unable to read cyMux connection configuration: "
                << e.what() << ". Using default address: " << mux_socket_address);
    }
    return mux_socket_address;
}

void Service::execute() {
    log_debug(LOGUSR, "IPMB Service started.");

    init_mux_connection(m_mux, Service::get_mux_address());

    while (is_running()) {
        try {
            slave_read_and_dispatch();
        }
        catch(std::exception& e) {
            log_error(LOGUSR, "IPMB Service error: " << e.what());
            m_mux.close();
            init_mux_connection(m_mux, Service::get_mux_address());
        }
    }
    log_debug(LOGUSR, "IPMB Service stopped.");
}

void Service::slave_read_and_dispatch() {
    auto buffer = m_mux.recv(IpmiMessage::IPMI_MAX_MSG_LENGTH + 1,
                             std::chrono::seconds(MUX_RECV_TIMEOUT_SEC));

    log_debug(LOGUSR, "Received  (" << buffer.size() << " bytes) frame: "
            << utils::print_frame(&buffer[0], static_cast<uint16_t>(buffer.size())));

    auto request = message_builder(buffer);
    if (!verify_chcksum(request, buffer)) {
        return; // Ensure valid message
    }

    Dispatcher dispatcher{};
    auto command = dispatcher.dispatch(request);
    command->unpack(request);
    if (request.is_request()) {
        IpmiMessage response{};
        command->pack(response);
        Patcher patcher{};
        patcher.patch(request, response);
        master_write(response);
    }
}

void Service::master_write(IpmiMessage& msg) {
    // Build the message to send to the MUX (slave address : IPMB message)
    auto buffer = msg.as_byte_array();

    try {
        log_debug(LOGUSR, "Sending  (" << msg.get_len() << " bytes) frame: "
                << utils::print_frame(buffer.data(), static_cast<uint16_t>(buffer.size())));

        m_mux.send(buffer);
    }
    catch (const std::exception& e) {
        log_error(LOGUSR, "Unable to send response data to MUX: " << e.what());
        m_mux.close();
    }
}

void Service::send_request_process_response(IpmiMessage& msg) {
    if (!msg.is_request()) {
        log_warning(LOGUSR, "Expected IPMI request, got IPMI response.");
        return;
    }

    // Message is an outbound request, encapsulate transaction here.
    try {
        // Build the message to send to the MUX (slave address : IPMB message)
        auto buffer = msg.as_byte_array();

        log_debug(LOGUSR, "Sending request (" << msg.get_len() << " bytes) frame: "
                << utils::print_frame(buffer.data(), static_cast<uint16_t>(buffer.size())));

        // Create a new client connection to the MUX
        Client c_mux;

        c_mux.connect(Service::get_mux_address());
        c_mux.send(buffer);

        // Fetch response (slave address : IPMB response)
        buffer = c_mux.recv(IpmiMessage::IPMI_MAX_MSG_LENGTH + 1);

        log_debug(LOGUSR, "Received  (" << buffer.size() << " bytes) frame: "
                << utils::print_frame(&buffer[0], static_cast<uint16_t> (buffer.size())));

        // Process response
        auto rsp = message_builder(buffer);
        if (!verify_chcksum(rsp, buffer)) {
            return; // Ensure valid message
        }

        // Now dispatch the response
        Dispatcher dispatcher{};
        auto command = dispatcher.dispatch(rsp);
        command->unpack(rsp);
    }
    catch (const std::exception &e) {
        log_error(LOGUSR, "Unable process request via MUX: " << e.what());
    }
}

bool Service::verify_chcksum(const IpmiMessage& msg, const byte_vec_t& read_buffer) {
    auto hdr_chcksum = utils::ipmi_checksum(&read_buffer[0], 2);
    auto data_chcksum = utils::ipmi_checksum(&(read_buffer[3]), static_cast<uint16_t>(read_buffer.size() - 4));

    if (msg.get_h_chcksum() != hdr_chcksum) {
        log_error(LOGUSR, "Incorrect header checksum!");
        return false;
    }

    if (msg.get_d_chcksum() != data_chcksum) {
        log_error(LOGUSR, "Incorrect body checksum!");
        log_debug(LOGUSR, "Received: " << static_cast< unsigned>(msg.get_d_chcksum()));
        log_debug(LOGUSR, "Calculated: " << static_cast< unsigned>(data_chcksum));
        return false;
    }

    return true;
}

IpmiMessage Service::message_builder(const byte_vec_t& read_buffer) {
    IpmiMessage msg;
    msg.add_len(static_cast<uint16_t>(read_buffer.size()));

    msg.set_dest(read_buffer[0] );
    msg.set_netfn(IPMI_MSG_NETFN(read_buffer[1]));
    msg.set_dlun(read_buffer[1] & IPMI_MSG_DST_LUN_MASK);
    msg.set_h_chcksum(read_buffer[2]);

    msg.set_src(read_buffer[3]);
    msg.set_seq(IPMI_MSG_SEQ(read_buffer[4]));
    msg.set_slun(read_buffer[4] & IPMI_MSG_SRC_LUN_MASK);
    msg.set_cmd(read_buffer[5]);

    if ((read_buffer.size() - 1) > IPMB_OFFSET_DATA) {
            msg.set_data(&read_buffer[IPMB_OFFSET_DATA],
                static_cast<uint16_t>((read_buffer.size() - 1) - IPMB_OFFSET_DATA));
    }

    msg.set_d_chcksum(read_buffer[read_buffer.size() - 1]);
    return msg;
}
