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
 *
 * @section DESCRIPTION
 *
 * @file ethtool.hpp
 *
 * @brief Ethtool class.
 * */

#include "api/netlink/ethtool.hpp"
#include "netlink/nl_exception_invalid_input.hpp"

#include <safe-string/safe_lib.hpp>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/netdevice.h>
#include <linux/sockios.h>
#include <linux/ethtool.h>
#include <unistd.h>

#include <string>
#include <cstring>
#include <sstream>
#include <stdexcept>

using namespace agent::network::api::netlink;

Ethtool::Ethtool(const IfName& ifname) : m_sock{-1}, m_ifname{ifname} {
    /* open socket */
    m_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_sock < 0) {
        throw std::runtime_error("Ethtool() failed to create socket");
    }
}

Ethtool::~Ethtool() {
    if (m_sock >= 0) {
        close(m_sock);
        m_sock = -1;
    }
}

void Ethtool::get_settings(struct ethtool_cmd& edata) {
    struct ifreq ifr;

    /* zeroing the memomry */
    memset(&ifr, 0, sizeof(ifr));

    /* setup interface info */
    strncpy_s(ifr.ifr_name, sizeof(ifr.ifr_name), m_ifname.c_str(), m_ifname.size());
    ifr.ifr_data = &edata;

    /* get ethtool settings */
    edata.cmd = ETHTOOL_GSET;
    if (ioctl(m_sock, SIOCETHTOOL, &ifr)) {
        throw std::runtime_error(std::string("get_settings ioctl(): ")
                                 + std::string(strerror(errno))
                                 + " [ ifname=" + m_ifname + "]");
    }
}

void Ethtool::set_settings(struct ethtool_cmd& edata) {
    struct ifreq ifr;

    /* zeroing the memomry */
    memset(&ifr, 0, sizeof(ifr));

    /* setup interface info */
    strncpy_s(ifr.ifr_name, sizeof(ifr.ifr_name), m_ifname.c_str(), m_ifname.size());
    ifr.ifr_data = &edata;

    /* get ethtool settings */
    edata.cmd = ETHTOOL_SSET;
    if (ioctl(m_sock, SIOCETHTOOL, &ifr)) {
        switch (errno) {
            case EINVAL:
                throw netlink_base::NlExceptionInvalidInput();
            default:
                break;
        }
        throw std::runtime_error(std::string("set_settings ioctl(): ")
                                 + std::string(strerror(errno))
                                 + " [ ifname=" + m_ifname + ", speed="
                                 + std::to_string(ethtool_cmd_speed(&edata))
                                 + " ]");
    }
}

void Ethtool::set_speed(uint32_t speed) {
    struct ethtool_cmd edata;
    memset(&edata, 0, sizeof(edata));
    get_settings(edata);
    ethtool_cmd_speed_set(&edata, speed);
    set_settings(edata);
}

uint32_t Ethtool::get_speed() {
    struct ethtool_cmd edata;
    memset(&edata, 0, sizeof(edata));
    get_settings(edata);
    return ethtool_cmd_speed(&edata);
}
