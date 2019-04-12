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

#include "agent-framework/module/model/network_interface.hpp"
#include "agent-framework/module/constants/compute.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component NetworkInterface::component = enums::Component::NetworkInterface;
const enums::CollectionName NetworkInterface::collection_name = enums::CollectionName::NetworkInterfaces;

NetworkInterface::NetworkInterface(const std::string& parent_uuid, enums::Component parent_type) :
    Resource{parent_uuid, parent_type} {}

NetworkInterface::~NetworkInterface() {}

json::Json NetworkInterface::to_json() const {
    json::Json result = json::Json();
    result[literals::NetworkInterface::STATUS] = get_status().to_json();
    result[literals::NetworkInterface::FRAME_SIZE] = get_frame_size();
    result[literals::NetworkInterface::SPEED] = get_speed_mbps();
    result[literals::NetworkInterface::AUTO_SENSE] = get_autosense();
    result[literals::NetworkInterface::FULL_DUPLEX] = get_full_duplex();
    result[literals::NetworkInterface::MAC_ADDRESS] = get_mac_address();
    result[literals::NetworkInterface::FACTORY_MAC_ADDRESS] = get_factory_mac_address();
    result[literals::NetworkInterface::IPv4_ADDRESSES] = get_ipv4_addresses().to_json();
    result[literals::NetworkInterface::IPv6_ADDRESSES] = get_ipv6_addresses().to_json();
    result[literals::NetworkInterface::IPv6_DEFAULT_GATEWAY] = get_ipv6_default_gateway();
    result[literals::NetworkInterface::MAX_IPv6_STATIC_ADDRESSES] = get_max_ipv6_static_addresses();
    result[literals::NetworkInterface::DEFAULT_VLAN] = get_default_vlan();
    result[literals::NetworkInterface::SUPPORTED_PROTOCOLS] = get_supported_transport_protocols().to_json();
    result[literals::NetworkInterface::COLLECTIONS] = get_collections().to_json();
    result[literals::NetworkInterface::OEM] = get_oem().to_json();
    return result;
}

NetworkInterface NetworkInterface::from_json(const json::Json& json) {
    NetworkInterface interface{};

    interface.set_status(attribute::Status::from_json(json[literals::NetworkInterface::STATUS]));
    interface.set_frame_size(json[literals::NetworkInterface::FRAME_SIZE]);
    interface.set_speed_mbps(json[literals::NetworkInterface::SPEED]);
    interface.set_autosense(json[literals::NetworkInterface::AUTO_SENSE]);
    interface.set_full_duplex(json[literals::NetworkInterface::FULL_DUPLEX]);
    interface.set_mac_address(json[literals::NetworkInterface::MAC_ADDRESS]);
    interface.set_factory_mac_address(json[literals::NetworkInterface::FACTORY_MAC_ADDRESS]);
    interface.set_ipv4_addresses(Ipv4Addresses::from_json(json[literals::NetworkInterface::IPv4_ADDRESSES]));
    interface.set_ipv6_addresses(Ipv6Addresses::from_json(json[literals::NetworkInterface::IPv6_ADDRESSES]));
    interface.set_ipv6_default_gateway(json[literals::NetworkInterface::IPv6_DEFAULT_GATEWAY]);
    interface.set_max_ipv6_static_addresses(json[literals::NetworkInterface::MAX_IPv6_STATIC_ADDRESSES]);
    interface.set_default_vlan(json[literals::NetworkInterface::DEFAULT_VLAN]);
    interface.set_supported_transport_protocols(TransportProtocols::from_json(json[literals::NetworkInterface::SUPPORTED_PROTOCOLS]));
    interface.set_collections(Collections::from_json(json[literals::NetworkInterface::COLLECTIONS]));
    interface.set_oem(attribute::Oem::from_json(json[literals::NetworkInterface::OEM]));

    return interface;
}
