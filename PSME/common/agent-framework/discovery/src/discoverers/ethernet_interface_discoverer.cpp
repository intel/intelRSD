/*!
 * @brief Ethernet interface discoverer implementation.
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
 * @file ethernet_interface_discoverer.cpp
 */

#include "agent-framework/discovery/discoverers/ethernet_interface_discoverer.hpp"
#include "agent-framework/discovery/builders/ethernet_interface_builder.hpp"

#include "net/network_interface.hpp"

using namespace agent_framework;
using namespace agent_framework::discovery;

EthernetInterfaceDiscoverer::~EthernetInterfaceDiscoverer() { }

std::vector<model::NetworkInterface> EthernetInterfaceDiscoverer::discover(const Uuid& parent_uuid) {
    std::vector<model::NetworkInterface> interfaces{};

    for (const auto& interface_name : m_interface_names) {
        try {
            auto ifa = net::NetworkInterface::for_name(interface_name);
            log_debug("interface-discoverer", "Found interface " << ifa.get_name());

            auto interface = EthernetInterfaceBuilder::build_default(parent_uuid);
            EthernetInterfaceBuilder::update_with_net(interface, ifa);
            interfaces.emplace_back(std::move(interface));
        }
        catch (const std::exception& ex) {
            log_error("interface-discoverer", "Error when reading interface '" << interface_name << "': " << ex.what());
        }
    }

    return interfaces;
}

