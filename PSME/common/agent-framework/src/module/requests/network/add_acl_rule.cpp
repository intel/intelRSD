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
 * @file requests/network/add_acl_rule.cpp
 *
 * @brief Network request AddAclRule implementation
 * */

#include "agent-framework/module/requests/network/add_acl_rule.hpp"
#include "agent-framework/module/constants/network.hpp"
#include "agent-framework/module/constants/common.hpp"
#include "json-wrapper/json-wrapper.hpp"



using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;


AddAclRule::AddAclRule(const OptionalField<std::string>& acl,
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
                       const Oem& oem)
    : m_acl(acl), m_rule_id(rule_id), m_action(action),
      m_forward_mirror_port(forward_mirror_port),
      m_mirrored_ports(mirrored_ports), m_mirror_type(mirror_type),
      m_vlan_id(vlan_id),
      m_source_ip(source_ip),
      m_destination_ip(destination_ip),
      m_source_mac(source_mac),
      m_destination_mac(destination_mac),
      m_source_port(source_port),
      m_destination_port(destination_port),
      m_protocol(protocol),
      m_oem(oem) {}


json::Json AddAclRule::to_json() const {
    json::Json value = json::Json();
    value[Acl::ACL] = m_acl;
    value[AclRule::RULE_ID] = m_rule_id;
    value[AclRule::ACTION] = m_action;
    value[AclRule::FORWARD_MIRROR_PORT] = m_forward_mirror_port;
    value[AclRule::MIRRORED_PORTS] = m_mirrored_ports.to_json();
    value[AclRule::MIRROR_TYPE] = m_mirror_type;
    value[AclRule::VLAN_ID] = m_vlan_id.to_json();
    value[AclRule::SOURCE_IP] = m_source_ip.to_json();
    value[AclRule::DESTINATION_IP] = m_destination_ip.to_json();
    value[AclRule::SOURCE_MAC] = m_source_mac.to_json();
    value[AclRule::DESTINATION_MAC] = m_destination_mac.to_json();
    value[AclRule::SOURCE_L4_PORT] = m_source_port.to_json();
    value[AclRule::DESTINATION_L4_PORT] = m_destination_port.to_json();
    value[AclRule::PROTOCOL] = m_protocol;
    value[AclRule::OEM] = m_oem.to_json();
    return value;
}


AddAclRule AddAclRule::from_json(const json::Json& json) {
    return AddAclRule{
        json[Acl::ACL],
        json.value(AclRule::RULE_ID, 0),
        json[AclRule::ACTION],
        json.value(AclRule::FORWARD_MIRROR_PORT, OptionalField<std::string>()),
        MirroredPorts::from_json(json.value(AclRule::MIRRORED_PORTS, json::Json::array())),
        json.value(AclRule::MIRROR_TYPE, OptionalField<enums::AclMirrorType>()),
        VlanId::from_json(json.value(AclRule::VLAN_ID, json::Json::object())),
        Ip::from_json(json.value(AclRule::SOURCE_IP, json::Json::object())),
        Ip::from_json(json.value(AclRule::DESTINATION_IP, json::Json::object())),
        Mac::from_json(json.value(AclRule::SOURCE_MAC, json::Json::object())),
        Mac::from_json(json.value(AclRule::DESTINATION_MAC, json::Json::object())),
        Port::from_json(json.value(AclRule::SOURCE_L4_PORT, json::Json::object())),
        Port::from_json(json.value(AclRule::DESTINATION_L4_PORT, json::Json::object())),
        json.value(AclRule::PROTOCOL, OptionalField<uint32_t>()),
        Oem::from_json(json.value(AclRule::OEM, json::Json::object()))
    };
}
