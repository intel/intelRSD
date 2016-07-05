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
 * */
#include "status/module_software_status.hpp"

#include <safe-string/safe_lib.hpp>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace agent::compute::status;
using namespace logger_cpp;

// @TODO Number of pings per iteartion.
static const int PING_COUNT = 5;

ModuleSoftwareStatus::~ModuleSoftwareStatus() {}

agent_framework::status::ModuleStatus::Status ModuleSoftwareStatus::read_status() {

    agent_framework::status::ModuleStatus::Status
        status = ModuleStatus::Status::UNKNOWN;

    try {
        bool is_pong = ping();

        if(is_pong) {
            status = ModuleStatus::Status::PRESENT;
        } else {
            status = ModuleStatus::Status::NOT_PRESENT;
        }
    }
    catch(std::runtime_error & error) {
        log_warning(GET_LOGGER("status"), "Cannot ping module: " << error.what() << std::endl);
        status = ModuleStatus::Status::UNKNOWN;
    }

    m_status = status;

    return m_status;
}

bool ModuleSoftwareStatus::ping() const {

    struct hostent * hostname = nullptr;
    struct protoent *protocol = nullptr;
    struct sockaddr conventional_address;
    struct sockaddr_in destination_address;
    int sock;
    bool is_online;
    icmp_packet_t packet;

    protocol = getprotobyname("icmp");
    if (nullptr == protocol) {
        throw std::runtime_error("cannot get protocol") ;
    }

    sock = create_socket_and_set_option(protocol->p_proto);
    if (0 > sock) {
        throw std::runtime_error("cannot create socket") ;
    }

    hostname = gethostbyname(m_ip_address.c_str());
    if (nullptr == hostname) {
        close(sock);
        throw std::runtime_error("cannot get hostname") ;
    }

    destination_address = create_destination_address(*hostname);

    packet = create_ping_packet();

    if (EOK != memcpy_s(&conventional_address, sizeof(conventional_address),
                        &destination_address, int(sizeof(destination_address)))) {
        close(sock);
        throw std::runtime_error("cannot copy dimm");
    }

    is_online = send_ping_receive_pong(sock, packet, &conventional_address);
    close(sock);

    return is_online;
}

int ModuleSoftwareStatus::create_socket_and_set_option(int protocol) const {
    int sock = -1;
    struct timeval tv;

    // Sets timeout.
    tv.tv_sec  = m_receive_pong_timeout_sec;
    tv.tv_usec = m_receive_pong_timeout_usec;

    if ((sock = socket(AF_INET, SOCK_RAW, protocol)) < 0) {
        if (EPERM == errno) {
            throw std::runtime_error("permission denied: must be root") ;
        }
        else {
            throw std::runtime_error("cannot create socket.") ;
        }
    }
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&tv),sizeof(struct timeval)) < 0) {
        close(sock);
        throw std::runtime_error("cannot set socket options.");
    }

    return sock;
}

sockaddr_in ModuleSoftwareStatus::create_destination_address(struct hostent& hostname) const{
    struct sockaddr_in destination_address;

    memset(&destination_address, 0, sizeof(destination_address));

    destination_address.sin_family = AF_INET;
    destination_address.sin_port = static_cast<unsigned short>(0);
    if (EOK != memcpy_s(&destination_address.sin_addr,
                        sizeof(destination_address.sin_addr),
                        hostname.h_addr, size_t(hostname.h_length))) {
        throw std::runtime_error("cannot copy dimm");
    }

    return destination_address;

}

ModuleSoftwareStatus::icmp_packet_t ModuleSoftwareStatus::create_ping_packet() const {

    icmp_packet_t packet;
    bzero(&packet, sizeof(packet));
    packet.type = ICMP_ECHO;
    packet.id = 1; // No matter.
    packet.sequence = 0; // no matter.
    packet.checksum = checksum(packet);
    return packet;
}

struct sockaddr * ModuleSoftwareStatus::convert_address(struct sockaddr_in * input_address) const {
    assert(nullptr != input_address);
    return reinterpret_cast<struct sockaddr *>(input_address);
}

bool ModuleSoftwareStatus::send_ping_receive_pong(int sock,
                                                  ModuleSoftwareStatus::icmp_packet_t & packet,
                                                  struct sockaddr * address) const {
    assert(nullptr != address);
    struct sockaddr_in socket;
    struct sockaddr dest_addr;
    struct sockaddr_in dest_socket;

    if (EOK != memcpy_s(&socket, sizeof(socket), address, int(sizeof(*address)))) {
        throw std::runtime_error("cannot copy dimm");
    }

    if (sendto(sock, &packet, sizeof(packet), 0, address, sizeof(*address)) <= 0 ) {
        throw std::runtime_error("cannot sendto data.") ;
    }

    bool result = false;
    socklen_t size = sizeof(dest_addr);
    for(int i = 0; i < PING_COUNT; i++) {
        if(recvfrom(sock, &packet, sizeof(packet), 0, &dest_addr, &size) > 0) {
            if (EOK != memcpy_s(&dest_socket, sizeof(dest_socket),
                                &dest_addr, int(sizeof(dest_addr)))) {
                throw std::runtime_error("cannot copy dimm");
            }
            if (socket.sin_addr.s_addr == dest_socket.sin_addr.s_addr){
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

unsigned short ModuleSoftwareStatus::checksum(ModuleSoftwareStatus::icmp_packet_t & icmp_packet) const {
    unsigned int sum_16bit_chunks = calculate_sum_of_2byte_chunks(icmp_packet);
    unsigned int sum_with_carry = calculate_sum_with_carry(sum_16bit_chunks);
    unsigned short checksum_result = static_cast<unsigned short>(~sum_with_carry);

    return checksum_result;
}

unsigned int ModuleSoftwareStatus::calculate_sum_of_2byte_chunks(ModuleSoftwareStatus::icmp_packet_t & packet) const {

    size_t length_in_bytes = sizeof(packet);
    unsigned short * curent_chunk_to_add = reinterpret_cast<unsigned short*>(&packet);
    unsigned int sum = 0;
    packet.checksum = 0; // According to RFC checksum is set to 0 at the begining.

    // Shift +2 because we want to add 16bit chunks.
    for(size_t index = 0; index < length_in_bytes ; index += 2) {
        sum += (*curent_chunk_to_add);
        curent_chunk_to_add++;
    }

    return sum;
}

unsigned int ModuleSoftwareStatus::calculate_sum_with_carry(unsigned int number) const {

    // Add first 4 bits to rest of the number.
    unsigned result = (number >> 16) + (number & 0xFFFF);\

    // If addition generates new carry, add carry to result too.
    result += (result >> 16);

    return result;
}
