/*!
 * @brief OPAE Proxy interface discoverer builder.
 *
 * @copyright Copyright (c) 2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file fpgaof_opaeproxy_interface_builder.cpp
 */


#include "agent-framework/module/model/network_interface.hpp"
#include "agent-framework/module/utils/to_hex_string.hpp"
#include "discovery/builders/fpgaof_opaeproxy_interface_builder.hpp"
#include "net/network_interface.hpp"



using namespace agent::fpgaof::discovery;
using namespace agent_framework::model;

namespace {
OptionalField<std::string> get_mac_address(const std::string& ipv4_address) {
    try {

        auto net_ip_address = net::IpAddress::from_string(ipv4_address);
        auto net_interface = net::NetworkInterface::for_address(net_ip_address);

        std::stringstream ss;
        ss << net_interface.get_hw_address();

        return ss.str();
    }
    catch (const std::exception& ex) {
        log_error("interface-discoverer",
                  "Error when trying to check MAC address for IP address'" << ipv4_address << ": "
                                                                           << ex.what());
    }

    return OptionalField<std::string>();
}


attribute::Ipv4Address get_ip_address_data(const std::string& ipv4_address) {
    attribute::Ipv4Address ipv4_address_attribute{};

    try {

        auto net_ip_address = net::IpAddress::from_string(ipv4_address);
        auto net_interface = net::NetworkInterface::for_address(net_ip_address);

        // Set IP address
        ipv4_address_attribute.set_address(ipv4_address);

        // Set subnet mask (requires index of IP address)
        const auto& addresses = net_interface.get_address_list();
        for (std::size_t index = 0; index < addresses.size(); ++index) {

            if (net_interface.get_address(index) == net_ip_address) {

                ipv4_address_attribute.set_subnet_mask(net_interface.get_subnet_mask(index).to_string());
                break;
            }

        }
    }
    catch (const std::exception& ex) {
        log_error("interface-discoverer",
                  "Error when trying to info about IP address'" << ipv4_address << ": "
                                                                << ex.what());
    }

    return ipv4_address_attribute;
}


OptionalField<std::string> get_interface_name(const std::string& ipv4_address) {
    try {

        auto net_ip_address = net::IpAddress::from_string(ipv4_address);
        auto net_interface = net::NetworkInterface::for_address(net_ip_address);

        return net_interface.get_name();
    }
    catch (const std::exception& ex) {
        log_error("interface-discoverer",
                  "Error when trying to read name of interface with IP address'" << ipv4_address << ": "
                                                                                 << ex.what());
    }
    return OptionalField<std::string>();
}

}


NetworkInterface FpgaofOpaeproxyInterfaceBuilder::build(const Uuid& parent_uuid,
                                                        const std::string& ipv4,
                                                        const std::vector<std::string>& opaeproxy_transport_protocols) {
    NetworkInterface network_interface{parent_uuid};

    NetworkInterface::Ipv4Addresses ipv4_addresses{};
    attribute::Ipv4Address ipv4_address = get_ip_address_data(ipv4);
    ipv4_addresses.add_entry(ipv4_address);
    network_interface.set_ipv4_addresses(ipv4_addresses);

    NetworkInterface::TransportProtocols transport_protocols{};
    for (const auto& transport_protocol: opaeproxy_transport_protocols) {
        if (transport_protocol == "RDMA") {
            transport_protocols.add_entry(enums::TransportProtocol::RoCEv2);
        }
        else {
            transport_protocols.add_entry(enums::TransportProtocol::OEM);
        }
    }
    network_interface.set_supported_transport_protocols(transport_protocols);

    network_interface.set_mac_address(get_mac_address(ipv4));

    auto network_interface_name = get_interface_name(ipv4);
    if (network_interface_name.has_value()) {
        network_interface.set_name(network_interface_name);
    }

    attribute::Status status{};
    status.set_state(enums::State::Enabled);
    status.set_health(enums::Health::OK);
    network_interface.set_status(status);

    return network_interface;
}