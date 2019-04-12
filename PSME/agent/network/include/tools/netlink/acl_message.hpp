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
 *
 * @file acl_message.hpp
 *
 * @brief ACL netlink message class
 * */

#pragma once
/* Internal headers */
#include "netlink/nl_message.hpp"

/* C/C++ standard headers */
#include <string>
#include <vector>
#include <map>
#include <netlink/genl/genl.h>

namespace agent {
namespace network {
namespace api {
namespace netlink {

/*! ACL Message class */
class AclMessage: public netlink_base::NlMessage {
public:
    using Strings = std::vector<std::string>;
    using MapOfStrings = std::map<std::string, Strings>;

    /*!
     * @brief ACL condition struct with ACL_ATTR_COND_ type.
     * */
    struct AclCondition {
        uint32_t type{};
        uint64_t value{};
        uint64_t mask{};
    };

    /*!
     * @brief ACL rule class.
     * */
    class Rule {
    public:
        Rule(uint16_t rule_id, uint16_t action, uint32_t destination_port,
             std::vector<uint32_t> mirrored_ports, uint16_t mirror_type,
             std::vector<AclCondition> conditions):
            m_rule_id(rule_id), m_action(action),
            m_destination_port(destination_port),
            m_mirrored_ports(mirrored_ports), m_mirror_type(mirror_type),
            m_conditions(conditions) {}

        /*!
         * @brief Get ACL rule id
         *
         * @return ACL rule id
         * */
        uint16_t get_rule_id() const { return m_rule_id; }

        /*!
         * @brief Get ACL action
         *
         * @return ACL action
         * */
        uint16_t get_action() const { return m_action; }

        /*!
         * @brief Get destination port index.
         *
         * @return port index
         * */
        uint32_t get_destination_port() const { return m_destination_port; }

        /*!
         * @brief Get mirrored ports.
         *
         * @return mirrored ports.
         * */
        const std::vector<uint32_t>& get_mirrored_ports() const {
            return m_mirrored_ports;
        }

        /*!
         * @brief Get mirror type.
         *
         * @return mirror type
         * */
        uint16_t get_mirror_type() const { return m_mirror_type; }

        /*!
         * @brief Get conditions.
         *
         * @return conditions.
         * */
        const std::vector<AclCondition>& get_conditions() const {
            return m_conditions;
        }

    private:
        uint16_t m_rule_id{};
        uint16_t m_action{};
        uint32_t m_destination_port{};
        std::vector<uint32_t> m_mirrored_ports{};
        uint16_t m_mirror_type{};
        std::vector<AclCondition> m_conditions{};
    };

    using Rules = std::vector<Rule>;

    /*!
     * @brief Default constructor.
     * */
    AclMessage() = delete;

    /*!
     * @brief Default constructor.
     *
     * @param[in] cmd ACL command.
     * @param[in] name ACL name.
     * */
    AclMessage(uint8_t cmd, const std::string& name);

    /*!
     * @brief Default destructor.
     * */
    virtual ~AclMessage();

    /*!
     * @brief Prepare message to be sent.
     *
     * @param[out] msg Netlink message.
     * */
    void prepare_message(struct nl_msg* msg) override;

    /*!
     * @brief Parse received netlink message and store results in message.
     *
     * @param[in] msg Netlink message.
     * */
    void process_message(struct nl_msg* msg) override;

    /*!
     * @brief Parse ACL list in message.
     *
     * @param[in] genl_hdr Generic netlink message header.
     * */
    void parse_acl_list(const genlmsghdr* genl_hdr);

    /*!
     * @brief Get error code from last command.
     *
     * @return errno
     * */
    uint32_t get_errno() { return m_errno; }

    /*!
     * @brief Get ACL list.
     *
     * @return ACLs
     * */
    const Strings& get_acls() { return m_acls; }

    /*!
     * @brief Get bound ports for all ACLs.
     *
     * @return Bound ports
     * */
    const MapOfStrings& get_bound_ports() {
        return m_bound_ports;
    }

    /*!
     * @brief Set information about ACL rule.
     *
     * @param[in] rule_id rule identifier.
     * @param[in] action ACL action type.
     * @param[in] ifindex ifindex of port used by forwarding action.
     * @param[in] conditions list of conditions for given rule.
     * */
    void set_rule(uint16_t rule_id, uint16_t action, uint32_t ifindex,
                  const std::vector<AclCondition>& conditions);

    /*!
     * @brief Set mirror type
     *
     * @param[in] mirror_type Mirror type.
     * */
    void set_mirror_type(uint16_t mirror_type) {
        m_mirror_type = mirror_type;
    }

    /*!
     * @brief Add bound port.
     *
     * @param[in] port_identifier Port identifier.
     * */
    void add_bound_port(const std::string& port_identifier) {
        m_bound_ports[m_name].push_back(port_identifier);
    }

    /*!
     * @brief Add ACL.
     *
     * @param[in] acl_name ACL name.
     * */
    void add_acl(const std::string& acl_name) {
        m_acls.push_back(acl_name);
    }

    /*!
     * @brief Set port interface indexes.
     *
     * @param[in] ifindexes List of port indexes.
     * */
    void set_ifindexes(const std::vector<uint32_t>& ifindexes) {
        m_ifindexes = ifindexes;
    }

    /*!
     * @brief Set ACL name.
     *
     * @param[in] name ACL name.
     * */
    void set_name(const std::string& name) {
        m_name = name;
    }

    /*!
     * @brief Get ACL name.
     *
     * @return ACL name
     * */
    const std::string& get_name() const { return m_name; }

    /*!
     * @brief Add ACL rule.
     *
     * @param[in] rule ACL rule.
     * */
    void add_rule(const Rule& rule) {
        m_rules.push_back(rule);
    }

    /*!
     * @brief Get ACL rules
     *
     * @return ACL rules
     * */
    const Rules& get_rules() const { return m_rules; }

    /*
     * @brief Set ACL rule id.
     *
     * @param[in] rule_id rule identifier.
     * */
    void set_rule_id(uint16_t rule_id) { m_rule_id = rule_id; }

private:
    uint8_t m_cmd{};
    std::string m_name{};
    uint32_t m_errno{};
    uint16_t m_rule_id{};
    uint16_t m_action{};
    uint32_t m_ifindex{};
    std::vector<AclCondition> m_conditions{};
    uint16_t m_mirror_type{};
    Strings m_acls{};
    MapOfStrings m_bound_ports{};
    std::vector<uint32_t> m_ifindexes{};
    Rules m_rules{};
};

} /* netlink */
} /* api */
} /* network */
} /* agent */
