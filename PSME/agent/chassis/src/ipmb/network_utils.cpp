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
 * */

#include "agent-framework/module/common_components.hpp"
#include "ipmb/network_utils.hpp"

#include <fstream>
#include <string>

extern "C" {
#include <unistd.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <netdb.h>
}

namespace agent {
namespace chassis {
namespace ipmb {
namespace network_utils {

using agent_framework::module::CommonComponents;


std::string get_network_interface() {
    auto drawer_manager_keys = CommonComponents::get_instance()->get_module_manager().get_keys("");
    if (!drawer_manager_keys.size()) {
        throw std::runtime_error("Cannot get chassis network interface name.");
    }

    auto chassis_keys = CommonComponents::get_instance()->
        get_chassis_manager().get_keys(drawer_manager_keys.front());

    auto chassis = CommonComponents::get_instance()->
        get_chassis_manager().get_entry_reference(chassis_keys.front());

    return chassis->get_network_interface();
}


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


json::Json parse_ini(std::istream& in_file) {
    json::Json parsed = json::Json();
    std::string section = "";
    std::string line{};

    while (getline(in_file, line)) {
        auto first = line.find_first_not_of(" \t");
        if (first == std::string::npos) {
            continue;
        }
        else if (first > 0) {
            line.erase(0, first);
        }

        /* parse section tag */
        if (line.at(0) == '[') {
            section = line.substr(1, line.length() - 2);
            parsed[section] = {};
            continue;
        }
        /* parse value tag */
        auto index = line.find('=');
        if (index == std::string::npos) {
            throw std::runtime_error{"Not a key=value: " + line};
        }
        if (section.empty()) {
            throw std::runtime_error{"Value not in any section: " + line};
        }
        parsed[section][line.substr(0, index)] = line.substr(index + 1);
    }
    return parsed;
}


namespace {

void save_values(std::ostream& out_file, const json::Json& section) {
    for (json::Json::const_iterator it = section.begin(); it != section.end(); it++) {
        if (it->is_object()) {
            throw std::runtime_error{"Subsection found: " + it.key()};
        }
        out_file << it.key() << "=" << it.value().get<std::string>() << std::endl;
    }
}

}

void save_ini(std::ostream& out_file, const json::Json& ini) {
    /* then store all key-value pairs from sections */
    bool add_empty_line = false;
    for (json::Json::const_iterator it = ini.begin(); it != ini.end(); it++) {
        if (!it->is_object()) {
            throw std::runtime_error{"Not a section: " + it.key()};
        }
        if (!add_empty_line) {
            add_empty_line = true;
        }
        else {
            out_file << std::endl;
        }
        out_file << "[" << it.key() << "]" << std::endl;
        save_values(out_file, it.value());
    }
}


namespace {

constexpr const char* const NETWORK_FILE_PREFIX = "/etc/systemd/network/";
constexpr const char* const NETWORK_FILE_SUFIX = ".network";

std::string make_network_file(const std::string& file) {
    return NETWORK_FILE_PREFIX + file + NETWORK_FILE_SUFIX;
}

json::Json parse_network_ini(const std::string& interface) {
    std::ifstream in_file{make_network_file(interface)};
    if (!in_file.is_open()) {
        throw std::runtime_error{"Cannot open interface " + interface + " file"};
    }

    json::Json parsed = parse_ini(in_file);
    if ((parsed.count("Network") != 0) && (parsed["Network"].count("Address") != 0)) {
        parsed["Address"]["Address"] = std::move(parsed["Network"]["Address"]);
        parsed["Network"].erase("Address");
    }
    return parsed;
}

void save_network_ini(const std::string& interface, const json::Json& ini) {
    /* check if given json is valid, will throw an exception if wrong */
    std::stringstream out_str{};
    save_ini(out_str, ini);

    /* file is created only if valid json passed! */
    std::ofstream out_file{make_network_file(interface)};
    if (!out_file.is_open()) {
        throw std::runtime_error{"Cannot open interface " + interface + " file"};
    }
    save_ini(out_file, ini);
}

int netmask_bits(const std::string& netmask) {
    struct in_addr addr;
    if (!inet_aton(netmask.c_str(), &addr)) {
        throw std::runtime_error{"Invalid netmask address"};
    }

    auto mask = (ntohl)(addr.s_addr);
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


SourceOption get_source(const std::string& interface) {
    json::Json parsed = parse_network_ini(interface);
    if ((parsed["Network"].count("DHCP") != 0) && (parsed["Network"]["DHCP"] == "yes")) {
        return SourceOption::DHCP;
    }
    else if (parsed["Address"].count("Address") != 0) {
        return SourceOption::STATIC;
    }
    else {
        return SourceOption::NONE;
    }
}


void set_ip(const std::string& interface, const std::string& ip) {
    json::Json ini = parse_network_ini(interface);
    std::string mask = "/24";
    if (ini["Address"].count("Address") != 0) {
        mask = get_mask_part(ini["Address"]["Address"]);
    }
    ini["Address"]["Address"] = ip + mask;
    save_network_ini(ini, interface);
}


void set_mask(const std::string& interface, const std::string& mask) {
    json::Json ini = parse_network_ini(interface);
    if (ini["Address"].count("Address") == 0) {
        throw std::runtime_error{"Address field not set!"};
    }
    std::string ip = get_ip_part(ini["Address"]["Address"]);
    ini["Address"]["Address"] = ip + "/" + std::to_string(netmask_bits(mask));
    save_network_ini(ini, interface);
}


void set_source(const std::string& interface, const SourceOption option) {
    json::Json ini = parse_network_ini(interface);
    switch (option) {
        case SourceOption::STATIC:
            /* IP must be set before */
            ini["Network"]["DHCP"] = "no";
            break;
        case SourceOption::DHCP:
            ini["Network"]["DHCP"] = "yes";
            break;
        case SourceOption::NONE:
            throw std::runtime_error{"Cannot disable source for " + interface};
        default:
            throw std::runtime_error{"Unhandled opiton for " + interface};
    }
    save_network_ini(ini, interface);
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

