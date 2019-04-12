/*!
 * @brief Definition of iSCSI boot attribute
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file iscsi_boot.cpp
 */

#include "agent-framework/module/model/attributes/iscsi_boot.hpp"
#include "agent-framework/module/constants/compute.hpp"
#include "json-wrapper/json-wrapper.hpp"



using namespace agent_framework::model::attribute;


IscsiBoot::IscsiBoot() { }


IscsiBoot::~IscsiBoot() { }


json::Json IscsiBoot::to_json() const {
    json::Json json = json::Json();
    json[literals::IscsiBoot::IP_ADDRESS_TYPE] = m_ip_address_type.to_string();
    json[literals::IscsiBoot::INITIATOR_IP_ADDRESS] = m_initiator_address;
    json[literals::IscsiBoot::INITIATOR_NAME] = m_initiator_name;
    json[literals::IscsiBoot::INITIATOR_DEFAULT_GATEWAY] = m_initiator_default_gateway;
    json[literals::IscsiBoot::INITIATOR_NETMASK] = m_initiator_netmask;
    json[literals::IscsiBoot::TARGET_INFO_VIA_DHCP] = m_target_info_via_dhcp;
    json[literals::IscsiBoot::IP_MASK_DNS_VIA_DHCP] = m_ip_mask_dns_via_dhcp;
    json[literals::IscsiBoot::ROUTER_ADVERTISEMENT_ENABLED] = m_router_advertisement_enabled;
    json[literals::IscsiBoot::PRIMARY_TARGET_IP_ADDRESS] = m_primary_target_address;
    json[literals::IscsiBoot::PRIMARY_TARGET_NAME] = m_primary_target_name;
    json[literals::IscsiBoot::PRIMARY_TARGET_TCP_PORT] = m_primary_target_port;
    json[literals::IscsiBoot::PRIMARY_LUN] = m_primary_lun;
    json[literals::IscsiBoot::PRIMARY_VLAN_ENABLE] = m_primary_vlan_enable;
    json[literals::IscsiBoot::PRIMARY_VLAN_ID] = m_primary_vlan_id;
    json[literals::IscsiBoot::PRIMARY_DNS] = m_primary_dns;
    json[literals::IscsiBoot::SECONDARY_TARGET_IP_ADDRESS] = m_secondary_target_address;
    json[literals::IscsiBoot::SECONDARY_TARGET_NAME] = m_secondary_target_name;
    json[literals::IscsiBoot::SECONDARY_TARGET_TCP_PORT] = m_secondary_target_port;
    json[literals::IscsiBoot::SECONDARY_LUN] = m_secondary_lun;
    json[literals::IscsiBoot::SECONDARY_VLAN_ENABLE] = m_secondary_vlan_enable;
    json[literals::IscsiBoot::SECONDARY_VLAN_ID] = m_secondary_vlan_id;
    json[literals::IscsiBoot::SECONDARY_DNS] = m_secondary_dns;
    json[literals::IscsiBoot::AUTHENTICATION_METHOD] = m_authentication_method.to_string();
    json[literals::IscsiBoot::CHAP_USERNAME] = m_chap_username;
    json[literals::IscsiBoot::CHAP_SECRET] = json::Json{};
    json[literals::IscsiBoot::MUTUAL_CHAP_USERNAME] = m_mutual_chap_username;
    json[literals::IscsiBoot::MUTUAL_CHAP_SECRET] = json::Json{};
    return json;
}


IscsiBoot IscsiBoot::from_json(const json::Json& json) {
    attribute::IscsiBoot boot;
    boot.set_ip_address_type(enums::IPAddressType::from_string(json[literals::IscsiBoot::IP_ADDRESS_TYPE]));
    boot.set_initiator_address(json[literals::IscsiBoot::INITIATOR_IP_ADDRESS]);
    boot.set_initiator_name(json[literals::IscsiBoot::INITIATOR_NAME]);
    boot.set_initiator_default_gateway(json[literals::IscsiBoot::INITIATOR_DEFAULT_GATEWAY]);
    boot.set_initiator_netmask(json[literals::IscsiBoot::INITIATOR_NETMASK]);
    boot.set_target_info_via_dhcp(json[literals::IscsiBoot::TARGET_INFO_VIA_DHCP]);
    boot.set_ip_mask_dns_via_dhcp(json[literals::IscsiBoot::IP_MASK_DNS_VIA_DHCP]);
    boot.set_router_advertisement_enabled(json[literals::IscsiBoot::ROUTER_ADVERTISEMENT_ENABLED]);

    boot.set_primary_target_address(json[literals::IscsiBoot::PRIMARY_TARGET_IP_ADDRESS]);
    boot.set_primary_target_name(json[literals::IscsiBoot::PRIMARY_TARGET_NAME]);
    boot.set_primary_target_port(json[literals::IscsiBoot::PRIMARY_TARGET_TCP_PORT]);
    boot.set_primary_lun(json[literals::IscsiBoot::PRIMARY_LUN]);
    boot.set_primary_vlan_enable(json[literals::IscsiBoot::PRIMARY_VLAN_ENABLE]);
    boot.set_primary_vlan_id(json[literals::IscsiBoot::PRIMARY_VLAN_ID]);
    boot.set_primary_dns(json[literals::IscsiBoot::PRIMARY_DNS]);

    boot.set_secondary_target_address(json[literals::IscsiBoot::SECONDARY_TARGET_IP_ADDRESS]);
    boot.set_secondary_target_name(json[literals::IscsiBoot::SECONDARY_TARGET_NAME]);
    boot.set_secondary_target_port(json[literals::IscsiBoot::SECONDARY_TARGET_TCP_PORT]);
    boot.set_secondary_lun(json[literals::IscsiBoot::SECONDARY_LUN]);
    boot.set_secondary_vlan_enable(json[literals::IscsiBoot::SECONDARY_VLAN_ENABLE]);
    boot.set_secondary_vlan_id(json[literals::IscsiBoot::SECONDARY_VLAN_ID]);
    boot.set_secondary_dns(json[literals::IscsiBoot::SECONDARY_DNS]);

    boot.set_authentication_method(
        enums::FunctionAuthenticationMethod::from_string(json[literals::IscsiBoot::AUTHENTICATION_METHOD]));
    boot.set_chap_username(json[literals::IscsiBoot::CHAP_USERNAME]);
    boot.set_chap_secret(json[literals::IscsiBoot::CHAP_SECRET]);
    boot.set_mutual_chap_username(json[literals::IscsiBoot::MUTUAL_CHAP_USERNAME]);
    boot.set_mutual_chap_secret(json[literals::IscsiBoot::MUTUAL_CHAP_SECRET]);
    return boot;
}
