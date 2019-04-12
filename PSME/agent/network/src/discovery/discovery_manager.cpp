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
 * @file discovery_manager.cpp
 * @brief Initial discovery implementation.
 * */

#include "discovery/discovery_manager.hpp"
#include "loader/network_config.hpp"
#include "tree_stability/network_tree_stabilizer.hpp"
#include "network_config.hpp"
#include "agent-framework/module/network_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/model/attributes/event_data.hpp"
#include "agent-framework/eventing/events_queue.hpp"

#include <eos/sdk.h>

#include "hal/switch_info.hpp"
#include "hal/switch_info_impl.hpp"
#include "hal/switch_port_info_impl.hpp"
#include "hal/switch_agent.hpp"
#include "hal/eos_eapi/port_running_configuration_parser.hpp"
#include "hal/eos_eapi/switch_running_configuration_parser.hpp"
#include "utils/utils.hpp"

#include <safe-string/safe_lib.hpp>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <regex>

using namespace agent::network;
using namespace agent::network::loader;
using namespace agent::network::discovery;
using namespace agent::network::utils;
using namespace agent::network::hal;

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

    void update_manager_info(const string& uuid) {
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
                                CollectionType::EthernetSwitches});
        manager->add_collection({CollectionName::Chassis,
                                CollectionType::Chassis});
    }

    string get_switch_uuid(const string& uuid) {
        auto uuids = NetworkComponents::get_instance()->
                                    get_switch_manager().get_keys(uuid);
        if (uuids.empty()) {
            throw std::runtime_error("No ethernet switches found");
        }
        return uuids.front();
    }

    EthernetSwitch get_switch_qos_running_config() {
        EthernetSwitch switch_module{};
        const auto running_config = hal::SwitchAgent::get_instance()->get_running_config();
        eapi::SwitchRunningConfigurationParser running_config_parser{running_config.dump()};

        auto qap_array = running_config_parser.get_qos_application_protocol();
        auto qpg_array = running_config_parser.get_qos_priority_group_mapping();
        auto qba_array = running_config_parser.get_qos_bandwidth_allocation();

        switch_module.set_qos_application_protocol(qap_array);
        switch_module.set_qos_priority_group_mapping(qpg_array);
        switch_module.set_qos_bandwidth_allocation(qba_array);
        switch_module.set_pfc_enabled(hal::SwitchAgent::get_instance()->is_switch_pfc_enabled());
        switch_module.set_lldp_enabled(running_config_parser.is_switch_lldp_enabled());

        return switch_module;
    }

    void update_switch_info(const string& switch_uuid) {
        /* get switch reference */
        auto switch_module = NetworkComponents::get_instance()->
                get_switch_manager().get_entry_reference(switch_uuid);
        /* TODO how do we know the location ? */
        switch_module->set_location(SWITCH_DEFAULT_LOCATION);
        switch_module->add_collection({CollectionName::EthernetSwitchPorts,
                                      CollectionType::EthernetSwitchPorts});

        EthernetSwitch switch_running_config = get_switch_qos_running_config();
        switch_module->set_qos_application_protocol(switch_running_config.get_qos_application_protocol());
        switch_module->set_qos_priority_group_mapping(switch_running_config.get_qos_priority_group_mapping());
        switch_module->set_qos_bandwidth_allocation(switch_running_config.get_qos_bandwidth_allocation());
        switch_module->set_pfc_enabled(switch_running_config.get_pfc_enabled());
        switch_module->set_lldp_enabled(switch_running_config.get_lldp_enabled());
    }

    void apply_switch_qos_config(const string& switch_uuid) {
        EthernetSwitch running_switch_config = get_switch_qos_running_config();
        auto switch_module =  NetworkComponents::get_instance()->get_switch_manager().get_entry(switch_uuid);

        SwitchInfoImpl switch_info(switch_module.get_switch_identifier());

        const auto& qap_from_config_file = switch_module.get_qos_application_protocol();
        if (!qap_from_config_file.empty()) {
            switch_info.update_switch_qos_application_protocol(running_switch_config.get_qos_application_protocol(),
                                                               qap_from_config_file);
        }

        const auto& qpg_from_config_file = switch_module.get_qos_priority_group_mapping();
        if (!qpg_from_config_file.empty()) {
            switch_info.update_switch_qos_priority_group_mapping(running_switch_config.get_qos_priority_group_mapping(),
                                                                 qpg_from_config_file);
        }

        const auto& qba_from_config_file = switch_module.get_qos_bandwidth_allocation();
        if (!qba_from_config_file.empty()) {
            switch_info.update_switch_qos_bandwidth_allocation(running_switch_config.get_qos_bandwidth_allocation(),
                                                               qba_from_config_file);
        }

        if (switch_module.get_pfc_enabled().has_value()) {
            switch_info.update_switch_pfc_enabled(running_switch_config.get_pfc_enabled(),
                                                  switch_module.get_pfc_enabled());
        }

        if (switch_module.get_lldp_enabled().has_value()) {
            switch_info.update_switch_lldp_enabled(running_switch_config.get_lldp_enabled(),
                                                   switch_module.get_lldp_enabled());
        }
    }

    OptionalField<EthernetSwitchPort> find_and_delete_default_port_config() {
        auto& port_manager = get_manager<EthernetSwitchPort>();
        string default_port_uuid{};

        if (get_port_uuid_by_identifier("Default", default_port_uuid)) {
            EthernetSwitchPort default_port(port_manager.get_entry(default_port_uuid));
            // Remove 'Default' Port entry from Switch model - it is used only for actual ports configuration purposes
            port_manager.remove_entry(default_port_uuid);

            return default_port;
        }
        return OptionalField<EthernetSwitchPort>();
    }

    void apply_port_qos_config(EthernetSwitchPort& port_model, const json::Json& port_running_config) {
        log_debug("network-agent", "Active port " + port_model.get_port_identifier().value() + " - configure/read switch parameters");
        eapi::PortRunningConfigurationParser current_port_config(port_running_config.dump());

        SwitchPortInfoImpl switch_port_info(port_model.get_port_identifier().value());
        if (port_model.get_pfc_enabled().has_value()) {
            switch_port_info.update_switch_port_pfc_enabled(current_port_config.is_pfc_enabled(),
                                                            port_model.get_pfc_enabled());
        }
        else {
            port_model.set_pfc_enabled(current_port_config.is_pfc_enabled());
        }

        if (!port_model.get_pfc_enabled_priorities().empty()) {
            switch_port_info.update_switch_port_pfc_priorities(current_port_config.get_enabled_priorities(),
                                                               port_model.get_pfc_enabled_priorities());
        }
        else {
            port_model.set_pfc_enabled_priorities(current_port_config.get_enabled_priorities());
        }

        if (port_model.get_dcbx_state().has_value()) {
            switch_port_info.update_switch_port_dcbx_state(current_port_config.get_dcbx_state(),
                                                           port_model.get_dcbx_state());
        }
        else {
            port_model.set_dcbx_state(current_port_config.get_dcbx_state());
        }

        if (port_model.get_lldp_enabled().has_value()) {
            switch_port_info.update_switch_port_lldp_enabled(current_port_config.is_lldp_enabled(),
                                                             port_model.get_lldp_enabled());
        }
        else {
            port_model.set_lldp_enabled(current_port_config.is_lldp_enabled());
        }
    }

    void set_common_port_properties(EthernetSwitchPort& port_model) {
        // Update port static attributes
        port_model.add_collection({CollectionName::Vlans, CollectionType::EthernetSwitchPortVlans});
        port_model.set_port_class(PortClass::Physical);
        port_model.set_port_mode(PortMode::Unknown);
        port_model.set_vlan_enable(true);
        port_model.set_status({State::Enabled, Health::OK});

        // Update port type attribute if not set already
        const auto& port_type = port_model.get_port_type();
        if (!port_type.has_value() || port_type == PortType::Unknown) {
            port_model.set_port_type(PortType::Downstream);
        }

        // Update port link technology attribute if not set already
        const auto& link_technology = port_model.get_link_technology();
        if (!link_technology.has_value() || link_technology == LinkTechnology::Unknown) {
            port_model.set_link_technology(LinkTechnology::Ethernet);
        }
    }

    void update_port(const string& port_uuid, const json::Json& interfaces_config) {
        auto& port_manager = get_manager<EthernetSwitchPort>();
        auto port_model = port_manager.get_entry_reference(port_uuid);
        auto& port_identifier = port_model->get_port_identifier().value();

        port_model->set_physical_or_up(is_port_physical_or_up(port_identifier, interfaces_config));

        // Perform switch configuration for physical and working port only
        if (port_model->is_physical_or_up()) {
            apply_port_qos_config(port_model.get_raw_ref(),
                                  interfaces_config[std::string("interface " + port_identifier)]);
        }

        set_common_port_properties(port_model.get_raw_ref());
    }

    void add_port(const string& switch_uuid,
                  const string& port_identifier,
                  const json::Json& interfaces_config,
                  const OptionalField<EthernetSwitchPort>& default_port_config) {
        //  Add port into network manager
        auto& port_manager = get_manager<EthernetSwitchPort>();
        EthernetSwitchPort port_model{switch_uuid};
        port_model.set_port_identifier(port_identifier);
        port_model.set_physical_or_up(is_port_physical_or_up(port_identifier, interfaces_config));

        // Perform switch configuration for physical and working port only
        if (port_model.is_physical_or_up()) {
            // Update port model with default configuration if available
            if (default_port_config.has_value()) {
                port_model.set_pfc_enabled(default_port_config.value().get_pfc_enabled());
                port_model.set_pfc_enabled_priorities(default_port_config.value().get_pfc_enabled_priorities());
                port_model.set_dcbx_state(default_port_config.value().get_dcbx_state());
                port_model.set_lldp_enabled(default_port_config.value().get_lldp_enabled());
            }

            apply_port_qos_config(port_model, interfaces_config[std::string("interface " + port_identifier)]);
        }

        set_common_port_properties(port_model);

        port_manager.add_entry(port_model);
    }

    const json::Json& get_interfaces_config(const json::Json& switch_config) {
            return switch_config[0][std::string("cmds")];
    }

    void discover_switch_ports(const string& switch_uuid) {
        /* discover physical & logical ports */
        auto& switch_manager = get_manager<EthernetSwitch>();
        try {
            auto switch_module = switch_manager.get_entry(switch_uuid);
            const string& switch_identifier = switch_module.get_switch_identifier();
            auto port_list = hal::SwitchAgent::get_instance()->get_port_list();

            const auto& running_config = hal::SwitchAgent::get_instance()->get_running_config();
            const auto& interfaces_config = get_interfaces_config(running_config);
            const auto& default_port_config = find_and_delete_default_port_config();

            for (const auto& port_identifier : port_list) {
                string port_uuid{};

                if (get_port_uuid_by_identifier(port_identifier, port_uuid)) {
                    log_debug("network-agent", "Updating port " + port_identifier);
                    update_port(port_uuid, interfaces_config);
                }
                else {
                    log_debug("network-agent", "Adding port " + port_identifier);
                    add_port(switch_uuid, port_identifier, interfaces_config, default_port_config);
                }
            }
        }
        catch (const std::runtime_error& e) {
            log_error("network-agent", e.what());
            throw;
        }
    }

    void discover_switch_vlan_ports(const string&) {
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

    void discover_port_neighbor_mac_addr(const string&) {
        hal::SwitchAgent::get_instance()->discover_port_neighbor_mac_addr();
    }

    void show_network_modules() {
        auto network_components = NetworkComponents::get_instance();
        auto common_components = CommonComponents::get_instance();

        auto& module_manager = common_components->get_module_manager();
        auto& switch_manager = network_components->get_switch_manager();
        auto& port_manager = network_components->get_port_manager();
        auto& port_vlan_manager = network_components->get_port_vlan_manager();
        for (const auto& uuid : module_manager.get_keys()) {
            log_debug("network-agent", "Manager UUID: " + uuid);
            for (const auto& switch_uuid : switch_manager.get_keys(uuid)) {
                log_debug("network-agent", "Switch UUID: " + switch_uuid);
                for (const auto& port_uuid : port_manager.get_keys(switch_uuid)) {
                    log_debug("network-agent", "Port UUID: " + port_uuid);
                    for (const auto& port_vlan_uuid :
                                        port_vlan_manager.get_keys(port_uuid)) {
                        log_debug("network-agent", "Port-vlan UUID: "
                                                                + port_vlan_uuid);
                    }
                }
            }
        }
    }

void send_event(const std::string& module, const std::string& parent, Component type,
        agent_framework::model::enums::Notification notification) {
    agent_framework::model::attribute::EventData edat;
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
    ::apply_switch_qos_config(switch_uuid);
    ::update_switch_info(switch_uuid);
    ::discover_switch_ports(switch_uuid);
    ::discover_switch_vlan_ports(switch_uuid);
    ::discover_port_neighbor_mac_addr(switch_uuid);
    ::show_network_modules();

    NetworkTreeStabilizer().stabilize(uuid);
    send_event(uuid, {}, Component::Manager, agent_framework::model::enums::Notification::Add);
}
