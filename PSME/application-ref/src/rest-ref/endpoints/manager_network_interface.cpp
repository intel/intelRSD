    /*!
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
 * */

#include "psme/rest-ref/endpoints/manager_network_interface.hpp"
#include "psme/rest-ref/constants/constants.hpp"
#include "psme/rest-ref/utils/network_interface_info.hpp"
#include "configuration/configuration.hpp"

using namespace psme::rest;
using namespace app::rest::constants;



namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#EthernetInterface.EthernetInterface";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#EthernetInterface.1.0.0.EthernetInterface";
    r[Common::ID] = json::Value::Type::NIL;
    r[Common::NAME] = "Manager NIC";
    r[Common::DESCRIPTION] = "Manager NIC description";
    r[Common::STATUS][Common::STATE] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Value::Type::NIL;
    r[Common::OEM] = json::Value::Type::OBJECT;
    r[NetworkInterface::MAC_ADDRESS] = json::Value::Type::NIL;
    r[NetworkInterface::PERMANENT_MAC_ADDRESS] = json::Value::Type::NIL;
    r[NetworkInterface::SPEED] = json::Value::Type::NIL;
    r[NetworkInterface::AUTO_NEG] = json::Value::Type::NIL;
    r[NetworkInterface::FULL_DUPLEX] = json::Value::Type::NIL;
    r[NetworkInterface::MTU_SIZE] = json::Value::Type::NIL;
    r[NetworkInterface::HOST_NAME] = json::Value::Type::NIL;
    r[NetworkInterface::FQDN] = json::Value::Type::NIL;
    r[NetworkInterface::MAX_IPv6_STATIC_ADDRESSES] = json::Value::Type::NIL;
    r[NetworkInterface::INTERFACE_ENABLED] = json::Value::Type::NIL;

    r[NetworkInterface::IPv4_ADDRESSES] = json::Value::Type::ARRAY;

    r[NetworkInterface::IPv6_ADDRESS_POLICY_TABLE] = json::Value::Type::ARRAY;

    r[NetworkInterface::IPv6_ADDRESSES] = json::Value::Type::ARRAY;
    r[NetworkInterface::IPv6_DEFAULT_GATEWAY] = json::Value::Type::NIL;
    r[NetworkInterface::IPv6_STATIC_ADDRESSES] = json::Value::Type::ARRAY;
    r[NetworkInterface::NAME_SERVERS] = json::Value::Type::ARRAY;
    r[NetworkInterface::VLANS] = json::Value::Type::NIL;

    json::Value links;

    links[Common::OEM][Common::RACKSCALE]
        [Common::ODATA_TYPE] =
            "#Intel.Oem.EthernetInterface";
    links[Common::OEM][Common::RACKSCALE]
        [NetworkInterface::NEIGHBOR_PORT][Common::ODATA_ID] =
            json::Value::Type::NIL;

    r[Common::LINKS] = std::move(links);

    return r;
}

void read_drawer_interface(json::Value& r){
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

            r[NetworkInterface::MAC_ADDRESS] = mac_address;
        }
        else {
            log_warning(GET_LOGGER("rest"),
                "Empty IPv4 Address in the network interface read from the configuration file");
        }

    } catch (const std::exception& ex){
        log_error(GET_LOGGER("rest"), "Unable to read data for nic: " + nic_name + " . Exception " << ex.what());
    }
}

void read_from_model(json::Value& r, const agent_framework::model::Manager& manager) {
    r[NetworkInterface::MAC_ADDRESS] = json::Value::Type::NIL;
    r[NetworkInterface::IPv4_ADDRESSES] = json::Value::Type::ARRAY;

    if (manager.get_ipv4_address().has_value())
    {
        json::Value ipv4_address;
        ipv4_address[IpAddress::ADDRESS] = manager.get_ipv4_address();
        ipv4_address[IpAddress::SUBNET_MASK] = json::Value::Type::NIL;
        r[NetworkInterface::IPv4_ADDRESSES].push_back(std::move(ipv4_address));
    }
}

}

endpoint::ManagerNetworkInterface::ManagerNetworkInterface(const std::string& path) : Endpoint(path) {}
endpoint::ManagerNetworkInterface::~ManagerNetworkInterface() {}

void endpoint::ManagerNetworkInterface::get(const server::Request& req, server::Response& res) {
    auto id = psme::rest::endpoint::utils::id_to_uint64(req.params[PathParam::NIC_ID]);

    // each manager has precisely one EthernetInterface with id = 1
    auto manager = psme::rest::endpoint::utils::get_manager(req.params[PathParam::MANAGER_ID]);
    auto switch_ids = agent_framework::module::NetworkManager::get_instance()->get_switch_manager().get_ids(manager.get_uuid());

    if (id != 1 || !switch_ids.empty()) {
        THROW(agent_framework::exceptions::NotFound, "rpc",
              "EthernetInterface entry not found (id= " + req.params[PathParam::NIC_ID] + ").");
    }

    auto r = ::make_prototype();
    r[Common::ODATA_ID] = PathBuilder(req).build();
    r[Common::ID] = req.params[PathParam::NIC_ID];

    // if the manager that has this NIC controls a Drawer (Chassis of type
    // Drawer) - read data from hardware.
    // If it doesn't - fill this NIC with stub data.

    if (psme::rest::endpoint::utils::is_drawer_manager(manager.get_uuid())) {
        read_drawer_interface(r);
    } else {
        read_from_model(r, manager);
    }

    set_response(res, r);
}
