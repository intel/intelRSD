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

#include "generate_data.hpp"
#include "agent-framework/module-ref/network_manager.hpp"

using namespace agent_framework::module;
using namespace agent_framework::model;

namespace {

StaticMac make_nulled_static_mac(const std::string& parent) {
    StaticMac static_mac{parent};
    return static_mac;
}

StaticMac make_full_static_mac(const std::string& parent) {
    StaticMac static_mac{parent};
    static_mac.set_address("AA:BB:CC:DD:EE:FF");
    static_mac.set_vlan_id(17);
    static_mac.set_status(attribute::Status(
            enums::State::Enabled,
            enums::Health::OK));
    return static_mac;
}

AclRule make_nulled_acl_rule(const std::string& parent) {
    AclRule rule{parent};
    return rule;
}

AclRule make_full_acl_rule(const std::string& parent) {
    const auto ports = NetworkManager::get_instance()->
        get_port_manager().get_keys();
    AclRule rule{parent};
    rule.set_status(attribute::Status(
        enums::State::Enabled,
        enums::Health::OK));
    rule.set_rule_id(1);
    rule.set_action(enums::AclAction::Mirror);
    rule.set_forward_mirror_port(ports[0]);
    rule.set_mirrored_ports(std::vector<std::string>(ports.begin() + 1,
        ports.begin() + 5));
    rule.set_mirror_type(enums::AclMirrorType::Bidirectional);
    rule.set_vlan_id(attribute::AclVlanId(
        1,
        13));
    rule.set_source_ip(attribute::AclAddress(
        "10.0.0.2",
        "255.0.0.0"));
    rule.set_destination_ip(attribute::AclAddress(
        "10.100.0.97",
        "255.255.0.0"));
    rule.set_source_mac(attribute::AclAddress(
        "FF:EE:DD:CC:BB:AA",
        "00:00:00:00:00:00"));
    rule.set_destination_mac(attribute::AclAddress(
        "1A:B2:3C:77:00:00",
        "00:00:00:00:FF:FF"));
    rule.set_source_port(attribute::AclPort(
        80,
        45));
    rule.set_destination_port(attribute::AclPort(
        443,
        17));
    rule.set_protocol(6);
    return rule;
}

Acl make_acl(const std::string& parent, const
    attribute::Array<std::string>& ports) {
    Acl acl{parent};
    acl.set_status(attribute::Status(
        agent_framework::model::enums::State::Enabled,
        agent_framework::model::enums::Health::OK));
    for(const auto& port : ports) {
        NetworkManager::get_instance()->
            get_acl_port_manager().add_entry(acl.get_uuid(), port);
    }
    acl.add_collection(attribute::Collection(
             enums::CollectionName::AclRules,
             enums::CollectionType::Rules,
             "slotMask"));
    acl.add_collection(attribute::Collection(
             enums::CollectionName::Ports,
             enums::CollectionType::Ports,
             "slotMask"));
    return acl;
}

PortVlan make_port_vlan(const std::string& parent, bool tagged = true) {
    PortVlan vlan{parent};
    vlan.set_status(attribute::Status(
        agent_framework::model::enums::State::Enabled,
        agent_framework::model::enums::Health::OK));
    vlan.set_vlan_id(1);
    vlan.set_vlan_name("VLAN Name");
    vlan.set_vlan_enable(false);
    vlan.set_tagged(tagged);
    vlan.set_oem(attribute::Oem());
    return vlan;
}

SwitchPort make_port(const std::string& parent, bool lag = false) {
    SwitchPort port{parent};
    port.set_status(attribute::Status(
        agent_framework::model::enums::State::Enabled,
        agent_framework::model::enums::Health::OK));
    port.set_port_identifier("Port ID");
    port.set_port_class(
        lag ? enums::PortClass::Logical :
              enums::PortClass::Physical);
    port.set_port_type(enums::PortType::Upstream);
    port.set_port_mode(
        lag ? enums::PortMode::LinkAggregationDynamic :
              enums::PortMode::LinkAggregationStatic);
    port.set_link_technology(enums::LinkTechnology::Ethernet);
    port.set_link_speed_mbps(5000);
    port.set_max_speed_mbps(10000);
    port.set_operational_state(enums::OperationalState::Up);
    port.set_administrative_state(enums::AdministrativeState::Up);
    port.set_port_width(1);
    port.set_frame_size(1400);
    port.set_auto_sense(true);
    port.set_full_duplex(false);
    port.set_is_management_port(false);
    port.set_last_error_code(0);
    port.set_error_cleared(false);
    port.set_last_state_change_time("2015-02-23T14:44:00+00:00");
    port.set_mac_address("AA:BB:CC:DD:EE:FF");
    port.set_ipv4_address(attribute::Ipv4Address(
            "10.0.2.10",
            "255.255.255.0",
            enums::Ipv4AddressOrigin::DHCP,
            "10.0.2.1"));
    port.set_ipv6_address(attribute::Ipv6Address(
            "fe80:1ec1:deff:fe6f:1c37",
            16,
            enums::Ipv6AddressOrigin::DHCP,
            enums::Ipv6AddressState::Preferred));
    port.set_neighbor_info(attribute::NeighborInfo(
            "NeighborID",
            "portID",
            "cableID"));
    port.set_neighbor_mac("80:00:27:2C:FF:00");
    port.set_vlan_enable(true);
    port.set_default_vlan("");
    port.add_collection(attribute::Collection(
             enums::CollectionName::PortVlans,
             enums::CollectionType::PortVlans,
             "SlotMask"));
    port.add_collection(attribute::Collection(
            enums::CollectionName::PortMembers,
            enums::CollectionType::PortMembers,
            "SlotMask"));
    port.add_collection(attribute::Collection(
             enums::CollectionName::Acls,
             enums::CollectionType::Acls,
             "SlotMask"));
    port.add_collection(attribute::Collection(
            enums::CollectionName::StaticMacs,
            enums::CollectionType::StaticMacs,
            "SlotMask"));
    port.set_oem(attribute::Oem());
    return port;
}

Vlan make_vlan(const std::string& parent) {
    Vlan vlan{parent};
    vlan.set_status(attribute::Status(
        agent_framework::model::enums::State::Enabled,
        agent_framework::model::enums::Health::OK));
    vlan.set_vlan_id(1);
    vlan.set_vlan_name("VLAN Name");
    vlan.set_vlan_enable(false);
    vlan.set_oem(attribute::Oem());
    return vlan;
}

RemoteSwitch make_remote_switch(const std::string& parent) {
    RemoteSwitch rem_switch{parent};
    rem_switch.set_status(attribute::Status(
        agent_framework::model::enums::State::Enabled,
        agent_framework::model::enums::Health::OK));
    rem_switch.set_switch_identifier("RemoteSwitchID");
    rem_switch.set_mac_address("AA:BB:CC:DD:EE:FF");
    rem_switch.add_next_hop(attribute::NextHop(
        128,
        "16",
        "10.0.2.10",
        "111::2222:3333:4444:5555"));
    rem_switch.set_oem(attribute::Oem());
    return rem_switch;
}

Switch make_ethernet_switch(const std::string& parent, const std::string& chassis) {
    Switch eth_switch{parent};
    eth_switch.set_status(attribute::Status(
        agent_framework::model::enums::State::Enabled,
        agent_framework::model::enums::Health::OK));
    eth_switch.set_switch_identifier("Switch ID");
    eth_switch.set_technology(enums::SwitchTechnology::Ethernet);
    eth_switch.set_mac_address("AA:BB:CC:DD:EE:FF");
    eth_switch.set_firmware_name("FW file name");
    eth_switch.set_firmware_version("1.1.0.2341");
    eth_switch.set_role(enums::SwitchRole::TOR);
    eth_switch.set_fru_info(attribute::FruInfo(
        "123fed3029c-b23094-12",
        "Intel Corporation",
        "E234",
        "29ee2220939"));
    eth_switch.set_manufacturing_date("02/21/2015 00:00:00");
    eth_switch.set_location(1);
    eth_switch.set_chassis(chassis);
    eth_switch.set_max_acl_number(25);
    eth_switch.add_collection(attribute::Collection(
        enums::CollectionName::Ports,
        enums::CollectionType::Ports,
        "SlotMask"));
    eth_switch.add_collection(attribute::Collection(
        enums::CollectionName::Vlans,
        enums::CollectionType::Vlans,
        "SlotMask"));
    eth_switch.add_collection(attribute::Collection(
        enums::CollectionName::NeighborSwitches,
        enums::CollectionType::NeighborSwitches,
        "SlotMask"));
    eth_switch.add_collection(attribute::Collection(
        enums::CollectionName::Acls,
        enums::CollectionType::Acls,
        "SlotMask"));
    eth_switch.set_oem(attribute::Oem());
    return eth_switch;
}

Chassis make_chassis(const std::string& parent) {
    Chassis chassis{parent};
    chassis.set_status(attribute::Status(
        agent_framework::model::enums::State::Enabled,
        agent_framework::model::enums::Health::OK));
    chassis.set_type(enums::ChassisType::Module);
    chassis.set_size(3);
    chassis.set_location_offset(1);
    chassis.set_parent_id(std::to_string(1));
    chassis.set_power_zone("");
    chassis.set_thermal_zone("");
    chassis.set_fru_info(attribute::FruInfo(
        "123fed3029c-b23094-12",
        "Intel Corporation",
        "E234",
        "29ee2220939"));
    chassis.set_sku("SKU");
    chassis.set_indicator_led(
        agent_framework::model::enums::IndicatorLed::Lit);
    chassis.set_asset_tag("Asset Tag");
    chassis.set_oem(attribute::Oem());
    return chassis;
}

Manager make_manager() {
    Manager manager{};
    manager.set_status(attribute::Status(
        agent_framework::model::enums::State::Enabled,
        agent_framework::model::enums::Health::OK));
    manager.set_firmware_version("2.58");
    manager.set_ipv4_address("1.1.2.1");
    manager.add_network_service(
        attribute::NetworkService(enums::NetworkServiceName::IPMI, 0, false));
    manager.add_network_service(
        attribute::NetworkService(enums::NetworkServiceName::SSH, 0, false));
    manager.add_network_service(
        attribute::NetworkService(enums::NetworkServiceName::Telnet, 0, false));
    agent_framework::model::attribute::SerialConsole serial{};
    serial.set_signal_type(enums::SerialConsoleSignalType::Rs232);
    serial.set_bitrate(115200);
    serial.set_parity(enums::SerialConsoleParity::None);
    serial.set_data_bits(8);
    serial.set_stop_bits(1);
    serial.set_flow_control(enums::SerialConsoleFlowControl::None);
    serial.set_pin_out(enums::SerialConsolePinOut::Cisco);
    serial.set_enabled(true);
    manager.set_serial_console(std::move(serial));
    manager.set_oem(attribute::Oem());
    manager.add_collection(attribute::Collection(
        enums::CollectionName::EthernetSwitches,
        enums::CollectionType::EthernetSwitches,
        "SlotMask"));
    manager.add_collection(attribute::Collection(
        enums::CollectionName::Chassis,
        enums::CollectionType::Chassis,
        "SlotMask"));
    return manager;
}

void create_port_vlan(const std::string& parent) {
    for (auto i = 0u; i < 2u; ++i) {
        auto port_vlan = ::make_port_vlan(parent);
        log_debug(GET_LOGGER("rpc"), "Adding port VLAN: "
            << port_vlan.get_uuid());
        NetworkManager::get_instance()->
            get_port_vlan_manager().add_entry(port_vlan);
    }
}

std::string create_default_port_vlan(const std::string& parent) {
    auto port_vlan = ::make_port_vlan(parent, false);
    log_debug(GET_LOGGER("rpc"), "Adding port VLAN: " << port_vlan.get_uuid());
    NetworkManager::get_instance()->
        get_port_vlan_manager().add_entry(port_vlan);
    return port_vlan.get_uuid();
}

std::string create_lag_port(const std::string& parent,
        const std::vector<std::string>& members) {

    auto lag_port = ::make_port(parent, true);
    auto vlan = ::create_default_port_vlan(lag_port.get_uuid());
    lag_port.set_default_vlan(vlan);

    for (const auto& member : members) {
        NetworkManager::get_instance()->get_port_members_manager().add_entry(
            lag_port.get_uuid(), member, "");
    }

    log_debug(GET_LOGGER("rpc"), "Adding LAG port: " << lag_port.get_uuid());
    NetworkManager::get_instance()->
        get_port_manager().add_entry(lag_port);
    create_port_vlan(lag_port.get_uuid());

    return lag_port.get_uuid();
}

void create_static_macs(const std::string& parent) {
    auto static_mac = ::make_nulled_static_mac(parent);
    log_debug(GET_LOGGER("rpc"), "Adding Static MAC Rule: " << static_mac.get_uuid());
    NetworkManager::get_instance()->get_static_mac_manager().
            add_entry(std::move(static_mac));
    static_mac = ::make_full_static_mac(parent);
    log_debug(GET_LOGGER("rpc"), "Adding Static MAC Rule: " << static_mac.get_uuid());
    NetworkManager::get_instance()->get_static_mac_manager().
            add_entry(std::move(static_mac));
}

void create_acl_rules(const std::string& parent) {
    auto rule = ::make_nulled_acl_rule(parent);
    log_debug(GET_LOGGER("rpc"), "Adding ACL Rule: " << rule.get_uuid());
    NetworkManager::get_instance()->get_acl_rule_manager().
        add_entry(std::move(rule));
    rule = ::make_full_acl_rule(parent);
    log_debug(GET_LOGGER("rpc"), "Adding ACL Rule: " << rule.get_uuid());
    NetworkManager::get_instance()->get_acl_rule_manager().
        add_entry(std::move(rule));
}

void create_acl(const std::string& parent,
    const attribute::Array<std::string>& ports) {
    // an acl binding all the ports
    auto acl = ::make_acl(parent, ports);
    create_acl_rules(acl.get_uuid());
    log_debug(GET_LOGGER("rpc"), "Adding ACL: " << acl.get_uuid());
    NetworkManager::get_instance()->get_acl_manager().
        add_entry(std::move(acl));
    // acls binding first 0,1,...,9 ports
    for(auto i = 0u; i < 10u; ++i) {
        acl = ::make_acl(parent, ports.subarray(0, i));
        create_acl_rules(acl.get_uuid());
        log_debug(GET_LOGGER("rpc"), "Adding ACL: " << acl.get_uuid());
        NetworkManager::get_instance()->get_acl_manager().
            add_entry(std::move(acl));
    }
}

std::vector<std::string> create_port(const std::string& parent) {
    std::vector<std::string> ports{};
    for (auto i = 0u; i < 10u; ++i) {
        auto port = ::make_port(parent);
        auto vlan = ::create_default_port_vlan(port.get_uuid());
        port.set_default_vlan(vlan);
        log_debug(GET_LOGGER("rpc"), "Adding port: " << port.get_uuid());
        NetworkManager::get_instance()->
            get_port_manager().add_entry(port);
        create_static_macs(port.get_uuid());
        create_port_vlan(port.get_uuid());
        ports.push_back(port.get_uuid());
    }
    return ports;
}

void create_vlan(const std::string& parent) {
    for (auto i = 0u; i <  2u; ++i) {
        auto vlan = ::make_vlan(parent);
        log_debug(GET_LOGGER("rpc"), "Adding switch VLAN: "
            << vlan.get_uuid());
        NetworkManager::get_instance()->
            get_vlan_manager().add_entry(vlan);
    }
}

void create_remote_switch(const std::string& parent) {
    for (auto i = 0u; i < 4u; ++i) {
        auto rem_switch = ::make_remote_switch(parent);
        log_debug(GET_LOGGER("rpc"), "Adding remote switch: "
            << rem_switch.get_uuid());
        NetworkManager::get_instance()->
            get_remote_switch_manager().add_entry(rem_switch);
    }
}

void create_ethernet_switch(const std::string& parent, const std::string& chassis) {
    for (auto i = 0u; i < 2u; ++i) {
        auto eth_switch = ::make_ethernet_switch(parent, chassis);
        log_debug(GET_LOGGER("rpc"), "Adding ethernet switch: " << eth_switch.get_uuid());
        NetworkManager::get_instance()->
            get_switch_manager().add_entry(eth_switch);
        create_remote_switch(eth_switch.get_uuid());
        create_vlan(eth_switch.get_uuid());

        // Below 10 ports are created, then 1 LAG port containing all ports.
        // Lastly, additional LAG port is created to contain the previous LAG
        // Port. This creates the structure: 1 LAG > 1 LAG > 10 ports for
        // testing

        auto ports = create_port(eth_switch.get_uuid());
        auto lag = create_lag_port(eth_switch.get_uuid(), ports);
        auto lag_containing_lag = create_lag_port(eth_switch.get_uuid(), std::vector<std::string>{lag});

        ports.push_back(lag);
        ports.push_back(lag_containing_lag);
        create_acl(eth_switch.get_uuid(), ports);
    }
}

std::string create_chassis(const std::string& parent) {
    auto chassis = ::make_chassis(parent);
    log_debug(GET_LOGGER("rpc"), "Adding chassis: " << chassis.get_uuid());
    NetworkManager::get_instance()->
        get_chassis_manager().add_entry(chassis);
    return chassis.get_uuid();
}

void create_manager() {
    auto manager = ::make_manager();
    log_debug(GET_LOGGER("rpc"), "Adding manager: " << manager.get_uuid());
    NetworkManager::get_instance()->
        get_module_manager().add_entry(manager);
    auto chassis = create_chassis(manager.get_uuid());
    create_ethernet_switch(manager.get_uuid(), chassis);
}

}

void agent::network::generate_data() {
    create_manager();
}
