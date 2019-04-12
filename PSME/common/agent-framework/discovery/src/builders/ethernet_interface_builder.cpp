/*!
 * @brief Ethernet interface builder class implementation.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file ethernet_interface_builder.cpp
 */

#include "agent-framework/discovery/builders/ethernet_interface_builder.hpp"
#include "net/network_interface.hpp"


using namespace agent_framework;
using namespace agent_framework::discovery;


model::NetworkInterface EthernetInterfaceBuilder::build_default(const Uuid& parent_uuid) {
    model::NetworkInterface interface{parent_uuid};
    interface.set_status({model::enums::State::Enabled, model::enums::Health::OK});
    return interface;
}


void EthernetInterfaceBuilder::update_with_net(model::NetworkInterface& interface,
                                               const net::NetworkInterface& net_interface) {

    interface.set_name(net_interface.get_name());

    model::attribute::Status status{};
    status.set_health(model::enums::Health::OK);
    status.set_state(net_interface.is_up() ? model::enums::State::Enabled : model::enums::State::Disabled);
    interface.set_status(status);

    const auto& addresses = net_interface.get_address_list();
    for(std::size_t index = 0; index < addresses.size(); ++index) {
        const auto& ip_address = net_interface.get_address(index);

        if (ip_address.get_address_family() == net::AddressFamily::IPv4) {
            const auto& subnet_mask = net_interface.get_subnet_mask(index);
            model::attribute::Ipv4Address ipv4_address{};
            ipv4_address.set_address(ip_address.to_string());
            ipv4_address.set_subnet_mask(subnet_mask.to_string());
            interface.add_ipv4_address(ipv4_address);
        }
        else if (ip_address.get_address_family() == net::AddressFamily::IPv6) {
            model::attribute::Ipv6Address ipv6_address{};
            ipv6_address.set_address(ip_address.to_string());
            interface.add_ipv6_address(ipv6_address);
        }
        else {
            log_warning("model-builder", "Unsupported address family: "
                << std::to_string(ip_address.get_native_address_family())
                << " for interface " << net_interface.get_name());
        }
    }

    std::stringstream ss{};
    ss << net_interface.get_hw_address();
    interface.set_mac_address(ss.str());

    log_debug("model-builder", "MAC address for interface "
        << interface.get_name() << " is " << interface.get_mac_address());

    interface.set_frame_size(net_interface.get_mtu());
}
