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
 *
 * @file ip_watcher.cpp
 * @brief Implementation of IP watcher thread
 * */

#include "agent-framework/module/storage_components.hpp"
#include "agent-framework/eventing/utils.hpp"
#include "agent-framework/discovery/discoverers/ethernet_interface_discoverer.hpp"

#include "tree_stability/storage_stabilizer.hpp"
#include "agent/watcher/ip_watcher.hpp"

using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework::module::managers;
using namespace agent_framework::eventing;
using namespace agent::storage::watcher;

namespace {

static const constexpr std::chrono::seconds TIMEOUT_S{30};

void handle_interface_change(agent::storage::AgentContext::SPtr context, const NetworkInterface& interface) {

    // Send notification about changed NIC
    send_update(interface);

    OptionalField<std::string> new_address{};
    if (interface.get_ipv4_addresses().empty()) {
        log_warning("ip-watcher", "Interface " << interface.get_uuid() << " has no IP addresses set");
    }
    else {
        new_address = interface.get_ipv4_addresses().front().get_address();
    }

    auto iscsi_data = context->configuration->get_iscsi_data();
    if (iscsi_data.get_portal_interface() == interface.get_name()) {
        iscsi_data.set_portal_ip(new_address.value_or(std::string{}));
        context->configuration->set_iscsi_data(iscsi_data);
        log_info("ip-watcher", "Portal IP was changed to: " << new_address.value_or("<empty>"));
    }

    for (const auto& endpoint : get_manager<Endpoint>().get_entries()) {
        bool endpoint_changed = false;
        auto transport_details = endpoint.get_ip_transport_details();
        for (auto& transport_detail: transport_details) {
            if (transport_detail.get_interface() == interface.get_uuid()) {
                transport_detail.set_ipv4_address({new_address});
                log_info("ip-watcher", "Interface changed for endpoint " << endpoint.get_uuid());
                endpoint_changed = true;
            }
        }

        if (endpoint_changed) {
            auto endpoint_ref = get_manager<Endpoint>().get_entry_reference(endpoint.get_uuid());
            endpoint_ref->set_ip_transport_details(transport_details);
            send_update(endpoint_ref.get_raw_ref());
        }
    }
}

}


void IpWatcher::execute() {
    while (is_running()) {
        try {
            std::this_thread::sleep_for(::TIMEOUT_S);
            if (!get_manager<System>().get_entries().empty()) {
                auto system = get_manager<System>().get_entries().front();
                auto interfaces = agent_framework::discovery::EthernetInterfaceDiscoverer{
                    m_context->interface_reader->get_interfaces()
                }.discover(system.get_uuid());

                for (auto& interface : interfaces) {
                    StorageStabilizer().stabilize(interface);
                    auto status = agent_framework::module::get_manager<NetworkInterface>()
                        .add_or_update_entry(interface);
                    if (status == agent_framework::module::TableInterface::UpdateStatus::Updated) {
                        ::handle_interface_change(m_context, interface);
                        log_debug("ip-watcher", "Ethernet interface " << interface.get_uuid() << " was updated.");
                    }
                }
            }
        }
        catch (const std::exception& ex) {
            log_error("ip-watcher", "Error occurred during IP watching: " << ex.what());
        }
    }
}


IpWatcher::~IpWatcher() {}
