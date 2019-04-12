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

#include "psme/rest/endpoints/manager/manager_network_interface.hpp"
#include "psme/rest/constants/constants.hpp"
#include "psme/rest/utils/status_helpers.hpp"
#include "configuration/configuration.hpp"
#include "net/network_interface.hpp"
#include <sstream>



using namespace psme::rest;
using namespace psme::rest::constants;
using namespace agent_framework::model::enums;

namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#EthernetInterface.EthernetInterface";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#EthernetInterface.v1_1_0.EthernetInterface";
    r[Common::ID] = json::Json::value_t::null;
    r[Common::NAME] = "Manager NIC";
    r[Common::DESCRIPTION] = "Manager NIC description";
    r[Common::STATUS][Common::STATE] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Json::value_t::null;
    r[Common::MAC_ADDRESS] = json::Json::value_t::null;
    r[NetworkInterface::PERMANENT_MAC_ADDRESS] = json::Json::value_t::null;
    r[NetworkInterface::SPEED_MBPS] = json::Json::value_t::null;
    r[NetworkInterface::AUTO_NEG] = json::Json::value_t::null;
    r[NetworkInterface::FULL_DUPLEX] = json::Json::value_t::null;
    r[NetworkInterface::MTU_SIZE] = json::Json::value_t::null;
    r[NetworkInterface::HOST_NAME] = json::Json::value_t::null;
    r[NetworkInterface::FQDN] = json::Json::value_t::null;
    r[NetworkInterface::MAX_IPv6_STATIC_ADDRESSES] = json::Json::value_t::null;
    r[NetworkInterface::INTERFACE_ENABLED] = json::Json::value_t::null;
    r[NetworkInterface::LINK_STATUS] = json::Json::value_t::null;

    r[NetworkInterface::IPv4_ADDRESSES] = json::Json::value_t::array;
    r[NetworkInterface::IPv6_ADDRESS_POLICY_TABLE] = json::Json::value_t::array;
    r[NetworkInterface::IPv6_ADDRESSES] = json::Json::value_t::array;
    r[NetworkInterface::IPv6_DEFAULT_GATEWAY] = json::Json::value_t::null;
    r[NetworkInterface::IPv6_STATIC_ADDRESSES] = json::Json::value_t::array;
    r[NetworkInterface::NAME_SERVERS] = json::Json::value_t::array;
    r[NetworkInterface::VLANS] = json::Json::value_t::null;

    json::Json links = json::Json();
    links[Fabric::ENDPOINTS] = json::Json::value_t::array;
    links[Common::OEM][Common::RACKSCALE][Common::ODATA_TYPE] = "#Intel.Oem.EthernetInterfaceLinks";
    links[Common::OEM][Common::RACKSCALE][NetworkInterface::NEIGHBOR_PORT] = json::Json::value_t::null;
    r[Common::LINKS] = std::move(links);

    r[Common::OEM][Common::RACKSCALE][Common::ODATA_TYPE] = "#Intel.Oem.EthernetInterface";
    r[Common::OEM][Common::RACKSCALE][NetworkInterface::SUPPORTED_PROTOCOLS] = json::Json::value_t::array;

    return r;
}


void read_interface_from_hw(json::Json& r, const std::string& nic_name) {
    try {
        auto iface = net::NetworkInterface::for_name(nic_name);

        const auto& address_list = iface.get_address_list();
        for (size_t idx = 0; idx < address_list.size(); ++idx) {
            const auto& ip_address = iface.get_address(idx);
            const auto& subnet_mask = iface.get_subnet_mask(idx);

            if (net::AddressFamily::IPv4 == ip_address.get_address_family()) {
                json::Json ipv4_address;
                ipv4_address[Common::ODATA_TYPE] = "#IPAddresses.v1_0_0.IPv4Address";
                ipv4_address[IpAddress::ADDRESS] = ip_address.to_string();
                ipv4_address[IpAddress::SUBNET_MASK] = subnet_mask.to_string();
                r[NetworkInterface::IPv4_ADDRESSES].push_back(std::move(ipv4_address));
            }
        }
        std::stringstream mac_address_str{};
        mac_address_str << iface.get_hw_address();
        r[Common::MAC_ADDRESS] = mac_address_str.str();
    }
    catch (const std::exception& ex) {
        log_error("rest", "Unable to read data for nic: " + nic_name + ". Exception " << ex.what());
    }
}


void read_interface_from_hw(json::Json& r, std::uint64_t idx) {
    const json::Json config = configuration::Configuration::get_instance().to_json();
    const auto& nic_names = config.value("server", json::Json::object())
        .value("network-interface-name", json::Json::array());
    if (idx == 0 || nic_names.size() < idx) {
        THROW(agent_framework::exceptions::NotFound, "rest", "EthernetInterface entry not found.");
    }
    read_interface_from_hw(r, nic_names[size_t(idx - 1)].get<std::string>());
}


void read_from_model(json::Json& r, const agent_framework::model::Manager& manager) {
    r[Common::MAC_ADDRESS] = json::Json::value_t::null;
    r[NetworkInterface::IPv4_ADDRESSES] = json::Json::value_t::array;

    if (manager.get_ipv4_address().has_value()) {
        json::Json ipv4_address = json::Json();
        ipv4_address[Common::ODATA_TYPE] = "#IPAddresses.v1_0_0.IPv4Address";
        ipv4_address[IpAddress::ADDRESS] = manager.get_ipv4_address();
        ipv4_address[IpAddress::SUBNET_MASK] = json::Json::value_t::null;
        r[NetworkInterface::IPv4_ADDRESSES].push_back(std::move(ipv4_address));
    }
}


void read_from_network_interface(const agent_framework::model::NetworkInterface& nic, json::Json& r) {

    r[NetworkInterface::PERMANENT_MAC_ADDRESS] = nic.get_factory_mac_address();
    r[Common::MAC_ADDRESS] = nic.get_mac_address();
    r[NetworkInterface::SPEED_MBPS] = nic.get_speed_mbps();
    r[NetworkInterface::AUTO_NEG] = nic.get_autosense();
    r[NetworkInterface::FULL_DUPLEX] = nic.get_full_duplex();
    r[NetworkInterface::MTU_SIZE] = nic.get_frame_size();
    r[NetworkInterface::MAX_IPv6_STATIC_ADDRESSES] = nic.get_max_ipv6_static_addresses();
    r[NetworkInterface::INTERFACE_ENABLED] = true;
    r[NetworkInterface::IPv6_DEFAULT_GATEWAY] = nic.get_ipv6_default_gateway();

    const auto& supported_protocols = nic.get_supported_transport_protocols();
    for (const auto& supported_protocol : supported_protocols) {
        r[Common::OEM][Common::RACKSCALE][NetworkInterface::SUPPORTED_PROTOCOLS].push_back(
            supported_protocol.to_string());
    }

    for (const auto& address : nic.get_ipv4_addresses()) {
        json::Json ipv4_address = json::Json();

        ipv4_address[Common::ODATA_TYPE] = "#IPAddresses.v1_0_0.IPv4Address";
        ipv4_address[IpAddress::ADDRESS] = address.get_address();
        ipv4_address[IpAddress::SUBNET_MASK] = address.get_subnet_mask();
        ipv4_address[IpAddress::ADDRESS_ORIGIN] = address.get_address_origin();
        ipv4_address[IpAddress::GATEWAY] = address.get_gateway();

        r[NetworkInterface::IPv4_ADDRESSES].push_back(std::move(ipv4_address));
    }

    for (const auto& address : nic.get_ipv6_addresses()) {
        json::Json ipv6_address = json::Json();

        ipv6_address[Common::ODATA_TYPE] = "#IPAddresses.v1_0_0.IPv6Address";
        ipv6_address[IpAddress::ADDRESS] = address.get_address();
        ipv6_address[IpAddress::PREFIX_LENGTH] = address.get_prefix_length();
        ipv6_address[IpAddress::ADDRESS_ORIGIN] = address.get_address_origin();
        ipv6_address[IpAddress::ADDRESS_STATE] = address.get_address_state();

        r[NetworkInterface::IPv6_ADDRESSES].push_back(std::move(ipv6_address));
    }
    psme::rest::endpoint::status_to_json(nic, r);
}

}


endpoint::ManagerNetworkInterface::ManagerNetworkInterface(const std::string& path) : EndpointBase(path) {}


endpoint::ManagerNetworkInterface::~ManagerNetworkInterface() {}


void endpoint::ManagerNetworkInterface::get(const server::Request& request, server::Response& response) {
    auto id = psme::rest::endpoint::utils::id_to_uint64(request.params[PathParam::NIC_ID]);
    auto manager = psme::rest::model::find<agent_framework::model::Manager>(
        request.params).get();
    auto nic_uuids = agent_framework::module::get_manager<agent_framework::model::NetworkInterface>()
        .get_keys(manager.get_uuid());

    auto r = ::make_prototype();
    r[Common::ODATA_ID] = PathBuilder(request).build();
    r[Common::ID] = request.params[PathParam::NIC_ID];

    r[NetworkInterface::VLANS][Common::ODATA_ID] =
        endpoint::PathBuilder(request).append(NetworkInterface::VLANS).build();

    if (!nic_uuids.empty()) {
        const auto nic = psme::rest::model::find<agent_framework::model::Manager,
                                                 agent_framework::model::NetworkInterface>(request.params).get();
        ::read_from_network_interface(nic, r);
    }
    else {
        // If the manager that has this NIC controls a Drawer or an Enclosure
        // (Chassis of type Drawer or Enclosure) - read data from hardware.
        // If it doesn't - fill this NIC with stub data.
        if (psme::rest::endpoint::utils::is_manager_for_drawer_or_enclosure(manager.get_uuid()) ||
            manager.get_manager_type() == ManagerInfoType::EnclosureManager) {
            ::read_interface_from_hw(r, id);
        }
        else {
            if (id != 1) {
                THROW(agent_framework::exceptions::NotFound, "rest", "EthernetInterface entry not found.");
            }
            else {
                ::read_from_model(r, manager);
            }
        }
    }
    set_response(response, r);
}
