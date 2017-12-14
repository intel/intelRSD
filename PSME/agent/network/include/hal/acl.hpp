/*!
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
 */

#pragma once
#include <string>
#include <vector>
#include <map>

namespace agent {
namespace network {
namespace hal {

/*!
 * @brief Acl network implementation.
 * */
class Acl {
public:
    using Strings = std::vector<std::string>;
    using MapOfStrings = std::map<std::string, Strings>;

    /*!
     * @enum ConditionType
     * @brief Different Acl condition types.
     *
     * Based on these conditions different actions are taken.
     *
     * @var ConditionType::CONDITION_IP_SRC
     * source IP
     *
     * @var ConditionType::CONDITION_IP_DEST
     * destination IP
     *
     * @var ConditionType::CONDITION_MAC_SRC
     * source MAC address
     *
     * @var ConditionType::CONDITION_MAC_DEST
     * destination MAC address
     *
     * @var ConditionType::CONDITION_VLAN
     * VLAN number
     *
     * @var ConditionType::CONDITION_PROTOCOL
     * protocol type
     *
     * @var ConditionType::CONDITION_L4_PORT_SRC
     * source port
     *
     * @var ConditionType::CONDITION_L4_PORT_DEST
     * destination port
     * */
    enum class ConditionType {
        CONDITION_IP_SRC = 0,
        CONDITION_IP_DEST,
        CONDITION_MAC_SRC,
        CONDITION_MAC_DEST,
        CONDITION_VLAN,
        CONDITION_PROTOCOL,
        CONDITION_L4_PORT_SRC,
        CONDITION_L4_PORT_DEST
    };

    /*!
     * @brief Condition structure with type, value and mask.
     * */
    struct Condition {
        Condition() {}
        Condition(ConditionType t, uint64_t v, uint64_t m) :
            type{t}, value{v}, mask{m} {}
        ConditionType type{};
        uint64_t value{};
        uint64_t mask{};
    };

    /*!
     * @enum ActionType
     * @brief Condition structure with type, value and mask.
     *
     * Different actions that are taken based on conditions.
     *
     * @var ActionType::ACTION_DENY
     * drop/deny
     *
     * @var ActionType::ACTION_PERMIT
     * permit/allow
     *
     * @var ActionType::ACTION_FORWARD
     * forward
     *
     * @var ActionType::ACTION_MIRROR
     * mirror
     * */
    enum class ActionType {
        ACTION_DENY = 0,
        ACTION_PERMIT,
        ACTION_FORWARD,
        ACTION_MIRROR
    };

    /*!
     * @enum MirrorType
     * @brief Mirror type.
     *
     * Different types of mirroring.
     *
     * @var MirrorType::MIRROR_INGRESS
     * ingress
     *
     * @var MirrorType::MIRROR_EGRESS
     * egress
     *
     * @var MirrorType::MIRROR_BIDIRECTIONAL
     * bidirectional
     *
     * @var MirrorType::MIRROR_REDIRECT
     * redirect
     *
     * */
    enum class MirrorType {
        MIRROR_NA = 0,
        MIRROR_INGRESS,
        MIRROR_EGRESS,
        MIRROR_BIDIRECTIONAL,
        MIRROR_REDIRECT
    };

    /*!
     * @brief Rule class.
     * */
    class Rule {
    public:
        Rule(uint16_t rule_id, const ActionType& action_type,
             const std::string& destination_port,
             const Strings& mirrored_ports,
             const MirrorType& mirror_type,
             const std::vector<Condition>& conditions):
            m_rule_id(rule_id), m_action_type(action_type),
            m_destination_port(destination_port),
            m_mirrored_ports(mirrored_ports),
            m_mirror_type(mirror_type),
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
        ActionType get_action() const { return m_action_type; }

        /*!
         * @brief Get destination port identifier
         *
         * @return port identifier
         * */
        const std::string& get_destination_port() const { return m_destination_port; }

        /*!
         * @brief Get mirrored ports.
         *
         * @return mirrored ports.
         * */
        const std::vector<std::string>& get_mirrored_ports() const {
            return m_mirrored_ports;
        }

        /*!
         * @brief Get mirror type.
         *
         * @return mirror type
         * */
        MirrorType get_mirror_type() const { return m_mirror_type; }

        /*!
         * @brief Get conditions.
         *
         * @return conditions.
         * */
        const std::vector<Condition>& get_conditions() const {
            return m_conditions;
        }

    private:
        uint16_t m_rule_id{};
        ActionType m_action_type{};
        std::string m_destination_port{};
        Strings m_mirrored_ports{};
        MirrorType m_mirror_type{MirrorType::MIRROR_NA};
        std::vector<Condition> m_conditions{};
    };

    using Rules = std::vector<Rule>;

    /*!
     * @brief Default constructor.
     * */
    Acl();

    /*!
     * @brief Default destructor.
     * */
    virtual ~Acl();

    /*!
     * @brief Create ACL.
     *
     * @param[in] name ACL name
     * */
    static void create_acl(const std::string& name);

    /*!
     * @brief Bind ACL to additional set of ports.
     *
     * @param[in] name ACL name
     * @param[in] port_identifiers Port identifiers.
     * */
    void bind_acl_to_ports(const std::string& name,
                           const std::vector<std::string>& port_identifiers);

    /*!
     * @brief Unbind ACL from all ports.
     *
     * @param[in] name ACL name
     * */
    void unbind_acl_from_all_ports(const std::string& name);

    /*!
     * @brief Delete ACL.
     *
     * @param[in] name ACL name
     * */
    static void delete_acl(const std::string& name);

    /*!
     * @brief Read ACLs from the switch.
     * */
    void read_acl_list();

    /*!
     * @brief Read ACL rules for given ACL from the switch.
     *
     * @param[in] acl_name ACL name
     * */
    void read_acl_rule_list(const std::string& acl_name);

    /*!
     * @brief Get ACLs.
     *
     * @return List of ACLs
     * */
    const Strings& get_acls() { return m_acls; }

    /*!
     * @brief Get bound ports for given ACL.
     *
     * @param acl ACL name
     *
     * @return Bound ports
     * */
    const Strings& get_bound_ports(const std::string& acl) {
        return m_bound_ports[acl];
    }

    /*!
     * @brief Add rule(drop or forward) to given ACL.
     *
     * @param[in] acl_name ACL name
     * @param[in] rule_id rule Id
     * @param[in] action action to be taken
     * @param[in] port_identifier forward only attribute
     * @param[in] conditions conditions to take action
     * */
    static void add_rule(const std::string& acl_name, uint16_t rule_id,
                         ActionType action, const std::string& port_identifier,
                         const std::vector<Condition>& conditions);

    /*!
     * @brief Add mirror rule to given ACL.
     *
     * @param[in] acl_name ACL name
     * @param[in] rule_id rule Id
     * @param[in] mirror_type mirror type
     * @param[in] destination_port destination port identifier
     * @param[in] mirrored_ports mirrored ports
     * @param[in] conditions conditions to take action
     * */
    static void add_mirror_rule(const std::string& acl_name, uint16_t rule_id,
                                const MirrorType& mirror_type,
                                const std::string& destination_port,
                                const std::vector<std::string>& mirrored_ports,
                                const std::vector<Condition>& conditions);

    /*!
     * @brief Delete ACL Rule.
     *
     * @param[in] name ACL name
     * @param[in] rule_id ACL rule id
     * */
    static void delete_acl_rule(const std::string& name, uint16_t rule_id);

    /*!
     * @brief Get rules for given ACL.
     *
     * @return Rules
     * */
    const Rules& get_rules() {
        return m_rules;
    }

    /*!
     * @brief Save ACL name for given UUID.

     * @param[in] uuid UUID of the ACL.
     * @param[in] name ACL name.
     * */
    static void add_acl_name(const std::string& uuid, const std::string& name);

    /*!
     * @brief Get ACL name for given ACL UUID if exist.

     * @param[in] uuid UUID of the ACL.

     * @return ACL name if found in the map, UUID if not.
     * */
    static const std::string& get_acl_name(const std::string& uuid);

private:
    Strings m_acls{};
    MapOfStrings m_bound_ports{};
    Rules m_rules{};
};

}
}
}
