/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * */

#include "agent-framework/module/model/ethernet_switch_port.hpp"
#include "agent-framework/module/constants/network.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component EthernetSwitchPort::component = enums::Component::EthernetSwitchPort;
const enums::CollectionName EthernetSwitchPort::collection_name =
    enums::CollectionName::EthernetSwitchPorts;

EthernetSwitchPort::EthernetSwitchPort(const std::string& parent_uuid, enums::Component parent_type) :
    Resource{parent_uuid, parent_type} {}

EthernetSwitchPort::~EthernetSwitchPort() {}

json::Json EthernetSwitchPort::to_json() const {
    json::Json result = json::Json();
    result[literals::EthernetSwitchPort::STATUS] = get_status().to_json();
    result[literals::EthernetSwitchPort::PORT_IDENTIFIER] = get_port_identifier();
    result[literals::EthernetSwitchPort::PORT_CLASS] = get_port_class();
    result[literals::EthernetSwitchPort::PORT_TYPE] = get_port_type();
    result[literals::EthernetSwitchPort::PORT_MODE] = get_port_mode();
    result[literals::EthernetSwitchPort::LINK_TECHNOLOGY] = get_link_technology();
    result[literals::EthernetSwitchPort::LINK_SPEED_MBPS] = get_link_speed_mbps();
    result[literals::EthernetSwitchPort::MAX_SPEED_MBPS] = get_max_speed_mbps();
    result[literals::EthernetSwitchPort::OPERATIONAL_STATE] = get_operational_state();
    result[literals::EthernetSwitchPort::ADMINISTRATIVE_STATE] = get_administrative_state();
    result[literals::EthernetSwitchPort::PORT_WIDTH] = get_port_width();
    result[literals::EthernetSwitchPort::FRAME_SIZE] = get_frame_size();
    result[literals::EthernetSwitchPort::AUTO_SENSE] = get_auto_sense();
    result[literals::EthernetSwitchPort::FULL_DUPLEX] = get_full_duplex();
    result[literals::EthernetSwitchPort::IS_MANAGEMENT_PORT] = get_is_management_port();
    result[literals::EthernetSwitchPort::LAST_ERROR_CODE] = get_last_error_code();
    result[literals::EthernetSwitchPort::ERROR_CLEARED] = get_error_cleared();
    result[literals::EthernetSwitchPort::LAST_STATE_CHANGE_TIME] = get_last_state_change_time();
    result[literals::EthernetSwitchPort::MAC_ADDRESS] = get_mac_address();
    result[literals::EthernetSwitchPort::IPV4_ADDRESS] = get_ipv4_address().to_json();
    result[literals::EthernetSwitchPort::IPV6_ADDRESS] = get_ipv6_address().to_json();
    result[literals::EthernetSwitchPort::NEIGHBOR_INFO] = get_neighbor_info().to_json();
    result[literals::EthernetSwitchPort::NEIGHBOR_MAC] = get_neighbor_mac();
    result[literals::EthernetSwitchPort::VLAN_ENABLE] = get_vlan_enable();
    result[literals::EthernetSwitchPort::DEFAULT_VLAN] = get_default_vlan();
    result[literals::EthernetSwitchPort::LLDP_ENABLED] = get_lldp_enabled();
    result[literals::EthernetSwitchPort::PFC_ENABLED] = get_pfc_enabled();
    result[literals::EthernetSwitchPort::PFC_ENABLED_PRIORITIES] = get_pfc_enabled_priorities().to_json();
    result[literals::EthernetSwitchPort::DCBX_STATE] = get_dcbx_state();
    result[literals::EthernetSwitchPort::COLLECTIONS] = get_collections().to_json();
    result[literals::EthernetSwitchPort::OEM] = get_oem().to_json();
    return result;
}

EthernetSwitchPort EthernetSwitchPort::from_json(const json::Json& json) {
    EthernetSwitchPort port;

    port.set_status(attribute::Status::from_json(json[literals::EthernetSwitchPort::STATUS]));
    port.set_port_identifier(json[literals::EthernetSwitchPort::PORT_IDENTIFIER]);
    port.set_port_class(json[literals::EthernetSwitchPort::PORT_CLASS]);
    port.set_port_type(json[literals::EthernetSwitchPort::PORT_TYPE]);
    port.set_port_mode(json[literals::EthernetSwitchPort::PORT_MODE]);
    port.set_link_technology(json[literals::EthernetSwitchPort::LINK_TECHNOLOGY]);
    port.set_link_speed_mbps(json[literals::EthernetSwitchPort::LINK_SPEED_MBPS]);
    port.set_max_speed_mbps(json[literals::EthernetSwitchPort::MAX_SPEED_MBPS]);
    port.set_operational_state(json[literals::EthernetSwitchPort::OPERATIONAL_STATE]);
    port.set_administrative_state(json[literals::EthernetSwitchPort::ADMINISTRATIVE_STATE]);
    port.set_port_width(json[literals::EthernetSwitchPort::PORT_WIDTH]);
    port.set_frame_size(json[literals::EthernetSwitchPort::FRAME_SIZE]);
    port.set_auto_sense(json[literals::EthernetSwitchPort::AUTO_SENSE]);
    port.set_full_duplex(json[literals::EthernetSwitchPort::FULL_DUPLEX]);
    port.set_is_management_port(json[literals::EthernetSwitchPort::IS_MANAGEMENT_PORT]);
    port.set_last_error_code(json[literals::EthernetSwitchPort::LAST_ERROR_CODE]);
    port.set_error_cleared(json[literals::EthernetSwitchPort::ERROR_CLEARED]);
    port.set_last_state_change_time(json[literals::EthernetSwitchPort::LAST_STATE_CHANGE_TIME]);
    port.set_mac_address(json[literals::EthernetSwitchPort::MAC_ADDRESS]);
    port.set_ipv4_address(attribute::Ipv4Address::from_json(json[literals::EthernetSwitchPort::IPV4_ADDRESS]));
    port.set_ipv6_address(attribute::Ipv6Address::from_json(json[literals::EthernetSwitchPort::IPV6_ADDRESS]));
    port.set_neighbor_info(attribute::NeighborInfo::from_json(json[literals::EthernetSwitchPort::NEIGHBOR_INFO]));
    port.set_neighbor_mac(json[literals::EthernetSwitchPort::NEIGHBOR_MAC]);
    port.set_vlan_enable(json[literals::EthernetSwitchPort::VLAN_ENABLE]);
    port.set_default_vlan(json[literals::EthernetSwitchPort::DEFAULT_VLAN]);
    port.set_lldp_enabled(json[literals::EthernetSwitchPort::LLDP_ENABLED]);
    port.set_pfc_enabled(json[literals::EthernetSwitchPort::PFC_ENABLED]);
    port.set_pfc_enabled_priorities(attribute::Array<std::uint32_t>::from_json(json[literals::EthernetSwitchPort::PFC_ENABLED_PRIORITIES]));
    port.set_dcbx_state(json[literals::EthernetSwitchPort::DCBX_STATE]);
    port.set_collections(Collections::from_json(json[literals::EthernetSwitchPort::COLLECTIONS]));
    port.set_oem(attribute::Oem::from_json(json[literals::EthernetSwitchPort::OEM]));

    return port;
}
