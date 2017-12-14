/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
#include "psme/rest/utils/network_interface_info.hpp"
#include "psme/rest/utils/status_helpers.hpp"
#include "configuration/configuration.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;
using namespace agent_framework::model::enums;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#EthernetInterface.EthernetInterface";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#EthernetInterface.v1_1_0.EthernetInterface";
    r[Common::ID] = json::Value::Type::NIL;
    r[Common::NAME] = "Manager NIC";
    r[Common::DESCRIPTION] = "Manager NIC description";
    r[Common::STATUS][Common::STATE] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Value::Type::NIL;
    r[Common::MAC_ADDRESS] = json::Value::Type::NIL;
    r[NetworkInterface::PERMANENT_MAC_ADDRESS] = json::Value::Type::NIL;
    r[NetworkInterface::SPEED_MBPS] = json::Value::Type::NIL;
    r[NetworkInterface::AUTO_NEG] = json::Value::Type::NIL;
    r[NetworkInterface::FULL_DUPLEX] = json::Value::Type::NIL;
    r[NetworkInterface::MTU_SIZE] = json::Value::Type::NIL;
    r[NetworkInterface::HOST_NAME] = json::Value::Type::NIL;
    r[NetworkInterface::FQDN] = json::Value::Type::NIL;
    r[NetworkInterface::MAX_IPv6_STATIC_ADDRESSES] = json::Value::Type::NIL;
    r[NetworkInterface::INTERFACE_ENABLED] = json::Value::Type::NIL;
    r[NetworkInterface::LINK_STATUS] = json::Value::Type::NIL;

    r[NetworkInterface::IPv4_ADDRESSES] = json::Value::Type::ARRAY;
    r[NetworkInterface::IPv6_ADDRESS_POLICY_TABLE] = json::Value::Type::ARRAY;
    r[NetworkInterface::IPv6_ADDRESSES] = json::Value::Type::ARRAY;
    r[NetworkInterface::IPv6_DEFAULT_GATEWAY] = json::Value::Type::NIL;
    r[NetworkInterface::IPv6_STATIC_ADDRESSES] = json::Value::Type::ARRAY;
    r[NetworkInterface::NAME_SERVERS] = json::Value::Type::ARRAY;
    r[NetworkInterface::VLANS] = json::Value::Type::NIL;

    json::Value links;
    links[Fabric::ENDPOINTS] = json::Value::Type::ARRAY;
    links[Common::OEM][Common::RACKSCALE][Common::ODATA_TYPE] = "#Intel.Oem.EthernetInterface";
    links[Common::OEM][Common::RACKSCALE][NetworkInterface::NEIGHBOR_PORT] = json::Value::Type::NIL;
    r[Common::LINKS] = std::move(links);
    r[Common::OEM] = json::Value::Type::OBJECT;

    return r;
}


void read_interface_from_hw(json::Value& r) {
    const json::Value config = configuration::Configuration::get_instance().to_json();
    const auto& nic_name = config["server"]["network-interface-name"].as_string();
    psme::rest::utils::NetworkInterfaceInfo nic_info(nic_name);

    try {
        const auto& nic_address = nic_info.get_interface_address();

        if (!nic_address.get_ip_address().empty()) {
            const auto& ip_address = nic_address.get_ip_address();
            const auto& subnet_mask = nic_address.get_netmask();
            const auto& mac_address = nic_address.get_mac_address();

            json::Value ipv4_address;
            ipv4_address[IpAddress::ADDRESS] = ip_address;
            ipv4_address[IpAddress::SUBNET_MASK] = subnet_mask;
            r[NetworkInterface::IPv4_ADDRESSES].push_back(std::move(ipv4_address));

            r[Common::MAC_ADDRESS] = mac_address;
        }
        else {
            log_warning(GET_LOGGER("rest"),
                        "Empty IPv4 Address in the network interface read from the configuration file.");
        }

    }
    catch (const std::exception& ex) {
        log_error(GET_LOGGER("rest"), "Unable to read data for nic: " + nic_name + ". Exception " << ex.what());
    }
}


void read_from_model(json::Value& r, const agent_framework::model::Manager& manager) {
    r[Common::MAC_ADDRESS] = json::Value::Type::NIL;
    r[NetworkInterface::IPv4_ADDRESSES] = json::Value::Type::ARRAY;

    if (manager.get_ipv4_address().has_value()) {
        json::Value ipv4_address{};
        ipv4_address[IpAddress::ADDRESS] = manager.get_ipv4_address();
        ipv4_address[IpAddress::SUBNET_MASK] = json::Value::Type::NIL;
        r[NetworkInterface::IPv4_ADDRESSES].push_back(std::move(ipv4_address));
    }
}

void read_from_network_interface(const agent_framework::model::NetworkInterface& nic, json::Value& r) {

    r[NetworkInterface::PERMANENT_MAC_ADDRESS] = nic.get_factory_mac_address();
    r[Common::MAC_ADDRESS] = nic.get_mac_address();
    r[NetworkInterface::SPEED_MBPS] = nic.get_speed_mbps();
    r[NetworkInterface::AUTO_NEG] = nic.get_autosense();
    r[NetworkInterface::FULL_DUPLEX] = nic.get_full_duplex();
    r[NetworkInterface::MTU_SIZE] = nic.get_frame_size();
    r[NetworkInterface::MAX_IPv6_STATIC_ADDRESSES] = nic.get_max_ipv6_static_addresses();
    r[NetworkInterface::INTERFACE_ENABLED] = true;
    r[NetworkInterface::IPv6_DEFAULT_GATEWAY] = nic.get_ipv6_default_gateway();

    for (const auto& address : nic.get_ipv4_addresses()) {
        json::Value ipv4_address{};

        ipv4_address[IpAddress::ADDRESS] = address.get_address();
        ipv4_address[IpAddress::SUBNET_MASK] = address.get_subnet_mask();
        ipv4_address[IpAddress::ADDRESS_ORIGIN] = address.get_address_origin();
        ipv4_address[IpAddress::GATEWAY] = address.get_gateway();

        r[NetworkInterface::IPv4_ADDRESSES].push_back(std::move(ipv4_address));
    }

    for (const auto& address : nic.get_ipv6_addresses()) {
        json::Value ipv6_address{};

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
    auto manager = psme::rest::model::Find<agent_framework::model::Manager>(
        request.params[PathParam::MANAGER_ID]).get();
    auto nic_uuids = agent_framework::module::get_manager<agent_framework::model::NetworkInterface>()
        .get_keys(manager.get_uuid());

    auto r = ::make_prototype();
    r[Common::ODATA_ID] = PathBuilder(request).build();
    r[Common::ID] = request.params[PathParam::NIC_ID];

    if (!nic_uuids.empty()) {
        const auto nic = psme::rest::model::Find<agent_framework::model::NetworkInterface>(request.params[PathParam::NIC_ID])
            .via<agent_framework::model::Manager>(request.params[PathParam::MANAGER_ID])
            .get();

        ::read_from_network_interface(nic, r);
        r[NetworkInterface::VLANS][Common::ODATA_ID] =
            endpoint::PathBuilder(request)
                .append(NetworkInterface::VLANS)
                .build();
    }
    else if (id != 1) {
        THROW(agent_framework::exceptions::NotFound, "rest", "EthernetInterface entry not found.");
    }
    else {
        // If the manager that has this NIC controls a Drawer or an Enclosure
        // (Chassis of type Drawer or Enclosure) - read data from hardware.
        // If it doesn't - fill this NIC with stub data.
        if (psme::rest::endpoint::utils::is_manager_for_drawer_or_enclosure(manager.get_uuid()) ||
            manager.get_manager_type() == ManagerInfoType::EnclosureManager) {
            ::read_interface_from_hw(r);
        }
        else {
            ::read_from_model(r, manager);
        }
    }

    set_response(response, r);
}
