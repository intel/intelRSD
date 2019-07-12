/*!
 * @brief Opaepp network interface discoverer interface.
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

#include "discovery/discoverers/fpgaof_opaeproxy_interface_discoverer.hpp"
#include "discovery/builders/fpgaof_opaeproxy_interface_builder.hpp"



using namespace agent::fpgaof::discovery;
using namespace agent_framework::model;

namespace {
bool is_ip_in_network_interfaces(const std::string& ipv4, const std::vector<NetworkInterface>& network_interfaces) {
    for (const auto& network_interface: network_interfaces) {

        for (const auto& interface_ip: network_interface.get_ipv4_addresses()) {

            if (interface_ip.get_address().has_value() && interface_ip.get_address() == ipv4) {
                return true;
            }

        }
    }

    return false;
}
}


std::vector<NetworkInterface> OpaeproxyNetworkInterfaceDiscoverer::discover(const Uuid& system_uuid) {
    std::vector<NetworkInterface> network_interfaces{};

    for (const auto& opae_proxy_transport: m_context->configuration->get_opae_proxy_transports()) {
        auto ip = opae_proxy_transport.get_ipv4_address();

        // Duplicate prevention
        if (is_ip_in_network_interfaces(ip, network_interfaces)) {
            continue;
        }

        std::vector<std::string> protocol_names;
        // Duplicate transports (same IP) with different transport protocols should be merged with all their transport protocols.
        for (const auto& opae_proxy_transport2: m_context->configuration->get_opae_proxy_transports()) {
            if (opae_proxy_transport2.get_ipv4_address() == opae_proxy_transport.get_ipv4_address()) {
                protocol_names.push_back(opae_proxy_transport2.get_protocol_name());
            }
        }
        NetworkInterface network_interface = FpgaofOpaeproxyInterfaceBuilder::build(system_uuid, ip, protocol_names);

        network_interfaces.push_back(network_interface);
    }

    return network_interfaces;
}