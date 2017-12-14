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
 *
 * @file discovery_manager.cpp
 * @brief Initial discovery implementation.
 * */

#include "discovery/discovery_manager.hpp"
#include "loader/network_config.hpp"
#include "tree_stability/network_tree_stabilizer.hpp"
#include "network_config.hpp"
#include "agent-framework/module/network_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/eventing/event_data.hpp"
#include "agent-framework/eventing/events_queue.hpp"

#include <eos/sdk.h>

#include "hal/switch_info.hpp"
#include "hal/switch_agent.hpp"
#include "utils/utils.hpp"

#include <safe-string/safe_lib.hpp>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>

using namespace agent::network;
using namespace agent::network::loader;
using namespace agent::network::discovery;
using namespace agent::network::utils;

using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;
using namespace agent_framework::model::attribute;

using std::string;

namespace {
    constexpr uint32_t SERIAL_DEFAULT_BITRATE = 115200;
    constexpr uint32_t SERIAL_DEFAULT_DATABITS = 8;
    constexpr uint32_t SERIAL_DEFAULT_STOPBIT = 1;
    constexpr uint32_t SWITCH_DEFAULT_LOCATION = 1;

    static void update_manager_info(const string& uuid) {
        /* get module reference */
        auto manager = CommonComponents::get_instance()->
                get_module_manager().get_entry_reference(uuid);
        /* update serial information */
        SerialConsole serial{manager->get_serial_console()};
        serial.set_bitrate(SERIAL_DEFAULT_BITRATE);
        serial.set_data_bits(SERIAL_DEFAULT_DATABITS);
        serial.set_flow_control(SerialConsoleFlowControl::None);
        serial.set_parity(SerialConsoleParity::None);
        serial.set_pin_out(SerialConsolePinOut::Cisco);
        serial.set_stop_bits(SERIAL_DEFAULT_STOPBIT);
        serial.set_signal_type(SerialConsoleSignalType::Rs232);
        /* add collections */
        manager->set_serial_console(std::move(serial));
        manager->add_collection({CollectionName::Switches,
                                CollectionType::EthernetSwitches, ""});
        manager->add_collection({CollectionName::Chassis,
                                CollectionType::Chassis, ""});
    }

    static string get_switch_uuid(const string& uuid) {
        auto uuids = NetworkComponents::get_instance()->
                                    get_switch_manager().get_keys(uuid);
        if (uuids.empty()) {
            throw std::runtime_error("No ethernet switches found");
        }
        return uuids.front();
    }

    static void update_switch_info(const string& switch_uuid) {
        /* get switch reference */
        auto switch_module = NetworkComponents::get_instance()->
                get_switch_manager().get_entry_reference(switch_uuid);
        /* TODO how do we know the location ? */
        switch_module->set_location(SWITCH_DEFAULT_LOCATION);
        switch_module->add_collection({CollectionName::EthernetSwitchPorts,
                                      CollectionType::EthernetSwitchPorts, ""});
    }

    static void add_physical_port(const string& switch_uuid,
            const string& port_identifier) {
        /* add port into network manager */
        auto& port_manager = get_manager<EthernetSwitchPort>();
        EthernetSwitchPort port_model{switch_uuid};
        port_model.add_collection({CollectionName::Vlans,
                                  CollectionType::EthernetSwitchPortVlans, ""});
        port_model.set_link_technology(LinkTechnology::Ethernet);
        port_model.set_port_class(PortClass::Physical);
        port_model.set_port_type(PortType::Downstream);
        port_model.set_port_mode(PortMode::Unknown);
        port_model.set_port_identifier(port_identifier);
        port_model.set_vlan_enable(true);
        port_model.set_status({State::Enabled, Health::OK});
        port_manager.add_entry(port_model);
    }

    static void discover_switch_ports(const string& switch_uuid) {
        /* discover physical & logical ports */
        auto& port_manager = get_manager<EthernetSwitchPort>();
        auto& switch_manager = get_manager<EthernetSwitch>();;
        try {
            /* physical ports */
            auto switch_module = switch_manager.get_entry(switch_uuid);
            const string& switch_identifier =
                switch_module.get_switch_identifier();
            auto port_list = hal::SwitchAgent::get_instance()->get_port_list();
            for (const auto& port_identifier : port_list) {
                string port_uuid{};
                if (get_port_uuid_by_identifier(port_identifier, port_uuid)) {
                    log_debug(GET_LOGGER("network-agent"), "Updating port " +
                                                                port_identifier);
                    auto port = port_manager.get_entry_reference(port_uuid);
                    /* add port collections */
                    port->add_collection({CollectionName::Vlans,
                                         CollectionType::EthernetSwitchPortVlans, ""});
                    /* update port link technology attribute */
                    if (LinkTechnology::Unknown == port->get_link_technology()) {
                        port->set_link_technology(LinkTechnology::Ethernet);
                    }
                    /* update port static attributes */
                    port->set_port_class(PortClass::Physical);
                    port->set_port_mode(PortMode::Unknown);
                    port->set_vlan_enable(true);
                    port->set_status({State::Enabled, Health::OK});
                }
                else {
                    log_debug(GET_LOGGER("network-agent"), "Adding physical port " +
                                                                port_identifier);
                    add_physical_port(switch_uuid, port_identifier);
                }
            }
        }
        catch (const std::runtime_error& e) {
            log_error(GET_LOGGER("network-agent"), e.what());
            throw;
        }
    }

    static void discover_switch_vlan_ports(const string&) {
        /* add port-vlan pair to the network manager */
        auto& port_manager = get_manager<EthernetSwitchPort>();
        for (const auto& port_uuid : port_manager.get_keys()) {
            auto port = port_manager.get_entry(port_uuid);
            if (PortClass::Logical == port.get_port_class()) {
                if (is_logical_port_empty(port.get_port_identifier())) {
                    /* it's not possible to get vlan list on empty lag */
                    continue;
                }
            }
            /* get vlan list on the port */
            init_switch_vlan_port(port.get_port_identifier());
        }
    }

    static void discover_port_neighbor_mac_addr(const string&) {
        hal::SwitchAgent::get_instance()->discover_port_neighbor_mac_addr();
    }

    static void show_network_modules() {
        auto network_components = NetworkComponents::get_instance();
        auto common_components = CommonComponents::get_instance();

        auto& module_manager = common_components->get_module_manager();
        auto& switch_manager = network_components->get_switch_manager();
        auto& port_manager = network_components->get_port_manager();
        auto& port_vlan_manager = network_components->get_port_vlan_manager();
        for (const auto& uuid : module_manager.get_keys()) {
            log_debug(GET_LOGGER("network-agent"), "Manager UUID: " + uuid);
            for (const auto& switch_uuid : switch_manager.get_keys(uuid)) {
                log_debug(GET_LOGGER("network-agent"), "Switch UUID: " + switch_uuid);
                for (const auto& port_uuid : port_manager.get_keys(switch_uuid)) {
                    log_debug(GET_LOGGER("network-agent"), "Port UUID: " + port_uuid);
                    for (const auto& port_vlan_uuid :
                                        port_vlan_manager.get_keys(port_uuid)) {
                        log_debug(GET_LOGGER("network-agent"), "Port-vlan UUID: "
                                                                + port_vlan_uuid);
                    }
                }
            }
        }
    }

void send_event(const std::string& module, const std::string& parent, Component type,
        agent_framework::eventing::Notification notification) {
    agent_framework::eventing::EventData edat;
    edat.set_parent(parent);
    edat.set_component(module);
    edat.set_type(type);
    edat.set_notification(notification);
    agent_framework::eventing::EventsQueue::get_instance()->push_back(edat);
    log_info("network-agent", "Sending an event for component " << edat.get_component() << "," << "("
            << edat.get_type() << ")" << ", notification type: " << edat.get_notification());
}

} // namespace

DiscoveryManager::DiscoveryManager() {}
DiscoveryManager::~DiscoveryManager() {}

void DiscoveryManager::discovery(const std::string& uuid) {
    /* start discovery */
    ::update_manager_info(uuid);
    auto switch_uuid = get_switch_uuid(uuid);
    ::update_switch_info(switch_uuid);
    ::discover_switch_ports(switch_uuid);
    ::discover_switch_vlan_ports(switch_uuid);
    ::discover_port_neighbor_mac_addr(switch_uuid);
    ::show_network_modules();

    NetworkTreeStabilizer().stabilize(uuid);
    send_event(uuid, {}, Component::Manager, agent_framework::eventing::Notification::Add);
}
