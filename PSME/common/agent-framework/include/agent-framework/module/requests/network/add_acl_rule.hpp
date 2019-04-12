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
 * @file requests/network/add_acl_rule.hpp
 * @brief network AddAclRule request
 * */

#pragma once

#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/enum/network.hpp"
#include "agent-framework/module/constants/command.hpp"
#include "agent-framework/module/constants/network.hpp"
#include "agent-framework/module/utils/optional_field.hpp"
#include "agent-framework/validators/procedure_validator.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace requests {

/*! network AddAclRule request */
class AddAclRule{
public:
    using MirroredPorts = agent_framework::model::attribute::Array<std::string>;
    using VlanId = agent_framework::model::attribute::AclVlanId;
    using Ip = agent_framework::model::attribute::AclAddress;
    using Mac = agent_framework::model::attribute::AclAddress;
    using Port = agent_framework::model::attribute::AclPort;
    using Oem = agent_framework::model::attribute::Oem;

    /*!
     * @brief explicit AddAclRule request constructor
     *
     * @param[in] acl ACL identifier
     * @param[in] rule_id Rule id
     * @param[in] action Action
     * @param[in] forward_mirror_port Forward mirror port
     * @param[in] mirrored_ports Mirrored ports
     * @param[in] mirror_type Mirror type
     * @param[in] vlan_id VLAN id
     * @param[in] source_ip Source IP
     * @param[in] destination_ip Destination IP
     * @param[in] source_mac Source MAC
     * @param[in] destination_mac Destination MAC
     * @param[in] source_port Source port
     * @param[in] destination_port Destination port
     * @param[in] protocol Protocol
     * @param[in] oem OEM specific data
     * */
    explicit AddAclRule(const OptionalField<std::string>& acl,
                        const OptionalField<uint32_t> rule_id,
                        const OptionalField<enums::AclAction>& action,
                        const OptionalField<std::string>& forward_mirror_port,
                        const MirroredPorts& mirrored_ports,
                        const OptionalField<enums::AclMirrorType>& mirror_type,
                        const VlanId& vlan_id,
                        const Ip& source_ip,
                        const Ip& destination_ip,
                        const Mac& source_mac,
                        const Mac& destination_mac,
                        const Port& source_port,
                        const Port& destination_port,
                        const OptionalField<uint32_t>& protocol,
                        const Oem& oem);

    /*!
     * @brief Get command name
     *
     * @return Command name
     * */
    static std::string get_command() {
        return literals::Command::ADD_ACL_RULE;
    }

    /*!
     * @brief Get acl UUID
     *
     * @return UUID as std::string
     * */
    const OptionalField<std::string>& get_acl() const {
        return m_acl;
    }

    /*!
     * @brief Get acl rule action
     *
     * @return action as std::string
     * */
    const OptionalField<enums::AclAction>& get_action() const {
        return m_action;
    }

    /*!
     * @brief Get forward/mirror port
     *
     * @return forward/mirror port as std::string
     * */
    const OptionalField<std::string>& get_forward_mirror_port() const {
        return m_forward_mirror_port;
    }

    /*!
     * @brief Get mirror type
     *
     * @return mirror type as std::string
     * */
    const OptionalField<enums::AclMirrorType>& get_mirror_type() const {
        return m_mirror_type;
    }

    /*!
     * @brief Get rule id
     *
     * @return rule id
     * */
    const OptionalField<uint32_t>& get_rule_id() const {
        return m_rule_id;
    }

    /*!
     * @brief Get vlan id
     *
     * @return vlan id
     * */
    const VlanId& get_vlan_id() const {
        return m_vlan_id;
    }

    /*!
     * @brief Get source IP
     *
     * @return source IP
     * */
    const Ip& get_source_ip() const {
        return m_source_ip;
    }

    /*!
     * @brief Get destination IP
     *
     * @return destination IP
     * */
    const Ip& get_destination_ip() const {
        return m_destination_ip;
    }

    /*!
     * @brief Get source MAC
     *
     * @return source MAC
     * */
    const Mac& get_source_mac() const {
        return m_source_mac;
    }

    /*!
     * @brief Get destination MAC
     *
     * @return destination MAC
     * */
    const Mac& get_destination_mac() const {
        return m_destination_mac;
    }

    /*!
     * @brief Get source port
     *
     * @return source port
     * */
    const Port& get_source_port() const {
        return m_source_port;
    }

    /*!
     * @brief Get destination port
     *
     * @return destination port
     * */
    const Port& get_destination_port() const {
        return m_destination_port;
    }

    /*!
     * @brief Get protocol
     *
     * @return protocol
     * */
    const OptionalField<uint32_t>& get_protocol() const {
        return m_protocol;
    }

    /*!
     * @brief Get mirrored ports
     *
     * @return mirrored ports
     * */
    const MirroredPorts& get_mirrored_ports() const {
        return m_mirrored_ports;
    }

    /*!
     * @brief Get OEM data
     *
     * @return OEM data
     */
    const Oem& get_oem() const {
        return m_oem;
    }

    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     */
    json::Json to_json() const;

    /*!
     * @brief create AddAclRule from Json
     *
     * @param[in] json the input argument
     *
     * @return new AddAclRule
     */
    static AddAclRule from_json(const json::Json& json);

    /*!
     * @brief Return procedure schema
     *
     * @return procedure schema
     */
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
            get_command(), jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_OBJECT,
            literals::Acl::ACL, jsonrpc::JSON_STRING,
            literals::AclRule::RULE_ID, VALID_NULLABLE(VALID_OPTIONAL(VALID_JSON(INTEGER))),
            literals::AclRule::ACTION, jsonrpc::JSON_STRING,
            literals::AclRule::FORWARD_MIRROR_PORT, VALID_NULLABLE(VALID_OPTIONAL(VALID_JSON(STRING))),
            literals::AclRule::MIRRORED_PORTS, VALID_NULLABLE(VALID_OPTIONAL(VALID_JSON(ARRAY))),
            literals::AclRule::MIRROR_TYPE, VALID_NULLABLE(VALID_OPTIONAL(VALID_JSON(STRING))),
            literals::AclRule::VLAN_ID, VALID_NULLABLE(VALID_OPTIONAL(VALID_JSON(OBJECT))),
            literals::AclRule::SOURCE_IP, VALID_NULLABLE(VALID_OPTIONAL(VALID_JSON(OBJECT))),
            literals::AclRule::DESTINATION_IP, VALID_NULLABLE(VALID_OPTIONAL(VALID_JSON(OBJECT))),
            literals::AclRule::SOURCE_MAC, VALID_NULLABLE(VALID_OPTIONAL(VALID_JSON(OBJECT))),
            literals::AclRule::DESTINATION_MAC, VALID_NULLABLE(VALID_OPTIONAL(VALID_JSON(OBJECT))),
            literals::AclRule::SOURCE_L4_PORT, VALID_NULLABLE(VALID_OPTIONAL(VALID_JSON(OBJECT))),
            literals::AclRule::DESTINATION_L4_PORT, VALID_NULLABLE(VALID_OPTIONAL(VALID_JSON(OBJECT))),
            literals::AclRule::PROTOCOL, VALID_NULLABLE(VALID_OPTIONAL(VALID_JSON(INTEGER))),
            literals::AclRule::OEM, VALID_NULLABLE(VALID_OPTIONAL(VALID_JSON(OBJECT))),
            nullptr
        };
        return procedure;
    }

private:
    OptionalField<std::string> m_acl{};
    OptionalField<uint32_t> m_rule_id{};
    OptionalField<enums::AclAction> m_action{};
    OptionalField<std::string> m_forward_mirror_port{};
    MirroredPorts m_mirrored_ports{};
    OptionalField<enums::AclMirrorType> m_mirror_type{};
    VlanId m_vlan_id{};
    Ip m_source_ip{};
    Ip m_destination_ip{};
    Mac m_source_mac{};
    Mac m_destination_mac{};
    Port m_source_port{};
    Port m_destination_port{};
    OptionalField<uint32_t> m_protocol{};
    Oem m_oem{};
};

}
}
}
