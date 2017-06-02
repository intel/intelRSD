/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016-2017 Intel Corporation
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
 * @file acl_message.cpp
 *
 * @brief ACL netlink message class
 *
 * */

/* Internal headers */
#include "api/netlink/acl_message.hpp"
#include "netlink/nl_exception.hpp"

#include <functional>

extern "C" {
#include <acl_nl_cmds.h>
#include <net/if.h>
}

using namespace netlink_base;
using namespace agent::network::api::netlink;
using namespace std;


namespace {
    template <class T>
    static void for_each_nested(const nlattr* nla, T& message,
                                std::function<void(const nlattr*, T&)>
                                function) {
        nlattr* pos{};
        int rem{};
        for (pos = static_cast<nlattr*>(nla_data(nla)), rem = nla_len(nla);
             nla_ok(pos, rem); pos = nla_next(pos, &(rem))) {
            function(pos, message);
        }
    }
    static inline void get_port_list_if_exists(const nlattr* nla,
                                               AclMessage& message) {
        for_each_nested<AclMessage>(nla, message,
                        [] (const nlattr* pos, AclMessage& msg) {
            int ifindex = nla_get_u32(pos);
            if (ifindex) {
                char ifname[IFNAMSIZ + 1] = {};
                if (if_indextoname(ifindex, ifname)) {
                    msg.add_bound_port(ifname);
                }
            }
        }
        );
    }

    static uint16_t get_action(const nlattr* const* attrs) {
        if (attrs[ACL_ATTR_ACTION_PERM]) {
            return ACL_ATTR_ACTION_PERM;
        }
        if (attrs[ACL_ATTR_ACTION_DENY]) {
            return ACL_ATTR_ACTION_DENY;
        }
        if (attrs[ACL_ATTR_ACTION_MIRROR_PORT]) {
            return ACL_ATTR_ACTION_MIRROR_PORT;
        }
        if (attrs[ACL_ATTR_ACTION_FORWARD]) {
            return ACL_ATTR_ACTION_FORWARD;
        }
        // no action defined, report error
        throw runtime_error("ACL rule action not defined");
    }

    static uint16_t get_rule_id(const nlattr* const* attrs) {
        if (attrs[ACL_ATTR_RULE_ID]) {
            return nla_get_u16(attrs[ACL_ATTR_RULE_ID]);
        }
        // no id defined, report error
        throw runtime_error("ACL rule id not defined");

    }

    static void get_mirror_type(const nlattr* const* attrs,
                                uint16_t& mirror_type) {
        if (attrs[ACL_ATTR_ACTION_MIRROR_TYPE]) {
            mirror_type = nla_get_u16(attrs[ACL_ATTR_ACTION_MIRROR_TYPE]);
        }
    }

    static void get_mirrored_port(const nlattr* nested,
                                  std::vector<uint32_t>& ports) {
        auto ifindex = nla_get_u32(nested);
        if (ifindex) {
            ports.push_back(ifindex);
        }
    }

    static void get_mirrored_ports(const nlattr* const* attrs,
                                   std::vector<uint32_t>& mirrored_ports) {
        if (!attrs[ACL_ATTR_PORT_LIST]) {
            return;
        }
        for_each_nested< std::vector<uint32_t> >(attrs[ACL_ATTR_PORT_LIST],
                                                 mirrored_ports,
                                                 get_mirrored_port);
    }

    static void get_mirror_port(const nlattr* const* attrs,
                                uint16_t& mirror_port) {
        if (attrs[ACL_ATTR_ACTION_MIRROR_PORT]) {
            mirror_port = nla_get_u16(attrs[ACL_ATTR_ACTION_MIRROR_PORT]);
        }
    }

    static void get_forward_port(const nlattr* const* attrs,
                                 uint32_t& forward_port) {
        if (attrs[ACL_ATTR_ACTION_FORWARD]) {
            forward_port = nla_get_u32(attrs[ACL_ATTR_ACTION_FORWARD]);
        }
    }

    static void get_condition_type(const nlattr* nested, uint32_t& type) {
        switch(nla_type(nested)) {
            case ACL_ATTR_COND_IP_SRC:
            case ACL_ATTR_COND_IP_DEST:
            case ACL_ATTR_COND_MAC_SRC:
            case ACL_ATTR_COND_MAC_DEST:
            case ACL_ATTR_COND_L4_PORT_SRC:
            case ACL_ATTR_COND_L4_PORT_DEST:
            case ACL_ATTR_COND_VLAN:
            case ACL_ATTR_COND_PROTOCOL:
                type = nla_type(nested);
                break;
            default:
                break;
        }
    }

    static void get_condition_value(const nlattr* nested, uint64_t& value) {
        switch(nla_type(nested)) {
            case ACL_ATTR_COND_IP_SRC:
            case ACL_ATTR_COND_IP_DEST:
                value = nla_get_u32(nested);
                break;
            case ACL_ATTR_COND_MAC_SRC:
            case ACL_ATTR_COND_MAC_DEST:
                value = nla_get_u64(nested);
                break;
            case ACL_ATTR_COND_L4_PORT_SRC:
            case ACL_ATTR_COND_L4_PORT_DEST:
            case ACL_ATTR_COND_VLAN:
                value = nla_get_u16(nested);
                break;
            case ACL_ATTR_COND_PROTOCOL:
                value = nla_get_u8(nested);
                break;
            default:
                break;
        }
    }

    static void get_condition_mask(const nlattr* nested, uint64_t& mask,
                                   bool& mask_set) {
        switch(nla_type(nested)) {
            case ACL_ATTR_COND_IP_SRC_MASK:
            case ACL_ATTR_COND_IP_DEST_MASK:
                mask = nla_get_u32(nested);
                mask_set = true;
                break;
            case ACL_ATTR_COND_MAC_SRC_MASK:
            case ACL_ATTR_COND_MAC_DEST_MASK:
                mask = nla_get_u64(nested);
                mask_set = true;
                break;
            case ACL_ATTR_COND_L4_PORT_SRC_MASK:
            case ACL_ATTR_COND_L4_PORT_DEST_MASK:
            case ACL_ATTR_COND_VLAN_MASK:
                mask = nla_get_u16(nested);
                mask_set = true;
                break;
            case ACL_ATTR_COND_PROTOCOL_MASK:
                mask = nla_get_u8(nested);
                mask_set = true;
                break;
            default:
                break;
        }
    }

    using Conditions = std::vector<AclMessage::AclCondition>;

    struct Attribute {
        Attribute(Conditions& conds) : conditions(conds) {}
        AclMessage::AclCondition condition{};
        bool mask_set{false};
        Conditions& conditions;
    };

    static void get_condition(const nlattr* nested, Attribute& attr) {
        get_condition_type(nested, attr.condition.type);
        get_condition_value(nested, attr.condition.value);
        get_condition_mask(nested, attr.condition.mask, attr.mask_set);
        if (attr.mask_set) {
            attr.conditions.push_back(attr.condition);
            attr.condition = {};
            attr.mask_set = false;
        }
    }

    static void get_conditions(const nlattr* const* attrs,
                               std::vector<AclMessage::AclCondition>& conditions) {
        if (!attrs[ACL_ATTR_COND]) {
            throw runtime_error("There are no conditions in the message.");
        }
        Attribute attr{conditions};
        for_each_nested<Attribute>(attrs[ACL_ATTR_COND], attr, get_condition);
    }

    static inline void get_rule_list_if_exists(nlattr* nla,
                                               AclMessage& message) {
        nlattr* attrs[ACL_ATTR_MAX + 1]{};
        nla_parse_nested(attrs, ACL_ATTR_MAX, nla, nullptr);
        auto rule_id = get_rule_id(attrs);
        uint16_t action = get_action(attrs);
        std::vector<uint32_t> mirrored_ports{};
        get_mirrored_ports(attrs, mirrored_ports);
        uint16_t mirror_type{ACL_MIRROR_UNSUPPORTED};
        get_mirror_type(attrs, mirror_type);
        std::vector<AclMessage::AclCondition> conditions{};
        get_conditions(attrs, conditions);
        uint32_t destination_port{};
        if (ACL_ATTR_ACTION_MIRROR_PORT == action) {
            uint16_t mirror_port{};
            get_mirror_port(attrs, mirror_port);
            destination_port = mirror_port;
        }
        else if (ACL_ATTR_ACTION_FORWARD == action) {
            get_forward_port(attrs, destination_port);
        }
        message.add_rule({rule_id, action, destination_port,
                         mirrored_ports, mirror_type, conditions});
    }

    static inline void get_acl_if_exists(nlattr* nla,
                                         AclMessage& message) {
        switch (nla_type(nla)) {
            case ACL_ATTR_NAME:
                {
                    string acl_name = nla_get_string(nla);
                    message.add_acl(acl_name);
                    message.set_name(acl_name);
                    break;
                }
            case ACL_ATTR_PORT_LIST:
                ::get_port_list_if_exists(nla, message);
                break;
            case ACL_ATTR_RULE_LIST_ENTRY:
                ::get_rule_list_if_exists(nla, message);
                break;
            default:
                break;
        }
    }

    static inline void nla_put(struct nl_msg* msg,
                               uint32_t attr, uint16_t value) {
        if (0 > nla_put_u16(msg, attr, value)) {
            throw runtime_error("nla_put_u16 failed");
        }
    }

    static inline void nla_put(struct nl_msg* msg,
                               uint32_t attr, uint32_t value) {
        if (0 > nla_put_u32(msg, attr, value)) {
            throw runtime_error("nla_put_u32 failed");
        }
    }

    static inline void nla_put(struct nl_msg* msg,
                               uint32_t attr, uint64_t value) {
        if (0 > nla_put_u64(msg, attr, value)) {
            throw runtime_error("nla_put_u64 failed");
        }
    }

    static inline void prepare_port_list(struct nl_msg* msg,
                                         const vector<uint32_t>& ifindexes) {
        nlattr *attr{};
        attr = nla_nest_start(msg, ACL_ATTR_PORT_LIST);
        if (!attr) {
            throw runtime_error("nla_nest_start failed for port list");
        }
        for (const auto& ifindex : ifindexes) {
            nla_put(msg, ACL_ATTR_PORT_IFINDEX, ifindex);
        }
        nla_nest_end(msg, attr);
    }

    template<class T>
    static inline void put_condition(struct nl_msg* msg,
                                     const AclMessage::AclCondition& condition,
                                     uint32_t value_attr,
                                     uint32_t mask_attr) {
        nla_put(msg, value_attr, T(condition.value));
        if (condition.mask) {
            nla_put(msg, mask_attr, T(condition.mask));
        }
    }

    static void put_ip_condition(struct nl_msg* msg,
                                 const AclMessage::AclCondition& condition) {
        switch (condition.type) {
            case ACL_ATTR_COND_IP_SRC:
                put_condition<uint32_t>(msg, condition,
                                        ACL_ATTR_COND_IP_SRC,
                                        ACL_ATTR_COND_IP_SRC_MASK);
                break;
            case ACL_ATTR_COND_IP_DEST:
                put_condition<uint32_t>(msg, condition,
                                             ACL_ATTR_COND_IP_DEST,
                                             ACL_ATTR_COND_IP_DEST_MASK);
                break;
            default:
                break;
        }
    }

    static void put_mac_condition(struct nl_msg* msg,
                                  const AclMessage::AclCondition& condition) {
        switch (condition.type) {
            case ACL_ATTR_COND_MAC_SRC:
                put_condition<uint64_t>(msg, condition,
                                        ACL_ATTR_COND_MAC_SRC,
                                        ACL_ATTR_COND_MAC_SRC_MASK);
                break;
            case ACL_ATTR_COND_MAC_DEST:
                put_condition<uint64_t>(msg, condition,
                                        ACL_ATTR_COND_MAC_DEST,
                                        ACL_ATTR_COND_MAC_DEST_MASK);
                break;
            default:
                break;
        }
    }

    static void put_port_condition(struct nl_msg* msg,
                                   const AclMessage::AclCondition& condition) {
        switch (condition.type) {
            case ACL_ATTR_COND_L4_PORT_SRC:
                put_condition<uint16_t>(msg, condition,
                                        ACL_ATTR_COND_L4_PORT_SRC,
                                        ACL_ATTR_COND_L4_PORT_SRC_MASK);
                break;
            case ACL_ATTR_COND_L4_PORT_DEST:
                put_condition<uint16_t>(msg, condition,
                                        ACL_ATTR_COND_L4_PORT_DEST,
                                        ACL_ATTR_COND_L4_PORT_DEST_MASK);
                break;
            default:
                break;
        }
    }

    static void put_vlan_condition(struct nl_msg* msg,
                                   const AclMessage::AclCondition& condition) {
        put_condition<uint16_t>(msg, condition,
                                ACL_ATTR_COND_VLAN,
                                ACL_ATTR_COND_VLAN_MASK);
    }

    static void put_protocol_condition(struct nl_msg* msg,
                                       const AclMessage::AclCondition& condition) {
        if (0 > nla_put_u8(msg, ACL_ATTR_COND_PROTOCOL,
                           uint8_t(condition.value))) {
            throw runtime_error("nla_put_u8 failed for rule condition");
        }
    }

    static inline void put_condition(struct nl_msg* msg,
                                     const AclMessage::AclCondition& condition) {
        switch (condition.type) {
            case ACL_ATTR_COND_IP_SRC:
            case ACL_ATTR_COND_IP_DEST:
                put_ip_condition(msg, condition);
                break;
            case ACL_ATTR_COND_MAC_SRC:
            case ACL_ATTR_COND_MAC_DEST:
                put_mac_condition(msg, condition);
                break;
            case ACL_ATTR_COND_L4_PORT_SRC:
            case ACL_ATTR_COND_L4_PORT_DEST:
                put_port_condition(msg, condition);
                break;
            case ACL_ATTR_COND_VLAN:
                put_vlan_condition(msg, condition);
                break;
            case ACL_ATTR_COND_PROTOCOL:
                put_protocol_condition(msg, condition);
                break;
            default:
                throw runtime_error("Unknown rule condition type");
        }
    }

    static void prepare_for_command(struct nl_msg* msg, uint8_t command,
                                    uint16_t rule_id,
                                    const vector<uint32_t>& ifindexes) {
        switch (command) {
            case ACL_ADD_RULE_CMD:
            case ACL_DEL_RULE_CMD:
                if (rule_id) {
                    NLA_PUT_U16(msg, ACL_ATTR_RULE_ID, rule_id);
                }
                break;
            case ACL_BIND_PORT_CMD:
                ::prepare_port_list(msg, ifindexes);
                break;
            default:
                break;
        }
        return;

        // following goto label is needed by NLA_PUT_ macros
nla_put_failure:
        throw runtime_error("NLA_PUT_U16 failed for rule command");
    }

    static void prepare_for_action(struct nl_msg* msg, uint16_t action,
                                   uint32_t ifindex, uint16_t mirror_type,
                                   const vector<uint32_t>& ifindexes) {
        switch (action) {
            case ACL_ATTR_ACTION_DENY:
            case ACL_ATTR_ACTION_PERM:
                NLA_PUT_FLAG(msg, action);
                break;
            case ACL_ATTR_ACTION_FORWARD:
                NLA_PUT_U32(msg, ACL_ATTR_ACTION_FORWARD, ifindex);
                break;
            case ACL_ATTR_ACTION_MIRROR_PORT:
                NLA_PUT_U32(msg, ACL_ATTR_ACTION_MIRROR_PORT, ifindex);
                NLA_PUT_U16(msg, ACL_ATTR_ACTION_MIRROR_TYPE,
                            mirror_type);
                ::prepare_port_list(msg, ifindexes);
                break;
            default:
                break;
        }
        return;

        // following goto label is needed by NLA_PUT_ macros
nla_put_failure:
        throw runtime_error("NLA_PUT_ failed for rule action");
    }
}

AclMessage::AclMessage(uint8_t cmd, const string& name) :
    NlMessage(Protocol::USER),
    m_cmd(cmd), m_name(name), m_errno(NL_ERR), m_action(ACL_ATTR_UNSPEC),
    m_mirror_type(ACL_MIRROR_UNSUPPORTED) {
    nl_socket_disable_auto_ack(get_sock());
    nl_socket_disable_seq_check(get_sock());
    nl_socket_set_peer_port(get_sock(), ACL_TOOL_SOCKET_PORT);
    auto status = nl_socket_set_msg_buf_size(get_sock(), ACL_NL_MSG_SIZE_BIG);
    if (status) {
        throw NlException(string("ACL: nl_socket_set_msg_buf_size failed ") +
                          to_string(status));
    }
    status = nl_socket_set_buffer_size(get_sock(), ACL_NL_MSG_SIZE_BIG,
                                       ACL_NL_MSG_SIZE_BIG);
    if (status) {
        throw NlException(string("ACL: nl_socket_set_buffer_size failed ") +
                          to_string(status));
    }
}

AclMessage::~AclMessage() {}

void AclMessage::prepare_message(struct nl_msg* msg) {
    if (!genlmsg_put(msg, NL_AUTO_PORT, NL_AUTO_SEQ, 0, 0, 0, m_cmd, 0)) {
        throw NlException("Cannot put NL_AUTO_PORT attributes");
    }
    if (0 < m_name.length()) {
        NLA_PUT_STRING(msg, ACL_ATTR_NAME, m_name.c_str());
    }
    ::prepare_for_command(msg, m_cmd, m_rule_id, m_ifindexes);
    ::prepare_for_action(msg, m_action, m_ifindex, m_mirror_type, m_ifindexes);
    for (const auto& condition : m_conditions) {
        put_condition(msg, condition);
    }
    return;

    // following goto label is needed by NLA_PUT_ macros
nla_put_failure:
    throw NlException("Cannot put attribute into the netlink message");
}

void AclMessage::parse_acl_list (const genlmsghdr* genl_hdr) {
        nlattr* nla{};
        int rem{};
        nla_for_each_attr(nla, genlmsg_attrdata(genl_hdr, 0),
                          genlmsg_attrlen(genl_hdr, 0), rem) {
            if ((ACL_ATTR_MAX + 1) >= nla_type(nla)) {
                ::get_acl_if_exists(nla, *this);
            }
        }
        m_errno = NL_OK;
}

void AclMessage::process_message(struct nl_msg* msg) {
    nlmsghdr *hdr = nlmsg_hdr(msg);
    genlmsghdr *genl_hdr = static_cast<genlmsghdr*>(nlmsg_data(hdr));
    nlattr *attrs[ACL_ATTR_MAX + 1]{};
    int status = genlmsg_parse(hdr, 0, attrs, ACL_ATTR_MAX, nullptr);
    if (status || !genl_hdr) {
        throw NlException("failed parsing message header");
    }

    if (ACL_GET_CMD_REPLY == genl_hdr->cmd) {
        parse_acl_list(genl_hdr);
    }
    else {
        if (attrs[ACL_ATTR_ERRNO]) {
            m_errno = nla_get_u32(attrs[ACL_ATTR_ERRNO]);
        }
        else {
            throw NlException("Missing attribute ACL_ATTR_ERRNO");
        }
    }
}

void AclMessage::set_rule(uint16_t rule_id, uint16_t action, uint32_t ifindex,
                          const vector<AclCondition>& conditions) {
    m_rule_id = rule_id;
    m_action = action;
    m_ifindex = ifindex;
    m_conditions = conditions;
}
