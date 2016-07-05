/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * @file service.cpp
 * @brief IPMI IPMB Service implementation.
 * */

#include <ipmb/service.hpp>
#include <ipmb/dispatcher.hpp>
#include <ipmb/client.hpp>
#include <logger/logger_factory.hpp>

#include <chrono>
#include <ratio>

using namespace agent::chassis::ipmb;

Service::~Service() {}

bool Service::init(::in_port_t mux_port) {
    std::unique_lock<std::mutex> lock(m_mutex);

    if (m_is_initialized) {
        log_warning(LOGUSR, "Service is already initialized.");
        return false;
    }

    // Starting Message Receiver Thread
    // (also used to reconnect to cyMUX if unavailable)
    if (!m_receiver_thread) {
        m_receiver_thread.reset(new ReceiverThread(mux_port));
        m_receiver_thread->start();
    }

    // Starting data processing thread
    if(!m_worker_thread) {
        m_worker_thread.reset(new WorkerThread());
        m_worker_thread->start();
    }

    log_debug(LOGUSR, "Attempting to connect to MUX");

    try {
        // Connect to the MUX server
        m_mux.connect(mux_port);
    }
    catch (const std::runtime_error& e) {
        m_mux.close();
        if (!m_server_failure_logged) {
            log_error(LOGUSR, "MUX Responder Negotiation falure: " << e.what());
            m_server_failure_logged = true;    // Only display error once
        }
        return false;
    }

    m_is_initialized = true;
    m_server_failure_logged = false;       // Reset so next failure is logged
    log_notice(LOGUSR, "IPMB Service initialized successfully.");
    return true;
}

bool Service::deinit(){
    if(m_is_working){
        log_error(LOGUSR, "Unable to deinitialize IPMB Service as it wasn't started");
        return false;
    }

    if(m_is_initialized){
        m_mux.close();
        m_is_initialized = false;
        log_notice(LOGUSR, "IPMB Service deinitialized successfully.");
        return true;
    }
    else {
        log_warning(LOGUSR, "Unable to deinitialize IPMB Service as it wasn't yet initialized.");
        return false;
    }
}

bool Service::start(void) {
    if (m_is_working) {
        log_warning(LOGUSR, "IPMB Service is already running.");
        return false;
    }

    if (!m_is_initialized) {
        log_error(LOGUSR, "Service is not initialized.");
        return false;
    }

    m_is_working = true;

    log_info(LOGUSR, "IPMI service started successfully." << std::endl);
    return true;
}

bool Service::stop(void) {
    if (m_is_working) {
        m_is_working = false;
        log_debug(LOGUSR, "IPMB Service stopped.");
        return true;
    }
    else {
        log_warning(LOGUSR, "Unable to stop IPMB Service as it wasn't started.");
        return false;
    }
}

bool Service::slave_read(IpmiMessage& msg) {
    try {
        auto buffer = m_mux.recv(IpmiMessage::IPMI_MAX_MSG_LENGTH + 1);
        log_debug(LOGUSR, "Received  (" << buffer.size() << " bytes) frame: " << utils::print_frame(&buffer[0], static_cast<uint16_t>(buffer.size())));

        if (0 == buffer.size()) {
            return false; // No data read
        }

        // Process response
        message_builder(msg, buffer);
        if (!verify_chcksum(msg, buffer)) {
            return false;  // Ensure valid message
        }
    }
    catch (const std::exception& e) {
        if (Mux::TIMEDOUT == e.what()) {
            return false;
        }
        log_error(LOGUSR, "Unable to read data from MUX: " << e.what());
        m_mux.close();
        m_is_initialized = false;
        return false;
    }
    return true;
}

bool Service::master_write(IpmiMessage& msg) {
    byte_vec_t  buffer{};

    // Build the message to send to the MUX (slave address : IPMB message)
    // resize the buffer to contain the message
    buffer.resize(msg.get_len());
    msg.to_rsp_buffer(&buffer[0]);

    if (msg.is_request()) {
        // Message is an outbound request, encapsulate transaction here.
        try {
            // Serialize client requests
            std::lock_guard<std::mutex> lock(m_client_mutex);

            Client c_mux{}; // Create a new client connection to the MUX

            log_debug(LOGUSR, "Sending  (" << msg.get_len() << " bytes) frame: "
                      << utils::print_frame(&buffer[0], static_cast<uint16_t>(buffer.size())));

            c_mux.send(buffer);

            // Fetch response (slave address : IPMB response)
            buffer = c_mux.recv(IpmiMessage::IPMI_MAX_MSG_LENGTH + 1);
            if (0 == buffer.size()) {
                log_error(LOGUSR, "Zero bytes read from MUX response");
                return false;
            }
            log_debug(LOGUSR, "Received  (" << buffer.size() << " bytes) frame: " << utils::print_frame(&buffer[0], static_cast<uint16_t>(buffer.size())));

            // Process response
            IpmiMessage rsp{};
            message_builder(rsp, buffer);
            if (!verify_chcksum(rsp, buffer)) {
                return false;  // Ensure valid message
            }

            // Now dispatch the response (short circuit Receiver/WorkerThread)
            Dispatcher dispatcher{};
            auto command = dispatcher.dispatch(rsp);
            command->unpack(rsp);

            m_client_failure_logged = false;
        }
        catch (const std::exception &e) {
            if (!m_client_failure_logged) {
                log_error(LOGUSR, "Unable to send request data to MUX: " << e.what());
                m_client_failure_logged = true;
            }
            return false;
        }
    }
    else {
        try {
            log_debug(LOGUSR, "Sending  (" << msg.get_len() << " bytes) frame: " << utils::print_frame(&buffer[0], static_cast<uint16_t>(buffer.size())));

            m_mux.send(buffer);

        } catch (const std::exception& e) {
            if (Mux::TIMEDOUT == e.what()) {
                return false;
            }
            log_error(LOGUSR, "Unable to send response data to MUX: " << e.what());
            m_mux.close();
            m_is_initialized = false;
        return false;
    }
    }
    return true;
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

void Service::message_builder(IpmiMessage& msg, const byte_vec_t& read_buffer) {
    msg.add_len(static_cast<uint16_t>(read_buffer.size()));

    msg.set_dest(read_buffer[0] );
    msg.set_netfn(IPMI_MSG_NETFN(read_buffer[1]));
    msg.set_dlun(read_buffer[1] & IPMI_MSG_DST_LUN_MASK);
    msg.set_h_chcksum(read_buffer[2]);

    msg.set_src(read_buffer[3]);
    msg.set_seq(IPMI_MSG_SEQ(read_buffer[4]));
    msg.set_slun(read_buffer[4] & IPMI_MSG_SRC_LUN_MASK);
    msg.set_cmd(read_buffer[5]);

    if (IPMB_OFFSET_DATA < (read_buffer.size() - 1)) {
            msg.set_data(&read_buffer[IPMB_OFFSET_DATA],
                static_cast<uint16_t>((read_buffer.size() - 1) - IPMB_OFFSET_DATA));
    }

    msg.set_d_chcksum(read_buffer[read_buffer.size() - 1]);
}
