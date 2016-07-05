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

#include "api/acl.hpp"
#include "api/netlink/acl_message.hpp"
#include "api/netlink/acl_socket.hpp"
#include "logger/logger_factory.hpp"

extern "C" {
#include <acl_nl_cmds.h>
#include <net/if.h>
}

using namespace agent::network::api;
using namespace agent::network::api::netlink;
using namespace std;

Acl::Acl() {}
Acl::~Acl() {}

static int netlink_callback(nl_msg *msg, void *arg) {
    if (!msg) {
        return NL_ERR;
    }
    AclMessage *acl_message = reinterpret_cast<AclMessage*>(arg);
    if (!acl_message) {
        return NL_ERR;
    }

    return acl_message->parse_reply(msg);
}

static void send_and_receive(AclMessage& msg) {
    AclSocket socket{};
    int retcode{NL_ERR};

    socket.connect();
    socket.modify_callback(netlink_callback, &msg);
    socket.send_message(msg);
    socket.receive_to_callback(msg);
    retcode = msg.get_errno();
    if (retcode) {
        throw runtime_error("ACL netlink function returned error condition: "
                            + to_string(retcode));
    }
}

namespace {
    uint32_t get_ifindex(const string& port_identifier) {
        if (0 == port_identifier.length()) {
            return 0;
        }
        uint32_t ifindex = if_nametoindex(port_identifier.c_str());
        if (0 == ifindex) {
            throw runtime_error("Unable to get ifindex for port identifier: " +
                                port_identifier + ", error: " +
                                to_string(errno));
        }
        return ifindex;
    }

    void get_ifindexes(const vector<string>& port_identifiers,
                       vector<uint32_t>& ifindexes) {
        for (const auto& port_identifier : port_identifiers) {
            ifindexes.push_back(get_ifindex(port_identifier));
        }
    }

    void bind_to_ports(const string& name,
                      const vector<string>& port_identifiers) {
        AclMessage msg(ACL_BIND_PORT_CMD, name);
        vector<uint32_t> ifindexes{};
        get_ifindexes(port_identifiers, ifindexes);
        msg.set_ifindexes(ifindexes);
        send_and_receive(msg);
    }

    void unbind(const std::string& name) {
        AclMessage msg(ACL_UNBIND_CMD, name);
        send_and_receive(msg);
    }

    static std::map<Acl::ConditionType, uint32_t>
        condition_type_to_acl_condition {
            {Acl::ConditionType::CONDITION_IP_SRC, ACL_ATTR_COND_IP_SRC},
            {Acl::ConditionType::CONDITION_IP_DEST, ACL_ATTR_COND_IP_DEST},
            {Acl::ConditionType::CONDITION_MAC_SRC, ACL_ATTR_COND_MAC_SRC},
            {Acl::ConditionType::CONDITION_MAC_DEST, ACL_ATTR_COND_MAC_DEST},
            {Acl::ConditionType::CONDITION_L4_PORT_SRC, ACL_ATTR_COND_L4_PORT_SRC},
            {Acl::ConditionType::CONDITION_L4_PORT_DEST, ACL_ATTR_COND_L4_PORT_DEST},
            {Acl::ConditionType::CONDITION_VLAN, ACL_ATTR_COND_VLAN},
            {Acl::ConditionType::CONDITION_PROTOCOL, ACL_ATTR_COND_PROTOCOL}
        };

    uint32_t get_acl_condition(Acl::ConditionType condition) {
        return condition_type_to_acl_condition[condition];
    }

    Acl::ConditionType get_condition_type(const uint32_t acl_condition) {
        for (const auto& iter : condition_type_to_acl_condition) {
            if (iter.second == acl_condition) {
                return iter.first;
            }
        }
        throw runtime_error("Unknown condition type: " +
                            std::to_string(acl_condition));
    }

    void get_acl_conditions(const vector<Acl::Condition>& conditions,
                            vector<AclMessage::AclCondition>& acl_conditions) {
        for (const auto& condition : conditions) {
            AclMessage::AclCondition acl_condition{};
            acl_condition.value = condition.value;
            acl_condition.mask = condition.mask;
            acl_condition.type = get_acl_condition(condition.type);
            acl_conditions.push_back(acl_condition);
        }
    }

    static std::map<Acl::MirrorType, uint16_t> mirror_type_to_acl_mirror{
        {Acl::MirrorType::MIRROR_NA, ACL_MIRROR_UNSUPPORTED},
        {Acl::MirrorType::MIRROR_INGRESS, ACL_MIRROR_INGRESS},
        {Acl::MirrorType::MIRROR_EGRESS, ACL_MIRROR_EGRESS},
        {Acl::MirrorType::MIRROR_BIDIRECTIONAL, ACL_MIRROR_BIDIRECTIONAL},
        {Acl::MirrorType::MIRROR_REDIRECT, ACL_MIRROR_REDIRECT}
    };

    uint16_t get_acl_mirror(const Acl::MirrorType& mirror_type) {
        return mirror_type_to_acl_mirror[mirror_type];
    }

    Acl::MirrorType get_mirror_type(const uint16_t mirror) {
        for (const auto& pair : mirror_type_to_acl_mirror) {
            if (pair.second == mirror) {
                return pair.first;
            }
        }
        throw runtime_error("Unknown mirror type: " +
                            std::to_string(int(mirror)));
    }

    static std::map<Acl::ActionType, uint16_t> action_type_to_acl_action{
        {Acl::ActionType::ACTION_PERMIT, ACL_ATTR_ACTION_PERM},
        {Acl::ActionType::ACTION_DENY, ACL_ATTR_ACTION_DENY},
        {Acl::ActionType::ACTION_FORWARD, ACL_ATTR_ACTION_FORWARD},
        {Acl::ActionType::ACTION_MIRROR, ACL_ATTR_ACTION_MIRROR_PORT}
    };

    uint16_t get_acl_action(const Acl::ActionType& action) {
        return action_type_to_acl_action[action];
    }

    Acl::ActionType get_action_type(const uint16_t action) {
        for (const auto& pair : action_type_to_acl_action) {
            if (pair.second == action) {
                return pair.first;
            }
        }
        throw runtime_error("Unknown action type: " +
                            std::to_string(int(action)));
    }

    string get_port_identifier(const uint32_t ifindex) {
        char buffer[IF_NAMESIZE + 1]{0};
        if (nullptr == if_indextoname(ifindex, buffer)) {
            throw runtime_error("Invalid ifindex for a switch port: " +
                                std::to_string(ifindex));
        }
        return buffer;
    }

    void get_conditions(const vector<AclMessage::AclCondition>& acl_conditions,
                        vector<Acl::Condition>& conditions) {
        for (const auto& acl_condition : acl_conditions) {
            conditions.push_back({
                                 get_condition_type(acl_condition.type),
                                 acl_condition.value,
                                 acl_condition.mask
                                 });

        }
    }

    void get_acl_rules(const AclMessage::Rules& acl_rules, Acl::Rules& rules) {
        for (const auto& acl_rule : acl_rules) {
            string destination_port{};
            if (acl_rule.get_destination_port()) {
                destination_port =
                    get_port_identifier(acl_rule.get_destination_port());
            }
            AclMessage::Strings mirrored_ports{};
            for (const auto& ifindex : acl_rule.get_mirrored_ports()) {
                mirrored_ports.push_back(get_port_identifier(ifindex));
            }
            std::vector<Acl::Condition> conditions{};
            get_conditions(acl_rule.get_conditions(), conditions);
            rules.push_back({acl_rule.get_rule_id(),
                get_action_type(acl_rule.get_action()),
                destination_port,
                mirrored_ports,
                get_mirror_type(acl_rule.get_mirror_type()),
                conditions});
        }
    }
}

void Acl::create_acl(const string& name) {
    AclMessage msg(ACL_CREATE_CMD, name);
    send_and_receive(msg);
}

void Acl::delete_acl(const string& name) {
    AclMessage msg(ACL_DELETE_CMD, name);
    send_and_receive(msg);
}

void Acl::add_rule(const string& acl_name, uint16_t rule_id,
                  ActionType action, const string& port_identifier,
                  const vector<Condition>& conditions) {
    AclMessage msg(ACL_ADD_RULE_CMD, acl_name);
    vector<AclMessage::AclCondition> acl_conditions{};
    ::get_acl_conditions(conditions, acl_conditions);
    msg.set_rule(rule_id, ::get_acl_action(action),
                 ::get_ifindex(port_identifier), acl_conditions);
    send_and_receive(msg);
}

void Acl::add_mirror_rule(const string& acl_name, uint16_t rule_id,
                          const MirrorType& mirror_type,
                          const string& destination_port,
                          const vector<string>& mirrored_ports,
                          const vector<Condition>& conditions) {
    AclMessage msg(ACL_ADD_RULE_CMD, acl_name);
    vector<AclMessage::AclCondition> acl_conditions{};
    vector<uint32_t> mirrored_ports_indexes{};

    ::get_acl_conditions(conditions, acl_conditions);
    msg.set_rule(rule_id, ACL_ATTR_ACTION_MIRROR_PORT,
                 ::get_ifindex(destination_port), acl_conditions);
    msg.set_mirror_type(::get_acl_mirror(mirror_type));
    ::get_ifindexes(mirrored_ports, mirrored_ports_indexes);
    msg.set_ifindexes(mirrored_ports_indexes);
    send_and_receive(msg);
}

void Acl::read_acl_list() {
    m_acls.clear();
    m_bound_ports.clear();
    AclMessage msg(ACL_GET_CMD, "");
    send_and_receive(msg);
    m_acls = msg.get_acls();
    m_bound_ports = msg.get_bound_ports();
}

void Acl::read_acl_rule_list(const string& acl_name) {
    m_rules.clear();
    AclMessage msg(ACL_GET_CMD, acl_name);
    send_and_receive(msg);
    ::get_acl_rules(msg.get_rules(), m_rules);
}

void Acl::bind_acl_to_ports(const string& acl_name,
                            const vector<string>& port_identifiers) {
    if (0 < port_identifiers.size()) {
        ::bind_to_ports(acl_name, port_identifiers);
    }
}

void Acl::unbind_acl_from_all_ports(const string& acl_name) {
    read_acl_list();
    if (m_bound_ports.end() != m_bound_ports.find(acl_name)) {
        ::unbind(acl_name);
    }
}

void Acl::delete_acl_rule(const std::string& name, uint16_t rule_id) {
    AclMessage msg(ACL_DEL_RULE_CMD, name);
    msg.set_rule_id(rule_id);
    send_and_receive(msg);
}

