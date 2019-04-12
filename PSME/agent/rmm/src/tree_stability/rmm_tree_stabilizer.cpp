/*!
 * @brief RmmTreeStabilizer class implementation.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file rmm_tree_stabilizer.cpp
 */


#include "tree_stability/rmm_tree_stabilizer.hpp"
#include "tree_stability/rmm_key_generator.hpp"
#include "tree_stability/helpers/relations_updates.hpp"
#include "agent-framework/module/chassis_components.hpp"
#include "agent-framework/module/common_components.hpp"



using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent::rmm;

using agent_framework::KeyValueMissingError;


RmmTreeStabilizer::~RmmTreeStabilizer() {}


const std::string RmmTreeStabilizer::stabilize_chassis(const std::string& in_chassis_uuid) const {
    auto& chassis_manager = get_manager<Chassis>();
    auto& power_zone_manager = get_manager<PowerZone>();
    auto& thermal_zone_manager = get_manager<ThermalZone>();

    const auto& chassis = chassis_manager.get_entry(in_chassis_uuid);
    std::string chassis_uuid{in_chassis_uuid};
    std::string chassis_unique_key{};

    try {
        if (enums::ChassisType::Zone == chassis.get_type()) {
            auto& nic_manager = get_manager<NetworkInterface>();
            const auto& nics_keys = nic_manager.get_keys(chassis.get_parent_uuid());

            if (1 != nics_keys.size()) {
                throw std::runtime_error(
                    "Improper number of network interfaces. Expected 1, found " + std::to_string(nics_keys.size()));
            }

            const auto& network_interface = nic_manager.get_entry(nics_keys.front());
            chassis_unique_key = RmmKeyGenerator::generate_key(chassis, network_interface);
        }
        else {
            chassis_unique_key = RmmKeyGenerator::generate_key(chassis);
        }

        chassis_uuid = stabilize_single_resource(in_chassis_uuid, chassis_manager, chassis_unique_key);

        helpers::update_chassis_in_relations(in_chassis_uuid, chassis_uuid);

        for (const auto& power_zone_key : power_zone_manager.get_keys(in_chassis_uuid)) {
            power_zone_manager.get_entry_reference(power_zone_key)->set_parent_uuid(chassis_uuid);
            stabilize_power_zone(power_zone_key);
        }

        for (const auto& thermal_zone_key : thermal_zone_manager.get_keys(in_chassis_uuid)) {
            thermal_zone_manager.get_entry_reference(thermal_zone_key)->set_parent_uuid(chassis_uuid);
            stabilize_thermal_zone(thermal_zone_key);
        }

    }
    catch (const std::exception& e) {
        agent_framework::log_key_value_missing(chassis.get_component().to_string(), in_chassis_uuid, e);
    }

    return chassis_uuid;
}





const std::string RmmTreeStabilizer::stabilize_network_interface(const std::string& in_network_interface_uuid) const {
    auto& nic_manager = get_manager<NetworkInterface>();
    auto& port_vlan_manager = get_manager<EthernetSwitchPortVlan>();
    const auto& network_interface = nic_manager.get_entry(in_network_interface_uuid);
    std::string network_interface_uuid{in_network_interface_uuid};

    try {
        const auto& network_interface_unique_key = RmmKeyGenerator::generate_key(network_interface);

        network_interface_uuid = stabilize_single_resource(in_network_interface_uuid, nic_manager,
                                                           network_interface_unique_key);

        for (const auto& port_vlan_key : port_vlan_manager.get_keys(in_network_interface_uuid)) {
            port_vlan_manager.get_entry_reference(port_vlan_key)->set_parent_uuid(network_interface_uuid);
            stabilize_port_vlan(port_vlan_key);
        }
    }
    catch (const std::exception& e) {
        agent_framework::log_key_value_missing(network_interface.get_component().to_string(), in_network_interface_uuid, e);
    }

    return network_interface_uuid;
}


const std::string RmmTreeStabilizer::stabilize_fan(const std::string& in_fan_uuid) const {
    auto& fan_manager = get_manager<Fan>();
    auto& thermal_zone_manager = get_manager<ThermalZone>();
    auto& chassis_manager = get_manager<Chassis>();
    auto& nic_manager = get_manager<NetworkInterface>();
    auto& module_manager = get_manager<Manager>();

    const auto& fan = fan_manager.get_entry(in_fan_uuid);
    const auto& parent_thermal_zone = thermal_zone_manager.get_entry(fan.get_parent_uuid());
    const auto& grandparent_zone = chassis_manager.get_entry(parent_thermal_zone.get_parent_uuid());
    const auto& root_zone_manager = module_manager.get_entry(grandparent_zone.get_parent_uuid());

    const auto& zone_nic_keys = nic_manager.get_keys(root_zone_manager.get_uuid());
    std::string fan_uuid{in_fan_uuid};

    try {
        if (1 != zone_nic_keys.size()) {
            throw std::runtime_error("Improper number of Zone network interfaces.");
        }

        const auto& zone_network_interface = nic_manager.get_entry(zone_nic_keys.front());

        const auto& fan_unique_key = RmmKeyGenerator::generate_key(fan, zone_network_interface);

        fan_uuid = stabilize_single_resource(in_fan_uuid, fan_manager, fan_unique_key);
    }
    catch (const std::exception& e) {
        agent_framework::log_key_value_missing(fan.get_component().to_string(), in_fan_uuid, e);
    }

    return fan_uuid;
}


const std::string RmmTreeStabilizer::stabilize_port_vlan(const std::string& in_port_vlan_uuid) const {
    auto& port_vlan_manager = get_manager<EthernetSwitchPortVlan>();
    auto& nic_manager = get_manager<NetworkInterface>();

    const auto& port_vlan = port_vlan_manager.get_entry(in_port_vlan_uuid);
    const auto& parent_nic = nic_manager.get_entry(port_vlan.get_parent_uuid());

    std::string port_vlan_uuid{in_port_vlan_uuid};

    try {
        const auto& port_vlan_unique_key = RmmKeyGenerator::generate_key(port_vlan, parent_nic);

        port_vlan_uuid = stabilize_single_resource(in_port_vlan_uuid, port_vlan_manager, port_vlan_unique_key);
    }
    catch (const std::exception& e) {
        agent_framework::log_key_value_missing(port_vlan.get_component().to_string(), in_port_vlan_uuid, e);
    }

    return port_vlan_uuid;
}


const std::string RmmTreeStabilizer::stabilize_power_zone(const std::string& in_power_zone_uuid) const {
    auto& power_zone_manager = get_manager<PowerZone>();
    auto& chassis_manager = get_manager<Chassis>();
    auto& nic_manager = get_manager<NetworkInterface>();
    auto& psu_manager = get_manager<Psu>();

    const auto& power_zone = power_zone_manager.get_entry(in_power_zone_uuid);
    const auto& zone = chassis_manager.get_entry(power_zone.get_parent_uuid());
    const auto& zone_network_interface_keys = nic_manager.get_keys(zone.get_parent_uuid());

    std::string power_zone_uuid{in_power_zone_uuid};

    try {
        if (1 != zone_network_interface_keys.size()) {
            throw std::runtime_error("Improper number of network interfaces.");
        }

        const auto& zone_network_interface = nic_manager.get_entry(zone_network_interface_keys.front());

        const auto& power_zone_unique_key = RmmKeyGenerator::generate_key(power_zone, zone_network_interface);

        power_zone_uuid = stabilize_single_resource(in_power_zone_uuid, power_zone_manager, power_zone_unique_key);

        helpers::update_power_zone_in_relations(in_power_zone_uuid, power_zone_uuid);

        for (const auto& psu_key : psu_manager.get_keys(in_power_zone_uuid)) {
            psu_manager.get_entry_reference(psu_key)->set_parent_uuid(power_zone_uuid);
            stabilize_psu(psu_key);
        }
    }
    catch (const std::exception& e) {
        agent_framework::log_key_value_missing(power_zone.get_component().to_string(), in_power_zone_uuid, e);
    }

    return power_zone_uuid;
}


const std::string RmmTreeStabilizer::stabilize_thermal_zone(const std::string& in_thermal_zone_uuid) const {
    auto& thermal_zone_manager = get_manager<ThermalZone>();
    auto& chassis_manager = get_manager<Chassis>();
    auto& nic_manager = get_manager<NetworkInterface>();
    auto& fan_manager = get_manager<Fan>();

    const auto& thermal_zone = thermal_zone_manager.get_entry(in_thermal_zone_uuid);
    const auto& zone = chassis_manager.get_entry(thermal_zone.get_parent_uuid());
    const auto& zone_network_interface_keys = nic_manager.get_keys(zone.get_parent_uuid());

    std::string thermal_zone_uuid{in_thermal_zone_uuid};

    try {
        if (1 != zone_network_interface_keys.size()) {
            throw std::runtime_error("Improper number of network interfaces.");
        }

        const auto& zone_network_interface = nic_manager.get_entry(zone_network_interface_keys.front());

        const auto& thermal_zone_unique_key = RmmKeyGenerator::generate_key(thermal_zone, zone_network_interface);

        thermal_zone_uuid = stabilize_single_resource(in_thermal_zone_uuid, thermal_zone_manager,
                                                      thermal_zone_unique_key);

        helpers::update_thermal_zone_in_relations(in_thermal_zone_uuid, thermal_zone_uuid);

        for (const auto& fan_key : fan_manager.get_keys(in_thermal_zone_uuid)) {
            fan_manager.get_entry_reference(fan_key)->set_parent_uuid(thermal_zone_uuid);
            stabilize_fan(fan_key);
        }
    }
    catch (const std::exception& e) {
        agent_framework::log_key_value_missing(thermal_zone.get_component().to_string(), in_thermal_zone_uuid, e);
    }

    return thermal_zone_uuid;
}


const std::string RmmTreeStabilizer::stabilize_psu(const std::string& in_psu_uuid) const {
    auto& psu_manager = get_manager<Psu>();
    const auto& psu = psu_manager.get_entry(in_psu_uuid);
    std::string psu_uuid{in_psu_uuid};

    try {
        const auto& psu_unique_key = RmmKeyGenerator::generate_key(psu);

        psu_uuid = stabilize_single_resource(in_psu_uuid, psu_manager, psu_unique_key);
    }
    catch (const std::exception& e) {
        agent_framework::log_key_value_missing(psu.get_component().to_string(), in_psu_uuid, e);
    }

    return psu_uuid;
}


const std::string RmmTreeStabilizer::stabilize(const std::string& in_manager_uuid) {
    log_info(LOGUSR, "Generating persistent UUID for resources for manager " << in_manager_uuid);

    auto& module_manager = get_manager<Manager>();
    const auto& manager = module_manager.get_entry(in_manager_uuid);
    std::string manager_uuid{in_manager_uuid};

    try {
        auto& chassis_manager = get_manager<Chassis>();
        auto& nic_manager = get_manager<NetworkInterface>();

        const auto& manager_nics = nic_manager.get_entries(manager_uuid);
        const std::string& manager_unique_key = RmmKeyGenerator::generate_key(manager, manager_nics);

        manager_uuid = stabilize_single_resource(manager_uuid, module_manager, manager_unique_key);

        // network interfaces must be stabilized first
        for (const auto& nic_key : nic_manager.get_keys(in_manager_uuid)) {
            nic_manager.get_entry_reference(nic_key)->set_parent_uuid(manager_uuid);
            stabilize_network_interface(nic_key);
        }

        for (const auto& chassis_key : chassis_manager.get_keys(in_manager_uuid)) {
            chassis_manager.get_entry_reference(chassis_key)->set_parent_uuid(manager_uuid);
            stabilize_chassis(chassis_key);
        }
    }
    catch (const std::exception& e) {
        agent_framework::log_key_value_missing(Manager::get_component().to_string(), in_manager_uuid, e);
    }

    return manager_uuid;
}
