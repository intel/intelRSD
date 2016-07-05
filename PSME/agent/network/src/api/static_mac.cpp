/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016 Intel Corporation
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

#include "api/static_mac.hpp"
#include "api/netlink/static_mac_message.hpp"
#include "netlink/socket.hpp"
#include "logger/logger_factory.hpp"

#include <netinet/ether.h>

using namespace agent::network::api;
using namespace agent::network::api::netlink;
using namespace netlink_base;
using namespace std;

StaticMac::StaticMac() {}
StaticMac::~StaticMac() {}

void StaticMac::add(const string& port, const string& address,
                    uint16_t vlan_id) {
    StaticMacMessage msg(true, port, address, vlan_id);
    Socket socket{};
    socket.connect();
    socket.send_message(msg);
    socket.receive_message(msg);
}

void StaticMac::remove(const string& port, const string& address,
                       uint16_t vlan_id) {
    StaticMacMessage msg(false, port, address, vlan_id);
    Socket socket{};
    socket.connect();
    socket.send_message(msg);
    socket.receive_message(msg);
}

namespace {
    struct Arg {
        Arg(netlink_base::Message& msg, StaticMac::Table& table) :
            message(msg), macs(table) {}
        netlink_base::Message& message;
        StaticMac::Table& macs;
    };

    static inline string get_mac_address(const ether_addr* addr) {
        char buffer[18];
        sprintf (buffer, "%02X:%02X:%02X:%02X:%02X:%02X",
                 addr->ether_addr_octet[0], addr->ether_addr_octet[1],
                 addr->ether_addr_octet[2], addr->ether_addr_octet[3],
                 addr->ether_addr_octet[4], addr->ether_addr_octet[5]);
        string ret{buffer};
        return ret;
    }

    void get_neighbour(const rtattr* const* attr, StaticMac::Table& macs) {
        if (attr[NDA_LLADDR]) {
            string mac = get_mac_address(static_cast<const ether_addr*>
                                         (RTA_DATA(attr[NDA_LLADDR])));
            uint16_t vlan_id{0};
            if (attr[NDA_VLAN]) {
                vlan_id = *(static_cast<uint16_t*>
                            (RTA_DATA(attr[NDA_VLAN])));
            }
            macs.insert(make_pair(mac, vlan_id));
        }
    }

    int neighbours_callback(nl_msg* nlmsg, void* ptr) {
        if (nullptr == nlmsg) {
            return NL_OK;
        }
        Arg* arg = static_cast<Arg*>(ptr);
        if (nullptr == arg) {
            return NL_OK;
        }
        nlmsghdr* hdr = nlmsg_hdr(nlmsg);
        ndmsg* data = static_cast<ndmsg*> (NLMSG_DATA(hdr));
        if (hdr->nlmsg_type == NLMSG_DONE) {
            arg->message.set_last();
            return NL_OK;
        }
        if (0 == (data->ndm_state & NUD_PERMANENT)) {
            //non-static entry, skip it
            return NL_OK;
        }
        rtattr *attr[NDA_MAX + 1]{};
        Message::parse_attr(attr, NDA_MAX, (rtattr*)
                            ((char*)(data) + NLMSG_ALIGN(sizeof(ndmsg))),
                            (int)hdr->nlmsg_len -
                            (int)NLMSG_LENGTH(sizeof(*data)));
        get_neighbour(attr, arg->macs);
        return NL_OK;
    }
}

void StaticMac::get_macs(const string& port, Table& macs) {
    Socket socket{};
    Message message{port};
    socket.connect();
    message.set_type(RTM_GETNEIGH);
    message.set_flags(NLM_F_DUMP | NLM_F_REQUEST);
    Arg arg(message, macs);
    socket.modify_callback(::neighbours_callback, &arg);
    socket.send_rtnl_message(message);
    socket.receive_to_callback(message);
}
