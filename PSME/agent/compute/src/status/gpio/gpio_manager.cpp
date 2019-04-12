/*!
 * @brief GPIO manager class implementation
 *
 * Generic "interface" class. It should be used as a reference to "real" controller.
 * Handle all passed requests. Connection data is to be passed as shared ptr,
 * no references are implemented (memory management actually is to be done in
 * the code)
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
 * @file gpio_manager.cpp
 */

#include "status/gpio/gpio_manager.hpp"
#include "ipmb/ipmi_message.hpp"
#include "ipmb/command/get_sled_presence.hpp"
#include "ipmb/client.hpp"
#include "logger/logger_factory.hpp"

#include <configuration/configuration.hpp>


using namespace agent::compute::ipmb;
using namespace agent::compute::ipmb::command;

namespace agent {
namespace compute {
namespace status {

GpioManager::GpioManager() {
    try {
        auto config = configuration::Configuration::get_instance().to_json();
        std::uint16_t mux_port = config.at("cyMux").at("port").get<std::uint16_t>();
        auto mux_ip = net::IpAddress::from_string(config.at("cyMux").at("ipv4").get<std::string>());
        m_mux_socket_address = net::SocketAddress(mux_ip, mux_port);
        if (config["cyMux"].count("gpioMinimalReadIntervalMs")) {
            m_update_interval = std::chrono::milliseconds(config["cyMux"]["gpioMinimalReadIntervalMs"].get<std::uint16_t>());
        }
    }
    catch (const std::exception& e) {
        log_warning("agent", "Unable to read cyMux connection configuration: "
            << e.what() << ". Using default address: " << m_mux_socket_address);
    }
    log_info("agent", "gpio minimal read interval: " << m_update_interval.count() << "ms");
}

std::uint8_t GpioManager::get_presence() {
    std::lock_guard<std::mutex> guard{m_mutex};
    const TimePoint now = Clock::now();
    if (now - m_last_update_time >= m_update_interval) {
        update_presence();
        m_last_update_time = now;
    }
    return m_presence;
}

bool GpioManager::is_present(std::uint8_t slot) {
    const auto presence = get_presence();
    const auto presence_mask = 1 << (slot - 1);
    return (presence & presence_mask);
}

std::chrono::milliseconds GpioManager::get_minimal_update_interval() const {
    return m_update_interval;
}

void GpioManager::send_request_process_response(IpmiMessage& msg) {
    // Message is an outbound request, encapsulate transaction here.
    try {
        // Build the message to send to the MUX (slave address : IPMB message)
        auto buffer = msg.as_byte_array();

        log_debug(LOGUSR, "Sending request (" << msg.get_len() << " bytes) frame: "
                                              << utils::print_frame(buffer.data(),
                                                                    static_cast<uint16_t>(buffer.size())));

        // Create a new client connection to the MUX
        Client c_mux;

        c_mux.connect(m_mux_socket_address);
        c_mux.send(buffer);

        // Fetch response (slave address : IPMB response)
        try {
            buffer = c_mux.recv(IpmiMessage::IPMI_MAX_MSG_LENGTH + 1);
        }
        catch (const std::exception& e) {
            log_error(LOGUSR, "Cannot receive response from cyMUX: " << e.what());
            throw;
        }

        log_debug(LOGUSR, "Received  (" << buffer.size() << " bytes) frame: "
                                        << utils::print_frame(&buffer[0], static_cast<uint16_t> (buffer.size())));

        // Process response
        auto rsp = message_builder(buffer);
        if (!verify_checksum(rsp, buffer)) {
            return; // Ensure valid message
        }

        GetSledPresence get_sled_presence{};
        get_sled_presence.unpack(rsp);
        // update only when the get operation is successful
        if (get_sled_presence.is_valid_value()) {
            m_presence = get_sled_presence.get_sled_presence();
        }
    }
    catch (const std::exception& e) {
        log_error(LOGUSR, "Unable to send/receive data to/from MUX: " << e.what());
    }
}


bool GpioManager::verify_checksum(const IpmiMessage& msg, const byte_vec_t& read_buffer) {
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


IpmiMessage GpioManager::message_builder(const byte_vec_t& read_buffer) {
    IpmiMessage msg;
    msg.add_len(static_cast<uint16_t>(read_buffer.size()));

    msg.set_dest(read_buffer[0]);
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

void GpioManager::update_presence() {
    IpmiMessage req{};

    GetSledPresence command{};
    command.pack(req);

    send_request_process_response(req);
}

}
}
}
