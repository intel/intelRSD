/*!
 * @brief Provides implementation of NetworkTreeStabilizer class
 *
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
 * @file network_tree_stabilizer.cpp
 * */

#include "tree_stability/network_tree_stabilizer.hpp"
#include "tree_stability/helpers/update_relations.hpp"
#include "logger/logger.hpp"
#include "agent-framework/module/managers/generic_manager.hpp"
#include "agent-framework/module/network_components.hpp"
#include "agent-framework/module/common_components.hpp"



using namespace agent::network;
using namespace agent_framework::module;

namespace {

/*! Auxiliary namespace to hold prefixes for unique key construction, needed to maintain data integrity */
namespace literals {
constexpr const char MODULE_KEY_BASE[] = "NetworkModule_";
constexpr const char CHASSIS_KEY_BASE[] = "NetworkChassis_";
constexpr const char SWITCH_KEY_BASE[] = "EthernetSwitch_";
constexpr const char PORT_KEY_BASE[] = "Port_";
constexpr const char PORT_VLAN_KEY_BASE[] = "PortVLAN_";
constexpr const char STATIC_MAC_KEY_BASE[] = "StaticMac_";
constexpr const char ACL_KEY_BASE[] = "ACL_";
constexpr const char ACL_RULE_KEY_BASE[] = "ACLRule_";

}
}


NetworkTreeStabilizer::~NetworkTreeStabilizer() { }


const std::string NetworkTreeStabilizer::stabilize_chassis(const std::string& chassis_uuid) {
    auto& chassis_manager = CommonComponents::get_instance()->get_chassis_manager();
    auto& switch_manager = NetworkComponents::get_instance()->get_switch_manager();

    const auto& switch_keys = switch_manager.get_keys(
        [&chassis_uuid](const agent_framework::model::EthernetSwitch& eth_switch) {
            return eth_switch.get_chassis() == chassis_uuid;
        });
    if (switch_keys.empty()) {
        throw std::runtime_error("No switch found for chassis " + chassis_uuid);
    }

    const auto switch_identifier = switch_manager.get_entry(switch_keys.front()).get_switch_identifier();
    const auto chassis = chassis_manager.get_entry(chassis_uuid);

    if (switch_identifier.has_value()) {
        const auto chassis_persistent_uuid = stabilize_single_resource(chassis_uuid, chassis_manager,
                                                                       literals::CHASSIS_KEY_BASE +
                                                                       switch_identifier.value());
        helpers::update_chassis_in_relations(chassis_uuid, chassis_persistent_uuid);
        return chassis_persistent_uuid;
    }
    else {
        agent_framework::log_key_value_missing(chassis.get_component().to_string(), chassis_uuid);
        return chassis_uuid;
    }
}


const std::string NetworkTreeStabilizer::stabilize_switch(const std::string& switch_uuid) {
    auto& switch_manager = NetworkComponents::get_instance()->get_switch_manager();
    auto& port_manager = NetworkComponents::get_instance()->get_port_manager();
    auto& acl_manager = NetworkComponents::get_instance()->get_acl_manager();

    const auto ethernet_switch = switch_manager.get_entry(switch_uuid);
    if (ethernet_switch.get_switch_identifier().has_value()) {
        const auto switch_identifier = ethernet_switch.get_switch_identifier().value();
        const auto& ports_uuids = port_manager.get_keys(switch_uuid);
        const auto& acls_uuids = acl_manager.get_keys(switch_uuid);

        const auto switch_persistent_uuid = stabilize_single_resource(switch_uuid, switch_manager,
                                                                      literals::SWITCH_KEY_BASE + switch_identifier);

        for (const auto& port_uuid : ports_uuids) {
            port_manager.get_entry_reference(port_uuid)->set_parent_uuid(switch_persistent_uuid);
            stabilize_port(port_uuid);
        }

        for (const auto& acl_uuid : acls_uuids) {
            acl_manager.get_entry_reference(acl_uuid)->set_parent_uuid(switch_persistent_uuid);
            stabilize_acl(acl_uuid);
        }

        return switch_persistent_uuid;
    }
    else {
        agent_framework::log_key_value_missing(ethernet_switch.get_component().to_string(), switch_uuid);
        return switch_uuid;
    }
}


const std::string NetworkTreeStabilizer::stabilize_acl_rule(const std::string& acl_rule_uuid) {
    auto& acl_rule_manager = NetworkComponents::get_instance()->get_acl_rule_manager();
    auto& acl_manager = NetworkComponents::get_instance()->get_acl_manager();

    auto acl_rule = acl_rule_manager.get_entry(acl_rule_uuid);
    auto parent_acl = acl_manager.get_entry(acl_rule.get_parent_uuid());

    if (parent_acl.has_persistent_uuid() && acl_rule.get_rule_id().has_value()) {
        auto rule_id = std::to_string(acl_rule.get_rule_id().value());
        const auto acl_rule_persistent_uuid = stabilize_single_resource(acl_rule_uuid, acl_rule_manager,
                                                                        literals::ACL_RULE_KEY_BASE +
                                                                        parent_acl.get_uuid() + rule_id);
        return acl_rule_persistent_uuid;
    }
    else {
        agent_framework::log_key_value_missing(acl_rule.get_component().to_string(), acl_rule_uuid);
        return acl_rule_uuid;
    }
}


const std::string NetworkTreeStabilizer::stabilize_acl(const std::string& acl_uuid) {
    auto& acl_manager = NetworkComponents::get_instance()->get_acl_manager();
    auto& acl_rule_manager = NetworkComponents::get_instance()->get_acl_rule_manager();
    auto& switch_manager = NetworkComponents::get_instance()->get_switch_manager();

    auto acl = acl_manager.get_entry(acl_uuid);
    auto parent_switch = switch_manager.get_entry(acl.get_parent_uuid());

    if (parent_switch.has_persistent_uuid() && acl.get_name().has_value()) {
        const auto acl_name = acl.get_name().value();
        const auto& acl_rules_uuids = acl_rule_manager.get_keys(acl_uuid);

        const auto acl_persistent_uuid = stabilize_single_resource(acl_uuid, acl_manager,
                                                                   literals::ACL_KEY_BASE + parent_switch.get_uuid() +
                                                                   acl_name);
        helpers::update_acl_in_relations(acl_uuid, acl_persistent_uuid);

        for (const auto& acl_rule_uuid : acl_rules_uuids) {
            acl_rule_manager.get_entry_reference(acl_rule_uuid)->set_parent_uuid(acl_persistent_uuid);
            stabilize_acl_rule(acl_rule_uuid);
        }

        return acl_persistent_uuid;
    }
    else {
        agent_framework::log_key_value_missing(acl.get_component().to_string(), acl_uuid);
        return acl_uuid;
    }
}


const std::string NetworkTreeStabilizer::stabilize_port_vlan(const std::string& port_vlan_uuid) {
    auto& port_vlan_manager = NetworkComponents::get_instance()->get_port_vlan_manager();
    auto& port_manager = NetworkComponents::get_instance()->get_port_manager();

    auto port_vlan = port_vlan_manager.get_entry(port_vlan_uuid);
    auto parent_port = port_manager.get_entry(port_vlan.get_parent_uuid());

    if (parent_port.has_persistent_uuid() &&
        port_vlan.get_vlan_id().has_value() && port_vlan.get_tagged().has_value()) {
        auto port_vlan_id = std::to_string(port_vlan.get_vlan_id().value());
        auto port_tagged = std::to_string(port_vlan.get_tagged());
        const auto port_vlan_persistent_uuid = stabilize_single_resource(port_vlan_uuid,
                                                                         port_vlan_manager,
                                                                         literals::PORT_VLAN_KEY_BASE +
                                                                         parent_port.get_uuid() +
                                                                         port_vlan_id + port_tagged);
        return port_vlan_persistent_uuid;
    }
    else {
        agent_framework::log_key_value_missing(port_vlan.get_component().to_string(), port_vlan_uuid);
        return port_vlan_uuid;
    }
}


const std::string NetworkTreeStabilizer::stabilize_static_mac(const std::string& static_mac_uuid) {
    auto& static_mac_manager = NetworkComponents::get_instance()->get_static_mac_manager();
    auto& port_manager = NetworkComponents::get_instance()->get_port_manager();

    auto static_mac = static_mac_manager.get_entry(static_mac_uuid);
    auto parent_port = port_manager.get_entry(static_mac.get_parent_uuid());

    if (parent_port.has_persistent_uuid() &&
        static_mac.get_address().has_value() && static_mac.get_vlan_id().has_value()) {
        auto mac_address = static_mac.get_address().value();
        auto vlan_id = std::to_string(static_mac.get_vlan_id().value());

        const auto static_mac_persistent_uuid = stabilize_single_resource(static_mac_uuid,
                                                                          static_mac_manager,
                                                                          literals::STATIC_MAC_KEY_BASE +
                                                                          parent_port.get_uuid() + mac_address +
                                                                          vlan_id);
        return static_mac_persistent_uuid;
    }
    else {
        agent_framework::log_key_value_missing(static_mac.get_component().to_string(), static_mac_uuid);
        return static_mac_uuid;
    }
}


const std::string NetworkTreeStabilizer::stabilize_port(const std::string& port_uuid) {
    auto& port_manager = NetworkComponents::get_instance()->get_port_manager();
    auto& static_mac_manager = NetworkComponents::get_instance()->get_static_mac_manager();
    auto& port_vlan_manager = NetworkComponents::get_instance()->get_port_vlan_manager();
    auto& switch_manager = NetworkComponents::get_instance()->get_switch_manager();

    auto port = port_manager.get_entry(port_uuid);
    auto parent_switch = switch_manager.get_entry(port.get_parent_uuid());

    if (parent_switch.has_persistent_uuid() && port.get_port_identifier().has_value()) {
        const auto port_identifier = port.get_port_identifier().value();
        const auto& static_macs_uuids = static_mac_manager.get_keys(port_uuid);
        const auto& port_vlans_uuids = port_vlan_manager.get_keys(port_uuid);

        const auto port_persistent_uuid = stabilize_single_resource(port_uuid, port_manager,
                                                                    literals::PORT_KEY_BASE + port_identifier);
        helpers::update_port_in_relations(port_uuid, port_persistent_uuid);

        for (const auto& static_mac_uuid: static_macs_uuids) {
            static_mac_manager.get_entry_reference(static_mac_uuid)->set_parent_uuid(port_persistent_uuid);
            stabilize_static_mac(static_mac_uuid);
        }

        for (const auto& port_vlan_uuid: port_vlans_uuids) {
            port_vlan_manager.get_entry_reference(port_vlan_uuid)->set_parent_uuid(port_persistent_uuid);
            stabilize_port_vlan(port_vlan_uuid);
        }

        return port_persistent_uuid;
    }
    else {
        agent_framework::log_key_value_missing(port.get_component().to_string(), port_uuid);
        return port_uuid;
    }
}


const std::string NetworkTreeStabilizer::stabilize(const std::string& module_uuid) {
    auto& module_manager = CommonComponents::get_instance()->get_module_manager();
    auto& switch_manager = NetworkComponents::get_instance()->get_switch_manager();
    auto& chassis_manager = CommonComponents::get_instance()->get_chassis_manager();
    const auto& switch_keys = switch_manager.get_keys(module_uuid);

    if (switch_keys.empty()) {
        throw std::runtime_error("No switch found for module " + module_uuid);
    }

    const auto module = module_manager.get_entry(module_uuid);
    const auto ethernet_switch = switch_manager.get_entry(switch_keys.front());
    const auto chassis = chassis_manager.get_entry(ethernet_switch.get_chassis());

    if (ethernet_switch.get_switch_identifier().has_value()) {
        const auto switch_identifier = ethernet_switch.get_switch_identifier().value();

        const std::string module_persistent_uuid = stabilize_single_resource(module_uuid, module_manager,
                                                                             literals::MODULE_KEY_BASE +
                                                                             switch_identifier);

        switch_manager.get_entry_reference(ethernet_switch.get_uuid())->set_parent_uuid(module_persistent_uuid);
        stabilize_switch(ethernet_switch.get_uuid());

        chassis_manager.get_entry_reference(chassis.get_uuid())->set_parent_uuid(module_persistent_uuid);
        stabilize_chassis(chassis.get_uuid());

        return module_persistent_uuid;
    }
    else {
        agent_framework::log_key_value_missing(module.get_component().to_string(), module_uuid);
        return module_uuid;
    }
}
