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

#include "agent-framework/module/model/ethernet_switch.hpp"
#include "agent-framework/module/constants/network.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component EthernetSwitch::component =
    enums::Component::EthernetSwitch;
const enums::CollectionName EthernetSwitch::collection_name =
    enums::CollectionName::EthernetSwitches;

EthernetSwitch::EthernetSwitch(const std::string& parent_uuid, enums::Component parent_type) :
    Resource{parent_uuid, parent_type} {}

EthernetSwitch::~EthernetSwitch() {}

json::Json EthernetSwitch::to_json() const {
    json::Json result = json::Json();
    result[literals::EthernetSwitch::STATUS] = get_status().to_json();
    result[literals::EthernetSwitch::SWITCH_IDENTIFIER] = get_switch_identifier();
    result[literals::EthernetSwitch::TECHNOLOGY] = get_technology();
    result[literals::EthernetSwitch::MAC_ADDRESS] = get_mac_address();
    result[literals::EthernetSwitch::FIRMWARE_NAME] = get_firmware_name();
    result[literals::EthernetSwitch::FIRMWARE_VERSION] = get_firmware_version();
    result[literals::EthernetSwitch::ROLE] = get_role();
    result[literals::EthernetSwitch::FRU_INFO] = get_fru_info().to_json();
    result[literals::EthernetSwitch::MANUFACTURING_DATE] = get_manufacturing_date();
    result[literals::EthernetSwitch::LOCATION] = get_location();
    result[literals::EthernetSwitch::CHASSIS] = get_chassis();
    result[literals::EthernetSwitch::MAX_ACL_NUMBER] = get_max_acl_number();
    result[literals::EthernetSwitch::LLDP_ENABLED] = get_lldp_enabled();
    result[literals::EthernetSwitch::PFC_ENABLED] = get_pfc_enabled();
    result[literals::EthernetSwitch::ETS_ENABLED] = get_ets_enabled();
    result[literals::EthernetSwitch::DCBX_ENABLED] = get_dcbx_enabled();
    result[literals::EthernetSwitch::QOS_APPLICATION_PROTOCOL] = get_qos_application_protocol().to_json();
    result[literals::EthernetSwitch::QOS_PRIORITY_TO_PRIORITY_GROUP_MAPPING] = get_qos_priority_group_mapping().to_json();
    result[literals::EthernetSwitch::QOS_BANDWIDTH_ALLOCATION] = get_qos_bandwidth_allocation().to_json();
    result[literals::EthernetSwitch::COLLECTIONS] = get_collections().to_json();
    result[literals::EthernetSwitch::OEM] = get_oem().to_json();
    return result;
}

EthernetSwitch EthernetSwitch::from_json(const json::Json& json) {
    EthernetSwitch sw;

    sw.set_status(attribute::Status::from_json(json[literals::EthernetSwitch::STATUS]));
    sw.set_switch_identifier(
        json[literals::EthernetSwitch::SWITCH_IDENTIFIER]);
    sw.set_technology(json[literals::EthernetSwitch::TECHNOLOGY]);
    sw.set_mac_address(json[literals::EthernetSwitch::MAC_ADDRESS]);
    sw.set_firmware_name(json[literals::EthernetSwitch::FIRMWARE_NAME]);
    sw.set_firmware_version(json[literals::EthernetSwitch::FIRMWARE_VERSION]);
    sw.set_role(json[literals::EthernetSwitch::ROLE]);
    sw.set_fru_info(attribute::FruInfo::from_json(
        json[literals::EthernetSwitch::FRU_INFO]));
    sw.set_manufacturing_date(json[literals::EthernetSwitch::MANUFACTURING_DATE]);
    sw.set_location(json[literals::EthernetSwitch::LOCATION]);
    sw.set_chassis(json[literals::EthernetSwitch::CHASSIS]);
    sw.set_max_acl_number(json[literals::EthernetSwitch::MAX_ACL_NUMBER]);
    sw.set_lldp_enabled(json[literals::EthernetSwitch::LLDP_ENABLED]);
    sw.set_pfc_enabled(json[literals::EthernetSwitch::PFC_ENABLED]);
    sw.set_ets_enabled(json[literals::EthernetSwitch::ETS_ENABLED]);
    sw.set_dcbx_enabled(json[literals::EthernetSwitch::DCBX_ENABLED]);
    sw.set_qos_application_protocol(attribute::Array<attribute::QosApplicationProtocol>::from_json(
        json[literals::EthernetSwitch::QOS_APPLICATION_PROTOCOL]));
    sw.set_qos_priority_group_mapping(attribute::Array<attribute::QosPriorityGroupMapping>::from_json(
        json[literals::EthernetSwitch::QOS_PRIORITY_TO_PRIORITY_GROUP_MAPPING]));
    sw.set_qos_bandwidth_allocation(attribute::Array<attribute::QosBandwidthAllocation>::from_json(
        json[literals::EthernetSwitch::QOS_BANDWIDTH_ALLOCATION]));
    sw.set_collections(Collections::from_json(
        json[literals::EthernetSwitch::COLLECTIONS]));
    sw.set_oem(attribute::Oem::from_json(json[literals::EthernetSwitch::OEM]));

    return sw;
}
