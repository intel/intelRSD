/*!
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
 * */

#include "psme/rest/constants/constants.hpp"
#include "psme/rest/endpoints/system/system_network_interface.hpp"
#include "psme/rest/utils/status_helpers.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;
using namespace agent_framework::module;

namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#EthernetInterface.EthernetInterface";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#EthernetInterface.v1_1_0.EthernetInterface";
    r[Common::ID] = json::Json::value_t::null;
    r[Common::NAME] = "Ethernet Interface";
    r[Common::DESCRIPTION] = "Ethernet Interface description";
    r[Common::STATUS][Common::STATE] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Json::value_t::null;
    r[Common::OEM][Common::RACKSCALE][Common::ODATA_TYPE] = "#Intel.Oem.EthernetInterface";
    r[Common::OEM][Common::RACKSCALE][NetworkInterface::SUPPORTED_PROTOCOLS] = json::Json::value_t::array;
    r[constants::NetworkInterface::PERMANENT_MAC_ADDRESS] = json::Json::value_t::null;
    r[constants::Common::MAC_ADDRESS] = json::Json::value_t::null;
    r[constants::NetworkInterface::SPEED_MBPS] = json::Json::value_t::null;
    r[constants::NetworkInterface::AUTO_NEG] = json::Json::value_t::null;
    r[constants::NetworkInterface::FULL_DUPLEX] = json::Json::value_t::null;
    r[constants::NetworkInterface::HOST_NAME] = json::Json::value_t::null;
    r[constants::NetworkInterface::FQDN] = json::Json::value_t::null;
    r[constants::NetworkInterface::MTU_SIZE] = json::Json::value_t::null;
    r[constants::NetworkInterface::LINK_STATUS] = json::Json::value_t::null;

    r[constants::NetworkInterface::IPv4_ADDRESSES] = json::Json::value_t::array;

    r[constants::NetworkInterface::IPv6_STATIC_ADDRESSES] = json::Json::value_t::array;

    r[constants::NetworkInterface::IPv6_ADDRESS_POLICY_TABLE] = json::Json::value_t::array;

    r[constants::NetworkInterface::MAX_IPv6_STATIC_ADDRESSES] = json::Json::value_t::null;
    r[constants::NetworkInterface::IPv6_ADDRESSES] = json::Json::value_t::array;
    r[constants::NetworkInterface::IPv6_DEFAULT_GATEWAY] = json::Json::value_t::null;
    r[constants::NetworkInterface::NAME_SERVERS] = json::Json::value_t::array;
    r[constants::NetworkInterface::VLAN] = json::Json::value_t::null;
    r[constants::NetworkInterface::INTERFACE_ENABLED] = json::Json::value_t::null;

    json::Json links = json::Json();
    links[Fabric::ENDPOINTS] = json::Json::value_t::array;
    links[Common::OEM][Common::RACKSCALE][Common::ODATA_TYPE] = "#Intel.Oem.EthernetInterfaceLinks";
    links[Common::OEM][Common::RACKSCALE][NetworkInterface::NEIGHBOR_PORT] = json::Json::value_t::null;
    r[Common::LINKS] = std::move(links);
    return r;
}
}


endpoint::SystemNetworkInterface::SystemNetworkInterface(const std::string& path) : EndpointBase(path) {}


endpoint::SystemNetworkInterface::~SystemNetworkInterface() {}


void endpoint::SystemNetworkInterface::get(const server::Request& req, server::Response& res) {
    auto r = ::make_prototype();

    r[constants::Common::ODATA_ID] = PathBuilder(req).build();
    r[constants::Common::ID] = req.params[PathParam::NIC_ID];

    auto ni = psme::rest::model::find<agent_framework::model::System, agent_framework::model::NetworkInterface>(
        req.params).get();

    endpoint::status_to_json(ni, r);

    r[constants::NetworkInterface::PERMANENT_MAC_ADDRESS] = ni.get_factory_mac_address();
    r[constants::Common::MAC_ADDRESS] = ni.get_mac_address();
    r[constants::NetworkInterface::SPEED_MBPS] = ni.get_speed_mbps();
    r[constants::NetworkInterface::AUTO_NEG] = ni.get_autosense();
    r[constants::NetworkInterface::FULL_DUPLEX] = ni.get_full_duplex();
    r[constants::NetworkInterface::MTU_SIZE] = ni.get_frame_size();
    r[constants::NetworkInterface::MAX_IPv6_STATIC_ADDRESSES] = ni.get_max_ipv6_static_addresses();

    const auto& ipv4_addresses = ni.get_ipv4_addresses();
    for (const auto& ipv4_addr : ipv4_addresses) {
        if (ipv4_addr.get_address().has_value()) {
            json::Json ipv4_address = json::Json();
            ipv4_address[IpAddress::ADDRESS] = ipv4_addr.get_address();
            ipv4_address[IpAddress::SUBNET_MASK] = ipv4_addr.get_subnet_mask();
            ipv4_address[IpAddress::ADDRESS_ORIGIN] = ipv4_addr.get_address_origin();
            ipv4_address[IpAddress::GATEWAY] = ipv4_addr.get_gateway();
            r[constants::NetworkInterface::IPv4_ADDRESSES].push_back(std::move(ipv4_address));
        }
    }

    const auto& ipv6_addresses = ni.get_ipv6_addresses();
    for (const auto& ipv6_addr : ipv6_addresses) {
        if (ipv6_addr.get_address().has_value()) {
            json::Json ipv6_address = json::Json();
            ipv6_address[IpAddress::ADDRESS] = ipv6_addr.get_address();
            ipv6_address[IpAddress::PREFIX_LENGTH] = ipv6_addr.get_prefix_length();
            // in GAMI there is DHCP option which has to be shown as DHCPv6
            auto address_origin = ipv6_addr.get_address_origin();
            ipv6_address[IpAddress::ADDRESS_ORIGIN] =
                address_origin == agent_framework::model::enums::Ipv6AddressOrigin::DHCP ?
                json::Json("DHCPv6") : json::Json(address_origin);
            ipv6_address[IpAddress::ADDRESS_STATE] = ipv6_addr.get_address_state();
            r[constants::NetworkInterface::IPv6_ADDRESSES].push_back(std::move(ipv6_address));
        }
    }

    r[constants::NetworkInterface::IPv6_DEFAULT_GATEWAY] = ni.get_ipv6_default_gateway();
    if (ni.get_status().get_state() == agent_framework::model::enums::State::Enabled) {
        r[constants::NetworkInterface::INTERFACE_ENABLED] = true;
    }
    else {
        r[constants::NetworkInterface::INTERFACE_ENABLED] = false;
    }

    const auto& supported_protocols = ni.get_supported_transport_protocols();
    for (const auto& supported_protocol : supported_protocols) {
        r[Common::OEM][Common::RACKSCALE][NetworkInterface::SUPPORTED_PROTOCOLS]
            .push_back(supported_protocol.to_string());
    }

    set_response(res, r);
}
