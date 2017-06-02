/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 *
 * @file icmp.hpp
 * @brief Ping software status implementation
*/

#pragma once

#include "agent-framework/status/module_software_status.hpp"
#include "agent-framework/status/module_status.hpp"
#include "agent-framework/logger_ext.hpp"

#include <stdexcept>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <cassert>

#include <fcntl.h>
#include <resolv.h>
#include <netdb.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <sys/socket.h>

/*! Agent namespace */
namespace agent {
/*! Compute namespace */
namespace compute {
/*! Status namespace */
namespace status {

/*!
 * @brief Icmp class.
 * */
class Icmp final {
public:
    /*!
     * @brief Class default constructor.
     * */
    Icmp(const std::string& ip_address) :
            m_ip_address(ip_address) {
    }

    /*!
    * @brief Sends ICMP ping message to module.
    *
    * @return true if got response, otherwise false.
    */
    bool ping() const;

private:
    std::string m_ip_address{};

    static constexpr unsigned int CFG_SIZE_ICMP_MESSAGE = 56; // 64 bytes of ICMP - 8 bytes of header.
    typedef struct _icmp_packet_t {
        unsigned char type;
        unsigned char code;
        unsigned short checksum;
        unsigned short id;
        unsigned short sequence;
        char message[CFG_SIZE_ICMP_MESSAGE];
    } icmp_packet_t;

    time_t m_receive_pong_timeout_sec = 3;
    /*!< Timeout for receiving pong in seconds. */
    suseconds_t m_receive_pong_timeout_usec = 0; /*!< Timeout for receiving pong in microseconds. */

    /*!
     * @brief Computes the Internet Checksum RFC 1071.
     *
     * @param icmp_packet reference to icmp_packet which checksum will be calculated.
     * @return icmp_packet checksum compatible with RFC 1071.
     */
    unsigned short checksum(icmp_packet_t& icmp_packet) const;

    /*!
     * @brief Tries to send ICMP ping message and waits with timeout for pong response.
     *
     * @param socket POSIX socket to send data through it.
     * @param packet reference to ICMP packet structure with data.
     * @param address pointer to struct with destination address.
     *
     * @return true if pong has been received, otherwise false.
     */
    bool send_ping_receive_pong(int socket, icmp_packet_t& packet, struct sockaddr* address) const;

    /*!
     * @brief Creates socket handler and set options needed to perform ICMP ping.
     *
     * @param protocol protocol type.
     *
     * @return socket handler.
     */
    int create_socket_and_set_option(int protocol) const;

    /*!
     * @brief Creates destination address structure.
     *
     * @param ip_address reference to destination hostname.
     *
     * @return destination address struct with data.
     */
    struct sockaddr_in create_destination_address(const std::string& ip_address) const;

    /*!
     * @brief First phase of checksum computing. Sum up 16bit chunks of data.
     *
     * @param packet reference to packet with data.
     *
     * @return computed sum.
     */
    unsigned int calculate_sum_of_2byte_chunks(icmp_packet_t& packet) const;

    /*!
     * @brief Calculate sum of 4bit chunks with carry.
     *
     * @param number a number to calculate its 4bit chunks sum.
     *
     * @return sum of 4bit chunks with carry.
     */
    unsigned int calculate_sum_with_carry(unsigned int number) const;

    /*!
     * @brief Fills packet structure with mandatory data and returns it.
     *
     * @return icmp_packet_t ready to send.
     */
    icmp_packet_t create_ping_packet() const;
};

}
}
}
