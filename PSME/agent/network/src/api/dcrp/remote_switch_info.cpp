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
 */

#include "api/dcrp/remote_switch_info.hpp"

#include "agent-framework/module-ref/network_manager.hpp"
#include "agent-framework/module-ref/model/remote_switch.hpp"


#include <safe-string/safe_lib.hpp>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <unistd.h>
#include <cstring>
#include <exception>
#include <sstream>
#include <vector>

#define ATTRIBUTE_PACKED __attribute__((packed))

struct switch_info {
    unsigned char mac[ETH_ALEN];
    struct in_addr ipv4addr;
    int32_t metric;
    uint32_t num_nexthop;
} ATTRIBUTE_PACKED;


using namespace agent::network::api::dcrp;
using namespace agent_framework::module;
using namespace agent_framework::model;
using std::vector;
using std::string;

RemoteSwitchInfo::RemoteSwitchInfo(const std::string& uuid) {
    auto network_manager = NetworkManager::get_instance();
    auto& neighbor_manager = network_manager->get_remote_switch_manager();
    neighbor_manager.get_entry(uuid);
}

RemoteSwitchInfo::~RemoteSwitchInfo() {}

void RemoteSwitchInfo::load_neighbor_switches_id(const string& parent_uuid) {
    Pointer buffer(nullptr);
    uint32_t bsize = 0;
    uint32_t count = 0;
    vector<string> new_neighbors;

    auto status = process_command("GetNodeNeighbors ", buffer, bsize);
    if (status) {
        throw std::runtime_error("GetNodeNeighbors command failed on remote side.");
    }
    if (sizeof(count) > bsize) {
        throw std::runtime_error("GetNodeNeighbors command result is unexpected");
    }
    auto ptr = buffer.get();
    memcpy_s(&count, sizeof(count), ptr, sizeof(count));
    ptr += sizeof(count);
    while (count-- > 0) {
        new_neighbors.push_back(ptr);
        ptr += strnlen_s(ptr, RSIZE_MAX_STR) + 1;
    }

    // based on the new neighbors list remove unknown switches from the model
    remove_unknown_neighbors(new_neighbors);

    for (const auto& neighbor : new_neighbors) {
        add_switch_to_neighbor_manager(parent_uuid, neighbor);
    }
}

string RemoteSwitchInfo::get_local_node_id() {
    Pointer buffer(nullptr);
    uint32_t bsize = 0;

    auto status = process_command("GetNodeId", buffer, bsize);
    if (status) {
        throw std::runtime_error("GetNodeId command failed.");
    }
    if (0 == bsize) {
        throw std::runtime_error("GetNodeId command result is unexpected");
    }
    return buffer.get();
}

namespace {
    void read_next_hops(const char* buffer, uint32_t bsize,
                        const switch_info& sinfo, RemoteSwitch& neighbor) {
        const char* ptr = buffer + sizeof(sinfo);
        for (uint32_t i = 0; i < sinfo.num_nexthop; i++) {
            // skip hop type (4 bytes), we don't need it
            ptr += sizeof(uint32_t);
            if (ptr >= buffer + bsize) {
                break;
            }
            // current DCRP implementation returns port only not supporting IPs
            neighbor.add_next_hop({static_cast<types::Number>(sinfo.metric),
                                  ptr, "", ""});
            ptr += strlen(ptr) + 1;
            if (ptr >= buffer + bsize) {
                break;
            }
        }
    }
}

void RemoteSwitchInfo::fill_response_with_switch_info(const string& id,
                                                      RemoteSwitch& neighbor) {
    Pointer buffer(nullptr);
    uint32_t bsize = 0;
    switch_info sinfo{};
    static const char CHARS_PER_OCTET = 3;
    const size_t MAC_ADDRESS_LENGTH = ETH_ALEN * CHARS_PER_OCTET;
    char mac_address[MAC_ADDRESS_LENGTH];

    auto status = process_command("GetNodeDetails " + id, buffer, bsize);
    if (status) {
        throw std::runtime_error("GetNodeDetails command failed on remote side.");
    }
    if (sizeof(switch_info) > bsize) {
        throw std::runtime_error("GetNodeDetails command result is unexpected");
    }
    memcpy_s(&sinfo, sizeof(sinfo), buffer.get(), sizeof(sinfo));

    int result = std::snprintf(mac_address, MAC_ADDRESS_LENGTH, "%02X:%02X:%02X:%02X:%02X:%02X",
                    sinfo.mac[0], sinfo.mac[1], sinfo.mac[2],
                    sinfo.mac[3], sinfo.mac[4], sinfo.mac[5]);
    mac_address[MAC_ADDRESS_LENGTH - 1] = '\0';
    if (result == MAC_ADDRESS_LENGTH - 1) {
        neighbor.set_mac_address(mac_address);
    }

    ::read_next_hops(buffer.get(), bsize, sinfo, neighbor);
}

void RemoteSwitchInfo::write_int(int sock_id, uint32_t value) {
    value = htonl(value);
    if (write(sock_id, &value, sizeof(value)) !=  sizeof(value)) {
        throw std::runtime_error("failed writing int to socket");
    }
}

void RemoteSwitchInfo::read_int(int sock, uint32_t& i) {
    if (read(sock, &i, sizeof(int)) != sizeof(int)) {
        throw std::runtime_error("unable to read from socket");
    }
    i = ntohl(i);
}

uint32_t RemoteSwitchInfo::process_command(const string& cmd, Pointer& buf,
                                           uint32_t& outsize)
{
    const static char* SERVER_IP = "127.0.0.1";
    const static int SERVER_PORT = 49152;
    const static int UNKNOWN_ERROR = 1;
    struct sockaddr_in serv_addr{};
    struct sockaddr addr{};
    uint32_t status = UNKNOWN_ERROR;
    uint32_t len;
    uint32_t cmd_len = uint32_t(cmd.length()) + 1;

    int sock_id = socket(AF_INET, SOCK_STREAM, 0);

    if (sock_id  < 0) {
        throw std::runtime_error("failed to open socket");
    }

    ScopedSocket ss(sock_id);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    serv_addr.sin_port = htons(SERVER_PORT);

    memcpy_s(&addr, sizeof(addr), &serv_addr, sizeof(serv_addr));
    if (connect(sock_id, &addr, sizeof(serv_addr))
        < 0) {
        throw std::runtime_error("failed to connect to server");
    }
    write_int(sock_id, cmd_len);
    if (write(sock_id, cmd.data(), cmd_len) != cmd_len) {
        throw std::runtime_error("failed to write command");
    }

    read_int(sock_id, len);

    if (len > sizeof(status)) {
        read_int(sock_id, status);
        len -= uint32_t(sizeof(status));
        if (len > 0) {
            Pointer p(new char[len]);
            if(read(sock_id, p.get(), len) != len) {
                throw std::runtime_error("failed to read result");
            }
            else {
                outsize = len;
                buf = std::move(p);
            }
        }
    }
    return status;
}

RemoteSwitchInfo::ScopedSocket::~ScopedSocket() {
    if (0 < m_sock) {
        close(m_sock);
    }
}

string RemoteSwitchInfo::get_neighbor_on_port(const string& switch_uuid,
                                              const string& port_identifier)
{
    // get all neighbors of our local switch
    auto network_manager = NetworkManager::get_instance();
    auto& neighbor_manager = network_manager->get_remote_switch_manager();
    load_neighbor_switches_id(switch_uuid);
    for (const auto& uuid : neighbor_manager.get_keys()) {
        auto neighbor = neighbor_manager.get_entry(uuid);
        fill_response_with_switch_info(neighbor.get_switch_identifier(),
                                       neighbor);
        for (const auto& hop  : neighbor.get_next_hop()) {
            if (hop.get_port_identifier() == port_identifier) {
                return neighbor.get_switch_identifier();
            }
        }
    }
    // no neighbor found on specified port
    return "";
}

void RemoteSwitchInfo::add_switch_to_neighbor_manager(const string& parent_uuid,
                                                      const string& identifier){
    auto network_manager = NetworkManager::get_instance();
    auto& neighbor_manager = network_manager->get_remote_switch_manager();

    bool found = false;
    for (const auto& uuid : neighbor_manager.get_keys()) {
        if (neighbor_manager.get_entry(uuid).get_switch_identifier() ==
            identifier) {
            found = true;
            break;
        }
    }
    if (!found) {
        RemoteSwitch neighbor(parent_uuid);
        neighbor.set_switch_identifier(identifier);
        neighbor_manager.add_entry(neighbor);
    }
}

void RemoteSwitchInfo::remove_unknown_neighbors(const vector<string>& new_neighbors) {
    auto network_manager = NetworkManager::get_instance();
    auto& neighbor_manager = network_manager->get_remote_switch_manager();
    vector<string> unknown_neighbors;

    for (const auto& uuid : neighbor_manager.get_keys()) {
        const auto& it = std::find(new_neighbors.begin(), new_neighbors.end(),
                             neighbor_manager.get_entry(uuid).get_switch_identifier());
        if (it == new_neighbors.end()) {
            unknown_neighbors.push_back(uuid);
        }
    }
    for (const auto& uuid : unknown_neighbors) {
        neighbor_manager.remove_entry(uuid);
    }
}
