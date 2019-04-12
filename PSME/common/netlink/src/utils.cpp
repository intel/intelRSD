/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file utils.cpp
 *
 * @brief Netlink utilities.
 * */

#include "netlink/utils.hpp"
#include "netlink/fdb_info_message.hpp"
#include "netlink/nl_socket.hpp"

#include <netlink/route/rtnl.h>
#include <netlink/route/link.h>
#include <netlink/route/addr.h>
#include <arpa/inet.h>


using namespace netlink_base;
using namespace std;

string netlink_base::utils::get_neighbor_mac_address(const string& port_identifier) {
    FdbInfoMessage msg{port_identifier};
    string address{};

    msg.send();
    if (!msg.get_entries().empty()) {
        address = msg.get_entries()[0].get_mac();
    }
    return address;
}

string netlink_base::utils::get_port_mac_address(const string& port_identifier) {
    NlSocket sock(NlSocket::Protocol::ROUTE);
    struct rtnl_link* link{nullptr};
    string ret{};

    if (rtnl_link_get_kernel(sock.get_sock(), 0, port_identifier.c_str(), &link) == 0) {
        struct nl_addr* addr = rtnl_link_get_addr(link);
        if (addr) {
            static const size_t MAC_ADDRESS_LENGTH = 20;
            char buf[MAC_ADDRESS_LENGTH];
            nl_addr2str(addr, buf, MAC_ADDRESS_LENGTH);
            ret = buf;
        }
        rtnl_link_put(link);
    }
    return ret;
}

string netlink_base::utils::get_port_ip_address(int ifindex, string& mask) {
    NlSocket sock(NlSocket::Protocol::ROUTE);
    struct nl_cache* ip_address_cache{nullptr};
    struct nl_object* ip_address_cache_entry{nullptr};
    string ret{};

    if (0 != rtnl_addr_alloc_cache(sock.get_sock(), &ip_address_cache)) {
        // do not throw exception, we don't want to break GET command
        // just because of not having IP address, which is optional.
        // simply return empty address
        return {};
    }
    ip_address_cache_entry = nl_cache_get_first(ip_address_cache);
    while (ip_address_cache_entry) {
        struct rtnl_addr* addr{nullptr};
        addr = reinterpret_cast<struct rtnl_addr*>(ip_address_cache_entry);
        if (rtnl_addr_get_ifindex(addr) == ifindex &&
            AF_INET == rtnl_addr_get_family(addr)) {
            struct nl_addr* la = rtnl_addr_get_local(addr);
            if (la) {
                const int MAX_ADDR_LEN = 20;
                char address_str[MAX_ADDR_LEN]{};
                if (nullptr == inet_ntop(AF_INET, nl_addr_get_binary_addr(la),
                                         address_str, MAX_ADDR_LEN)) {
                    break;
                }
                ret = address_str;
                struct in_addr ia{htonl(~(0xffffffff >>
                                          nl_addr_get_prefixlen(la)))};
                if (nullptr != inet_ntop(AF_INET, &ia, address_str, MAX_ADDR_LEN)) {
                    mask = address_str;
                }
            }
            break;
        }
        ip_address_cache_entry = nl_cache_get_next(ip_address_cache_entry);
    }
    if (ip_address_cache) {
        nl_cache_free(ip_address_cache);
    }
    return ret;
}
