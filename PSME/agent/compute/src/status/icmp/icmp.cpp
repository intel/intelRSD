/*!
 * @section LICENSE
 *
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
 * */
#include "status/icmp/icmp.hpp"
#include "agent-framework/module/common_components.hpp"
#include "discovery/common.hpp"
#include "net/ipaddress.hpp"
#include <safe-string/safe_lib.hpp>
extern "C" {
#include <arpa/inet.h>
#include <unistd.h>
}

using namespace logger_cpp;
using namespace agent::compute::status;
using namespace agent::compute::discovery;
using namespace agent_framework::generic;
using namespace agent_framework::model;
using namespace agent_framework::module;
using Status = agent_framework::status::ModuleStatus::Status;

// Number of pings per iteration.
static const int PING_COUNT = 5;

bool Icmp::ping() const {
    sockaddr conventional_address;
    sockaddr_in destination_address;
    int sock;
    bool is_online;
    icmp_packet_t packet;

    sock = create_socket_and_set_option(IPPROTO_ICMP);
    if (0 > sock) {
        throw std::runtime_error("Cannot create socket.");
    }

    try {
        destination_address = create_destination_address(m_ip_address);

        packet = create_ping_packet();

        if (EOK != memcpy_s(&conventional_address, sizeof(conventional_address),
                            &destination_address, rsize_t(sizeof(destination_address)))) {
            throw std::runtime_error("Cannot copy memory with IP addresses during conventional address creation.");
        }

        is_online = send_ping_receive_pong(sock, packet, &conventional_address);
    } catch (const std::runtime_error&) {
        close(sock); // Be sure to close Socket after exception.
        throw;
    }

    close(sock);

    return is_online;
}

int Icmp::create_socket_and_set_option(int protocol) const {
    int sock;
    struct timeval tv;

    // Sets timeout.
    tv.tv_sec = m_receive_pong_timeout_sec;
    tv.tv_usec = m_receive_pong_timeout_usec;

    if ((sock = socket(AF_INET, SOCK_RAW, protocol)) < 0) {
        if (EPERM == errno) {
            throw std::runtime_error("Permission denied: must be root.");
        }
        else {
            throw std::runtime_error("Cannot create socket.");
        }
    }

    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&tv), sizeof(struct timeval)) < 0) {
        close(sock);
        throw std::runtime_error("Cannot set socket options.");
    }

    return sock;
}

sockaddr_in Icmp::create_destination_address(const std::string& ip_address) const {
    struct sockaddr_in destination_address;

    memset(&destination_address, 0, sizeof(destination_address));

    destination_address.sin_family = AF_INET;
    destination_address.sin_port = static_cast<unsigned short>(0);
    destination_address.sin_addr = net::IpAddress::from_string(ip_address).ipv4_address();

    return destination_address;
}

Icmp::icmp_packet_t Icmp::create_ping_packet() const {
    icmp_packet_t packet;

    bzero(&packet, sizeof(packet));
    packet.type = ICMP_ECHO;
    packet.id = 1; // No matter.
    packet.sequence = 0; // no matter.
    packet.checksum = checksum(packet);

    return packet;
}

bool Icmp::send_ping_receive_pong(int sock, Icmp::icmp_packet_t& packet,
                                                  struct sockaddr* address) const {
    assert(nullptr != address);

    struct sockaddr_in socket;
    struct sockaddr dest_addr;
    struct sockaddr_in dest_socket;

    if (EOK != memcpy_s(&socket, sizeof(socket), address, rsize_t(sizeof(*address)))) {
        throw std::runtime_error("Cannot copy memory with IP addresses during PING sending.");
    }

    if (sendto(sock, &packet, sizeof(packet), 0, address, sizeof(*address)) <= 0) {
        throw std::runtime_error("Cannot call sendto().");
    }

    bool result = false;
    socklen_t size = sizeof(dest_addr);
    for (int i = 0; i < PING_COUNT; i++) {
        if (recvfrom(sock, &packet, sizeof(packet), 0, &dest_addr, &size) > 0) {
            if (EOK != memcpy_s(&dest_socket, sizeof(dest_socket),
                                &dest_addr, rsize_t(sizeof(dest_addr)))) {
                throw std::runtime_error("Cannot copy memory with IP addresses during PONG receiving.");
            }
            if (socket.sin_addr.s_addr == dest_socket.sin_addr.s_addr) {
                result = true;
                break;
            }
        }
        else {
            break;
        }
    }

    return result;
}

unsigned short Icmp::checksum(Icmp::icmp_packet_t& icmp_packet) const {
    unsigned int sum_16bit_chunks = calculate_sum_of_2byte_chunks(icmp_packet);
    unsigned int sum_with_carry = calculate_sum_with_carry(sum_16bit_chunks);
    unsigned short checksum_result = static_cast<unsigned short>(~sum_with_carry);

    return checksum_result;
}

unsigned int Icmp::calculate_sum_of_2byte_chunks(Icmp::icmp_packet_t& packet) const {
    size_t length_in_bytes = sizeof(packet);
    unsigned short* current_chunk_to_add = reinterpret_cast<unsigned short*>(&packet);
    unsigned int sum = 0;
    packet.checksum = 0; // According to RFC checksum is set to 0 at the beginning.

    // Shift +2 because we want to add 16bit chunks.
    for (size_t index = 0; index < length_in_bytes; index += 2) {
        sum += (*current_chunk_to_add);
        current_chunk_to_add++;
    }

    return sum;
}

unsigned int Icmp::calculate_sum_with_carry(unsigned int number) const {
    // Add first 4 bits to rest of the number.
    unsigned result = (number >> 16) + (number & 0xFFFF);

    // If addition generates new carry, add carry to result too.
    result += (result >> 16);

    return result;
}

