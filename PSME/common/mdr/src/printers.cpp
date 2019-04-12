/*!
 * @brief Simple printers for commonly used types.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file printers.cpp
 */

#include "mdr/printers.hpp"

#include <arpa/inet.h>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <safe-string/safe_lib.hpp>

namespace mdr {

std::string print_ip(const uint8_t* address) {
    if (nullptr == address) {
        return "";
    }

    char str[INET6_ADDRSTRLEN];
    std::stringstream ss;

    // try IPv4 first
    in_addr ip4;
    ::memcpy_s(&ip4, sizeof(ip4), address, sizeof(ip4));
    if (str == inet_ntop(AF_INET, &ip4, str, INET_ADDRSTRLEN)) {
            ss << str;
            return ss.str();
    }

    // try IPv6
    in6_addr ip6;
    ::memcpy_s(&ip6, sizeof(ip6), address, sizeof(ip6));
    if (str == inet_ntop(AF_INET6, &ip6, str, INET6_ADDRSTRLEN)) {
            ss << str;
            return ss.str();
    }

    // inet_ntop failed
    ss << "Invalid IP Address";

    return ss.str();
}

std::string print_mac(const uint8_t* address) {
    if (nullptr == address) {
        return "";
    }

    std::stringstream ss;
    ss << std::hex << std::setw(2) << std::setfill('0')
       << static_cast<int>(address[0]) << ":" << std::setw(2)
       << static_cast<int>(address[1]) << ":" << std::setw(2)
       << static_cast<int>(address[2]) << ":" << std::setw(2)
       << static_cast<int>(address[3]) << ":" << std::setw(2)
       << static_cast<int>(address[4]) << ":" << std::setw(2)
       << static_cast<int>(address[5]);
    return ss.str();
}

}  // namespace mdr
