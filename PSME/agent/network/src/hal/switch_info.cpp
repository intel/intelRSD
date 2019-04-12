/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * @file switch_info.cpp
 *
 * @brief Switch parameters
 * */

#include "hal/switch_info.hpp"

#include <safe-string/safe_lib.hpp>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <memory>
#include <agent-framework/exceptions/exception.hpp>


using namespace agent::network::hal;

SwitchInfo::~SwitchInfo() {}

string SwitchInfo::format_mac_address (const uint8_t* addr) {
    static const char CHARS_PER_OCTET = 3;
    const size_t MAC_ADDRESS_LENGTH = ETH_ALEN * CHARS_PER_OCTET;
    char mac_address[MAC_ADDRESS_LENGTH];
    int result = std::snprintf(mac_address, MAC_ADDRESS_LENGTH, "%02X:%02X:%02X:%02X:%02X:%02X",
                    addr[0], addr[1], addr[2],
                    addr[3], addr[4], addr[5]);
    mac_address[MAC_ADDRESS_LENGTH - 1] = '\0';
    if (result == MAC_ADDRESS_LENGTH - 1) {
        return string(mac_address);
    }
    return string();
}

void SwitchInfo::read_mgmt_mac_address() {
    struct ifreq ifr;
    bool success;

    /* zeroing the memomry */
    memset(&ifr, 0, sizeof(ifr));
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (0 > sock) {
        THROW(agent_framework::exceptions::NetworkError,
              "network-agent", "Read switch info: failed to create socket");
    }

    // Get MAC address of management port
    const auto& mgmt_port = get_management_port();
    strncpy_s(ifr.ifr_name, sizeof(ifr.ifr_name), mgmt_port.c_str(),
                                                        mgmt_port.length());
    success = (0 == ioctl(sock, SIOCGIFHWADDR, &ifr));
    close(sock);

    if (success) {
        uint8_t mac[ETH_ALEN];
        memcpy_s(mac, ETH_ALEN, ifr.ifr_hwaddr.sa_data, sizeof(mac));
        set_mac_address(mac);
    }
    else {
        THROW(agent_framework::exceptions::NetworkError,
              "network-agent", "Unable to get switch MAC address");
    }
}

void SwitchInfo::set_mac_address(const uint8_t* addr) {
    memcpy_s(m_mac_address, sizeof(m_mac_address), addr, ETH_ALEN);
}
