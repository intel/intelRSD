/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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

#include "agent-framework/module-ref/model/switch_port.hpp"
#include "agent-framework/module-ref/constants/network.hpp"
#include "agent-framework/module-ref/utils/utils.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component SwitchPort::component = enums::Component::SwitchPort;
const enums::CollectionName SwitchPort::collection_name =
    enums::CollectionName::Ports;

SwitchPort::SwitchPort(const std::string& parent_uuid) :
    Resource{parent_uuid, enums::Component::Switch} {}

SwitchPort::~SwitchPort() {}

Json::Value SwitchPort::to_json() const {
    Json::Value result;
    result[literals::SwitchPort::STATUS] = get_status().to_json();
    result[literals::SwitchPort::PORT_IDENTIFIER] = get_port_identifier();
    result[literals::SwitchPort::PORT_CLASS] = get_port_class();
    result[literals::SwitchPort::PORT_TYPE] = get_port_type();
    result[literals::SwitchPort::PORT_MODE] = get_port_mode();
    result[literals::SwitchPort::LINK_TECHNOLOGY] =
        get_link_technology();
    result[literals::SwitchPort::LINK_SPEED_MBPS] = get_link_speed_mbps();
    result[literals::SwitchPort::MAX_SPEED_MBPS] = get_max_speed_mbps();
    result[literals::SwitchPort::OPERATIONAL_STATE] = get_operational_state();
    result[literals::SwitchPort::ADMINISTRATIVE_STATE] =
        get_administrative_state();
    result[literals::SwitchPort::PORT_WIDTH] = get_port_width();
    result[literals::SwitchPort::FRAME_SIZE] = get_frame_size();
    result[literals::SwitchPort::AUTO_SENSE] = get_auto_sense();
    result[literals::SwitchPort::FULL_DUPLEX] = get_full_duplex();
    result[literals::SwitchPort::IS_MANAGEMENT_PORT] =
        get_is_management_port();
    result[literals::SwitchPort::LAST_ERROR_CODE] = get_last_error_code();
    result[literals::SwitchPort::ERROR_CLEARED] = get_error_cleared();
    result[literals::SwitchPort::LAST_STATE_CHANGE_TIME] =
        get_last_state_change_time();
    result[literals::SwitchPort::MAC_ADDRESS] = get_mac_address();
    result[literals::SwitchPort::IPV4_ADDRESS] = get_ipv4_address().to_json();
    result[literals::SwitchPort::IPV6_ADDRESS] = get_ipv6_address().to_json();
    result[literals::SwitchPort::NEIGHBOR_INFO] =
        get_neighbor_info().to_json();
    result[literals::SwitchPort::NEIGHBOR_MAC] = get_neighbor_mac();
    result[literals::SwitchPort::VLAN_ENABLE] = get_vlan_enable();
    result[literals::SwitchPort::DEFAULT_VLAN] = get_default_vlan();
    result[literals::SwitchPort::COLLECTIONS] = get_collections().to_json();
    result[literals::SwitchPort::OEM] = get_oem().to_json();
    return result;
}

SwitchPort SwitchPort::from_json(const Json::Value& json) {
    SwitchPort port;

    port.set_status(attribute::Status::from_json(json[literals::SwitchPort::STATUS]));
    port.set_port_identifier(json[literals::SwitchPort::PORT_IDENTIFIER]);
    port.set_port_class(json[literals::SwitchPort::PORT_CLASS]);
    port.set_port_type(json[literals::SwitchPort::PORT_TYPE]);
    port.set_port_mode(json[literals::SwitchPort::PORT_MODE]);
    port.set_link_technology(json[literals::SwitchPort::LINK_TECHNOLOGY]);
    port.set_link_speed_mbps(json[literals::SwitchPort::LINK_SPEED_MBPS]);
    port.set_max_speed_mbps(
        json[literals::SwitchPort::MAX_SPEED_MBPS]);
    port.set_operational_state(
        json[literals::SwitchPort::OPERATIONAL_STATE]);
    port.set_administrative_state(
        json[literals::SwitchPort::ADMINISTRATIVE_STATE]);
    port.set_port_width(json[literals::SwitchPort::PORT_WIDTH]);
    port.set_frame_size(json[literals::SwitchPort::FRAME_SIZE]);
    port.set_auto_sense(json[literals::SwitchPort::AUTO_SENSE]);
    port.set_full_duplex(json[literals::SwitchPort::FULL_DUPLEX]);
    port.set_is_management_port(json[literals::SwitchPort::IS_MANAGEMENT_PORT]);
    port.set_last_error_code(json[literals::SwitchPort::LAST_ERROR_CODE]);
    port.set_error_cleared(json[literals::SwitchPort::ERROR_CLEARED]);
    port.set_last_state_change_time(
        json[literals::SwitchPort::LAST_STATE_CHANGE_TIME]);
    port.set_mac_address(
        json[literals::SwitchPort::MAC_ADDRESS]);
    port.set_ipv4_address(attribute::Ipv4Address::from_json(
        json[literals::SwitchPort::IPV4_ADDRESS]));
    port.set_ipv6_address(attribute::Ipv6Address::from_json(
        json[literals::SwitchPort::IPV6_ADDRESS]));
    port.set_neighbor_info(attribute::NeighborInfo::from_json(
        json[literals::SwitchPort::NEIGHBOR_INFO]));
    port.set_neighbor_mac(
        json[literals::SwitchPort::NEIGHBOR_MAC]);
    port.set_vlan_enable(json[literals::SwitchPort::VLAN_ENABLE]);
    port.set_default_vlan(
        json[literals::SwitchPort::DEFAULT_VLAN]);
    port.set_collections(Collections::from_json(
        json[literals::SwitchPort::COLLECTIONS]));
    port.set_oem(attribute::Oem::from_json(json[literals::SwitchPort::OEM]));
    port.set_resource_hash(compute_hash(json));

    return port;
}
