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
 * */

#include "agent-framework/module-ref/chassis_manager.hpp"
#include "ipmb/command/ini-parser/INI.h"
#include "ipmb/network_utils.hpp"

extern "C" {
#include <unistd.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <netdb.h>
}

using ChassisComponents = agent_framework::module::ChassisManager;

namespace agent {
namespace chassis {
namespace ipmb {
namespace network_utils {

using namespace agent::chassis::ipmb::network_utils;

std::string get_ip(const std::string& interface) {
    struct ifaddrs* ifaddr{nullptr};
    char host[NI_MAXHOST] = {0};
    bool found = false;

    if (0 != getifaddrs(&ifaddr)) {
        throw std::runtime_error{"Error getifaddrs"};
    }

    for (auto* ifa = ifaddr; ifa; ifa = ifa->ifa_next) {
        auto family = ifa->ifa_addr->sa_family;
        if (AF_INET == family || AF_INET6 == family) {
            if (0 != interface.compare(ifa->ifa_name)) {
               continue;
            }
            auto socket_len = (family == AF_INET) ?
                                                sizeof(struct sockaddr_in) :
                                                sizeof(struct sockaddr_in6);
            auto rc = getnameinfo(ifa->ifa_addr,
                                    static_cast<std::uint32_t>(socket_len),
                                    host, NI_MAXHOST,
                                    nullptr, 0, NI_NUMERICHOST);
            if (0 != rc) {
                throw std::runtime_error("Cannot get network address!");
            }

            found = true;
            break;
        }
    }
    if (!found) {
        throw std::runtime_error("Network interface not found!");
    }

    freeifaddrs(ifaddr);

    return std::string{host};
}

std::string get_network_interface() {
    auto drawer_manager_keys = ChassisComponents::get_instance()->get_module_manager().get_keys("");
    if (!drawer_manager_keys.size()) {
        throw std::runtime_error("Cannot get chassis network interface name.");
    }

    auto chassis_keys = ChassisComponents::get_instance()->
            get_chassis_manager().get_keys(drawer_manager_keys.front());

    auto chassis = ChassisComponents::get_instance()->
            get_chassis_manager().get_entry_reference(chassis_keys.front());

    return chassis->get_network_interface();
}

namespace {
constexpr const char* const NETWORK_FILE_PREFIX = "/etc/systemd/network/";
constexpr const char* const NETWORK_FILE_SUFIX = ".network";

std::string make_network_file(const std::string& file) {
    return NETWORK_FILE_PREFIX + file + NETWORK_FILE_SUFIX;
}

bool  is_start_with(const std::string& str, const std::string& substring) {
    return !str.compare(0, substring.size(), substring);
}
}

SourceOption get_source(const std::string& interface) {
    std::ifstream in_file{};
    in_file.open(make_network_file(interface));

    if (!in_file.is_open()) {
        throw std::runtime_error{"Cannot open interface file " + interface};
    }

    std::string line{};
    std::string DHCP_OPTION = "DHCP=";
    std::string ADDRESS_OPTION = "Address=";

    bool address_found = false;
    bool dhcp_found = false;
    while (getline(in_file, line)) {
        if (is_start_with(line, DHCP_OPTION) &&
                            line.compare(DHCP_OPTION.size(), 2, "no")) {
           dhcp_found = true;
        }

        if (is_start_with(line, ADDRESS_OPTION)) {
            address_found = true;
            break;
        }
    }

    in_file.close();

    return address_found ? SourceOption::STATIC :
           dhcp_found    ? SourceOption::DHCP :
                           SourceOption::NONE;
}

std::string get_mask(const std::string& interface) {
    struct ifaddrs* ifaddr{nullptr};
    char host[NI_MAXHOST] = {0};
    bool found = false;

    if (0 != getifaddrs(&ifaddr)) {
        throw std::runtime_error{"Error getifaddrs"};
    }

    for (auto* ifa = ifaddr; ifa; ifa = ifa->ifa_next) {
        auto family = ifa->ifa_addr->sa_family;
        if (AF_INET == family || AF_INET6 == family) {
            if (0 != interface.compare(ifa->ifa_name)) {
               continue;
            }
            auto rc = getnameinfo(ifa->ifa_netmask,
                                    sizeof(struct sockaddr_in),
                                    host, NI_MAXHOST,
                                    nullptr, 0, NI_NUMERICHOST);
            if (0 != rc) {
                throw std::runtime_error("Cannot get network address!");
            }
            found = true;
            break;
        }
    }
    if (!found) {
        throw std::runtime_error("Network interface not found!");
    }

    freeifaddrs(ifaddr);

    return std::string{host};
}

namespace {

int netmask_bits(const std::string& netmask) {
    struct in_addr addr;
    if (!inet_aton(netmask.c_str(), &addr)) {
        throw std::runtime_error{"Invalid netmask address"};
    }

    auto mask = ntohl(addr.s_addr);
    auto maskbits = 32;
    for (; (mask & (1L << (32 - maskbits)))== 0; maskbits--);

    return maskbits;
}

std::string get_ip_part(const std::string& str) {
    return str.substr(0, str.find("/"));
}

std::string get_mask_part(const std::string& str) {
    return str.substr(str.find("/") + 1, str.size());
}
}

void set_ip(const std::string& interface, const std::string& ip) {
    INIConfig ini(make_network_file(interface), true);

    auto address = ini.get("Address", "");

    if (address.empty()) {
        ini["Address"]["Address"] = ip + "/24";
    } else {
        ini["Address"]["Address"] = ip + "/" + get_mask_part(address);
    }

    ini.save();
    ini.clear();
}

void set_mask(const std::string& interface, const std::string& mask) {
    INIConfig ini(make_network_file(interface), true);
    ini.select("Address");
    auto address = ini.get("Address", "");

    if (address.empty()) {
        throw std::runtime_error{"Address field not set!"};
    }

    ini["Address"]["Address"] = get_ip_part(address)
                                + "/" + std::to_string(netmask_bits(mask));
    ini.save();
    ini.clear();
}

void set_source(const std::string& interface, const SourceOption option) {
    INIConfig ini(make_network_file(interface), true);

    if (SourceOption::STATIC == option) {
        ini["Network"]["DHCP"] = "no";
    }

    if (SourceOption::DHCP == option) {
        ini["Network"]["DHCP"] = "yes";
    }

    ini.save();
    ini.clear();
}

NetworkBytes addr_to_bytes(const std::string& addr) {

    std::uint32_t ip = 0;
    if (1 != inet_pton(AF_INET, addr.c_str(), &ip)) {
        throw std::invalid_argument(addr);
    }

    NetworkBytes bytes = {
            uint8_t(ip & 0xFF),
            uint8_t(ip >> 8 & 0xFF),
            uint8_t(ip >> 16 & 0xFF),
            uint8_t(ip >> 24 & 0xFF),
    } ;

    return bytes;
}

std::string bytes_to_addr(const NetworkBytes& bytes) {
    return std::to_string(static_cast<unsigned>(bytes[0])) + "." +
           std::to_string(static_cast<unsigned>(bytes[1])) + "." +
           std::to_string(static_cast<unsigned>(bytes[2])) + "." +
           std::to_string(static_cast<unsigned>(bytes[3]));
}

}
}
}
}

