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
 * */

#include "agent-framework/module/utils/utils.hpp"
#include "agent-framework/module/model/acl_rule.hpp"
#include "agent-framework/module/constants/network.hpp"
#include "agent-framework/module/constants/common.hpp"



using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component AclRule::component = enums::Component::AclRule;
const enums::CollectionName AclRule::collection_name = enums::CollectionName::AclRules;


AclRule::AclRule(const std::string& parent_uuid, enums::Component parent_type) :
    Resource{parent_uuid, parent_type} {}


AclRule::~AclRule() {}


json::Json AclRule::to_json() const {
    json::Json value = json::Json();
    value[literals::AclRule::STATUS] = get_status().to_json();
    value[literals::AclRule::RULE_ID] = get_rule_id();
    value[literals::AclRule::ACTION] = get_action();
    value[literals::AclRule::FORWARD_MIRROR_PORT] = get_forward_mirror_port();
    value[literals::AclRule::MIRRORED_PORTS] = get_mirrored_ports().to_json();
    value[literals::AclRule::MIRROR_TYPE] = get_mirror_type();
    value[literals::AclRule::VLAN_ID] = get_vlan_id().to_json();
    value[literals::AclRule::SOURCE_IP] = get_source_ip().to_json();
    value[literals::AclRule::DESTINATION_IP] = get_destination_ip().to_json();
    value[literals::AclRule::SOURCE_MAC] = get_source_mac().to_json();
    value[literals::AclRule::DESTINATION_MAC] = get_destination_mac().to_json();
    value[literals::AclRule::SOURCE_L4_PORT] = get_source_port().to_json();
    value[literals::AclRule::DESTINATION_L4_PORT] = get_destination_port().to_json();
    value[literals::AclRule::PROTOCOL] = get_protocol();
    value[literals::AclRule::OEM] = get_oem().to_json();
    return value;
}


AclRule AclRule::from_json(const json::Json& json) {
    AclRule rule;

    rule.set_status(attribute::Status::from_json(json[literals::Acl::STATUS]));
    rule.set_rule_id(json[literals::AclRule::RULE_ID]);
    rule.set_action(json[literals::AclRule::ACTION]);
    rule.set_forward_mirror_port(json[literals::AclRule::FORWARD_MIRROR_PORT]);
    rule.set_mirrored_ports(MirroredPorts::from_json(json[literals::AclRule::MIRRORED_PORTS]));
    rule.set_mirror_type(json[literals::AclRule::MIRROR_TYPE]);
    rule.set_vlan_id(VlanId::from_json(json[literals::AclRule::VLAN_ID]));
    rule.set_source_ip(Ip::from_json(json[literals::AclRule::SOURCE_IP]));
    rule.set_destination_ip(Ip::from_json(json[literals::AclRule::DESTINATION_IP]));
    rule.set_source_mac(Mac::from_json(json[literals::AclRule::SOURCE_MAC]));
    rule.set_destination_mac(Mac::from_json(json[literals::AclRule::DESTINATION_MAC]));
    rule.set_source_port(Port::from_json(json[literals::AclRule::SOURCE_L4_PORT]));
    rule.set_destination_port(Port::from_json(json[literals::AclRule::DESTINATION_L4_PORT]));
    rule.set_protocol(json[literals::AclRule::PROTOCOL]);
    rule.set_oem(attribute::Oem::from_json(json[literals::AclRule::OEM]));

    return rule;
}
