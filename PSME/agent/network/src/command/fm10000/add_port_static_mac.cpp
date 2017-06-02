/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016-2017 Intel Corporation
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
 * */

#include "agent-framework/module/network_components.hpp"
#include "agent-framework/command-ref/registry.hpp"
#include "agent-framework/command-ref/network_commands.hpp"
#include "agent-framework/eventing/event_data.hpp"
#include "agent-framework/eventing/events_queue.hpp"
#include "api/static_mac.hpp"
#include "netlink/nl_exception_invalid_input.hpp"
#include "tree_stability/network_tree_stabilizer.hpp"

#include <vector>
#include <cctype>
#include <net/if.h>



using namespace agent_framework::command_ref;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework;
using namespace agent::network;
using namespace std;

namespace {
inline void create_static_mac(const string& port, const string& address,
                              uint16_t vlan_id) {
    try {
        api::StaticMac::add(port, address, vlan_id);
    }
    catch (const netlink_base::NlExceptionInvalidInput& e) {
        log_error(GET_LOGGER("network-agent"), port << ": " << e.what());
        THROW(agent_framework::exceptions::InvalidValue, "network-agent",
              "Failed to create Static MAC on port: invalid parameters given.");
    }
    catch (const runtime_error& e) {
        log_error(GET_LOGGER("network-agent"), port << ": " << e.what());
        THROW(agent_framework::exceptions::Fm10000Error, "network-agent",
              string("Failed to create Static MAC on port: ") + e.what());
    }
}


bool mac_vlan_exists_on_port(const string& port_uuid,
                             const string& mac_address,
                             uint16_t vlan_id,
                             string* static_mac_uuid) {
    auto network_manager = NetworkComponents::get_instance();
    auto& static_mac_manager = network_manager->get_static_mac_manager();

    for (const auto& uuid : static_mac_manager.get_keys(port_uuid)) {
        auto static_mac_module = static_mac_manager.get_entry(uuid);
        if (mac_address == static_mac_module.get_address()
            && vlan_id == static_mac_module.get_vlan_id()) {
            if (static_mac_uuid) {
                *static_mac_uuid = uuid;
            }
            return true;
        }
    }
    return false;
}


void send_event(const string& static_mac_uuid, const string& port_uuid) {
    agent_framework::eventing::EventData edat;
    edat.set_component(static_mac_uuid);
    edat.set_type(::agent_framework::model::enums::Component::StaticMac);
    edat.set_notification(::agent_framework::eventing::Notification::Remove);
    edat.set_parent(port_uuid);
    agent_framework::eventing::EventsQueue::get_instance()->push_back(edat);
}


void remove_mac_vlan_from_other_port_if_exists(const string& port_uuid,
                                               const string& mac_address,
                                               uint16_t vlan_id) {
    auto network_manager = NetworkComponents::get_instance();
    const auto& port_manager = network_manager->get_port_manager();
    for (const auto& uuid : port_manager.get_keys()) {
        auto port = port_manager.get_entry(uuid);
        string static_mac_uuid{};
        if ((port_uuid != uuid)
            && mac_vlan_exists_on_port(uuid, mac_address, vlan_id,
                                       &static_mac_uuid)) {
            auto& static_mac_manager =
                network_manager->get_static_mac_manager();
            static_mac_manager.remove_entry(static_mac_uuid);
            send_event(static_mac_uuid, uuid);
            break;
        }
    }
}


void add_port_static_mac(const AddPortStaticMac::Request& request,
                         AddPortStaticMac::Response& response) {
    auto network_manager = NetworkComponents::get_instance();
    const auto& port_manager = network_manager->get_port_manager();
    auto port = port_manager.get_entry(request.get_port());
    auto& static_mac_manager = network_manager->get_static_mac_manager();

    if (!request.get_vlan_id().has_value()) {
        THROW(agent_framework::exceptions::InvalidValue, "fm10000",
              "VLAN ID is mandatory on FM10000 devices.");
    }

    uint16_t vlan_id = uint16_t(request.get_vlan_id());
    string mac_address = request.get_mac_address();

    for (auto& c: mac_address) { c = char(toupper(c)); }

    /* check if the MAC/VLAN pair already exists on given port */
    if (mac_vlan_exists_on_port(port.get_uuid(), mac_address, vlan_id, nullptr)) {
        THROW(agent_framework::exceptions::InvalidValue, "fm10000",
              "MAC address/VLAN already exists ["
              + std::string(" port = ") + port.get_port_identifier()
              + std::string(" MAC address = ") + mac_address
              + std::string(" VLAN = ") + std::to_string(int(vlan_id))
              + std::string(" ]"));
    }

    ::create_static_mac(port.get_port_identifier(), mac_address, vlan_id);

    // FM10000 switch removeѕ automatically MAC/VLAN pair
    // if it already exists on another port,
    // check if we have it in the model and if so remove it
    ::remove_mac_vlan_from_other_port_if_exists(port.get_uuid(),
                                                mac_address,
                                                vlan_id);

    model::StaticMac static_mac(request.get_port());
    const std::string static_mac_uuid = static_mac.get_uuid();

    static_mac.set_status({enums::State::Enabled, enums::Health::OK});
    static_mac.set_address(mac_address);
    static_mac.set_vlan_id(vlan_id);

    static_mac_manager.add_entry(std::move(static_mac));

    const std::string static_mac_persistent_uuid = NetworkTreeStabilizer().stabilize_static_mac(static_mac_uuid);
    response.set_static_mac(static_mac_persistent_uuid);

}
}

REGISTER_COMMAND(AddPortStaticMac, ::add_port_static_mac
);
