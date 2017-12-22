/*!
 * @brief Provides implementation of ComputeTreeStabilizer class
 *
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
 * @file compute_tree_stabilizer.cpp
 * */


#include "tree_stability/pnc_tree_stabilizer.hpp"
#include "tree_stability/pnc_key_generator.hpp"
#include "tree_stability/helpers/update_relations.hpp"
#include "agent-framework/module/managers/generic_manager.hpp"
#include "agent-framework/module/pnc_components.hpp"
#include "agent-framework/module/common_components.hpp"

#include <algorithm>
#include <string>
#include <utility>



using namespace agent_framework::module;
using namespace agent_framework;
using namespace agent::pnc;

using agent_framework::KeyValueMissingError;

// Maintainers beware: due to hardware limitations this code is quirky and need significant
// amount of work to work on other machines.

PncTreeStabilizer::~PncTreeStabilizer() {}


const std::string PncTreeStabilizer::stabilize_storage_subsystem(const std::string& in_subsystem_uuid) const {
    auto& storage_subsystem_manager = get_manager<model::StorageSubsystem>();
    auto subsystem = storage_subsystem_manager.get_entry(in_subsystem_uuid);
    std::string subsystem_uuid{in_subsystem_uuid};

    try {
        const std::string& subsystem_unique_key = PncKeyGenerator::generate_key<model::StorageSubsystem>(subsystem);

        subsystem_uuid = stabilize_single_resource(in_subsystem_uuid,
                                                   storage_subsystem_manager,
                                                   subsystem_unique_key);

        helpers::update_storage_subsystem_in_relations(in_subsystem_uuid, subsystem_uuid);
    }
    catch (const KeyValueMissingError&) {
        log_key_value_missing(agent_framework::model::StorageSubsystem::get_component().to_string(), in_subsystem_uuid);
    }

    return subsystem_uuid;
}


const std::string PncTreeStabilizer::stabilize_system(const std::string& in_system_uuid) const {
    auto& system_manager = get_manager<model::System>();
    const auto& system = system_manager.get_entry(in_system_uuid);
    std::string system_uuid{in_system_uuid};

    try {
        const std::string& system_unique_key = PncKeyGenerator::generate_key(system);
        auto& storage_subsystem_manager = get_manager<model::StorageSubsystem>();
        const auto storage_subsystem_uuid = storage_subsystem_manager.get_keys(in_system_uuid).front();

        system_uuid = stabilize_single_resource(system_uuid, system_manager,
                                                system_unique_key);

        storage_subsystem_manager.get_entry_reference(storage_subsystem_uuid)->set_parent_uuid(system_uuid);
        stabilize_storage_subsystem(storage_subsystem_uuid);
    }
    catch (const KeyValueMissingError&) {
        log_key_value_missing(agent_framework::model::System::get_component().to_string(), in_system_uuid);
    }

    return system_uuid;
}


const std::string PncTreeStabilizer::stabilize_pcie_function(const std::string& in_function_uuid) const {
    auto& function_manager = get_manager<model::PcieFunction>();
    const auto& function = function_manager.get_entry(in_function_uuid);
    const auto& parent_device = get_manager<model::PcieDevice>().get_entry(function.get_parent_uuid());
    std::string function_uuid{in_function_uuid};

    try {
        const std::string& function_unique_key = PncKeyGenerator::generate_key(function, parent_device);

        function_uuid = stabilize_single_resource(function_uuid, function_manager,
                                                  function_unique_key);

        helpers::update_pcie_function_in_relations(in_function_uuid, function_uuid);
    }
    catch (const KeyValueMissingError&) {
        log_key_value_missing(agent_framework::model::PcieFunction::get_component().to_string(), in_function_uuid);
    }

    return function_uuid;
}


const std::string PncTreeStabilizer::stabilize_pcie_device(const std::string& in_device_uuid) const {
    auto& device_manager = get_manager<model::PcieDevice>();
    const auto& device = device_manager.get_entry(in_device_uuid);
    std::string device_uuid{in_device_uuid};

    try {
        const std::string& device_unique_key = PncKeyGenerator::generate_key(device);
        auto& function_manager = get_manager<model::PcieFunction>();

        device_uuid = stabilize_single_resource(device_uuid, device_manager,
                                                device_unique_key);

        for (const auto& function_uuid : function_manager.get_keys(in_device_uuid)) {
            function_manager.get_entry_reference(function_uuid)->set_parent_uuid(device_uuid);
            stabilize_pcie_function(function_uuid);
        }

    }
    catch (const KeyValueMissingError&) {
        log_key_value_missing(agent_framework::model::PcieDevice::get_component().to_string(), in_device_uuid);
    }

    return device_uuid;
}


const std::string PncTreeStabilizer::stabilize_drive(const std::string& in_drive_uuid) const {
    auto& drive_manager = get_manager<model::Drive>();
    const auto& drive = drive_manager.get_entry(in_drive_uuid);
    std::string drive_uuid{in_drive_uuid};

    try {
        const std::string& drive_unique_key = PncKeyGenerator::generate_key(drive);

        drive_uuid = stabilize_single_resource(drive_uuid, drive_manager,
                                               drive_unique_key);

        helpers::update_pcie_drive_in_relations(in_drive_uuid, drive_uuid);
    }
    catch (const KeyValueMissingError&) {
        log_key_value_missing(drive.get_component().to_string(), in_drive_uuid);
    }

    return drive_uuid;
}


const std::string PncTreeStabilizer::stabilize_chassis(const std::string& in_chassis_uuid) const {
    auto& chassis_manager = get_manager<model::Chassis>();
    const auto& chassis = chassis_manager.get_entry(in_chassis_uuid);
    std::string chassis_uuid{in_chassis_uuid};
    auto& switch_manager = get_manager<model::Switch>();

    const auto& switch_keys = switch_manager.get_keys(
        [&chassis](const model::Switch& in_pcie_switch) {
            return in_pcie_switch.get_chassis() == chassis.get_uuid();
        });

    if (switch_keys.empty()) {
        throw std::runtime_error("No switch found.");
    }

    try {
        const auto& pcie_switch = switch_manager.get_entry(switch_keys.front());
        const std::string& chassis_unique_key = PncKeyGenerator::generate_key(chassis, pcie_switch);
        auto& drive_manager = get_manager<model::Drive>();

        chassis_uuid = stabilize_single_resource(chassis_uuid, chassis_manager,
                                                 chassis_unique_key);

        helpers::update_chassis_in_relations(in_chassis_uuid, chassis_uuid);

        for (const auto& drive_uuid : drive_manager.get_keys(in_chassis_uuid)) {
            drive_manager.get_entry_reference(drive_uuid)->set_parent_uuid(chassis_uuid);
            stabilize_drive(drive_uuid);
        }
    }
    catch (const KeyValueMissingError&) {
        log_key_value_missing(chassis.get_component().to_string(), chassis_uuid);
    }

    return chassis_uuid;
}


const std::string PncTreeStabilizer::stabilize_fabric(const std::string& in_fabric_uuid) const {
    auto& fabric_manager = get_manager<model::Fabric>();
    const auto& fabric = fabric_manager.get_entry(in_fabric_uuid);
    std::string fabric_uuid{in_fabric_uuid};

    try {
        const std::string& fabric_unique_key = PncKeyGenerator::generate_key(fabric);
        auto& switch_manager = get_manager<model::Switch>();
        auto& zones_manager = get_manager<model::Zone>();
        auto& endpoints_manager = get_manager<model::Endpoint>();

        fabric_uuid = stabilize_single_resource(fabric_uuid, fabric_manager,
                                                fabric_unique_key);

        for (const auto& switch_uuid : switch_manager.get_keys(in_fabric_uuid)) {
            switch_manager.get_entry_reference(switch_uuid)->set_parent_uuid(fabric_uuid);
            stabilize_pcie_switch(switch_uuid);
        }

        for (const auto& zone_uuid : zones_manager.get_keys(in_fabric_uuid)) {
            zones_manager.get_entry_reference(zone_uuid)->set_parent_uuid(fabric_uuid);
            stabilize_pcie_zone(zone_uuid);
        }

        for (const auto& endpoint_uuid : endpoints_manager.get_keys(in_fabric_uuid)) {
            endpoints_manager.get_entry_reference(endpoint_uuid)->set_parent_uuid(fabric_uuid);
            stabilize_pcie_endpoint(endpoint_uuid);
        }

    }
    catch (const KeyValueMissingError&) {
        log_key_value_missing(fabric.get_component().to_string(), in_fabric_uuid);
    }

    return fabric_uuid;
}


const std::string PncTreeStabilizer::stabilize_pcie_switch(const std::string& in_switch_uuid) const {
    auto& switch_manager = get_manager<model::Switch>();
    const auto& pcie_switch = switch_manager.get_entry(in_switch_uuid);
    std::string switch_uuid{in_switch_uuid};

    try {
        const std::string& switch_unique_key = PncKeyGenerator::generate_key(pcie_switch);
        auto& ports_manager = get_manager<model::Port>();

        switch_uuid = stabilize_single_resource(switch_uuid, switch_manager,
                                                switch_unique_key);

        helpers::update_pcie_switch_in_relations(in_switch_uuid, switch_uuid);

        for (const auto& port_uuid : ports_manager.get_keys(in_switch_uuid)) {
            ports_manager.get_entry_reference(port_uuid)->set_parent_uuid(switch_uuid);
            stabilize_port(port_uuid);
        }
    }
    catch (const KeyValueMissingError&) {
        log_key_value_missing(pcie_switch.get_component().to_string(), in_switch_uuid);
    }

    return switch_uuid;
}


const std::string PncTreeStabilizer::stabilize_port(const std::string& in_port_uuid) const {
    auto& ports_manager = get_manager<model::Port>();
    const auto& port = ports_manager.get_entry(in_port_uuid);
    const auto& parent_switch = get_manager<model::Switch>().get_entry(port.get_parent_uuid());
    std::string port_uuid{in_port_uuid};

    try {
        const auto& port_unique_key = PncKeyGenerator::generate_key(port, parent_switch);

        port_uuid = stabilize_single_resource(port_uuid, ports_manager,
                                              port_unique_key);

        helpers::update_pcie_port_in_relations(in_port_uuid, port_uuid);
        stabilize_port_metric(in_port_uuid, port_uuid);

    }
    catch (const KeyValueMissingError&) {
        log_key_value_missing(port.get_component().to_string(), in_port_uuid);
    }

    return port_uuid;
}


void PncTreeStabilizer::stabilize_port_metric(const std::string& in_port_uuid, const std::string& port_uuid) const {
    auto& metrics_manager = get_manager<model::Metric>();

    const auto keys = metrics_manager.get_keys([&in_port_uuid](const model::Metric& metric) {
        return metric.get_component_uuid() == in_port_uuid;
    });
    if (keys.size() != 1) {
        log_error("pnc-agent", "Invalid number of metrics (" << keys.size() << ") found for port " << port_uuid << ".");
        return;
    }

    auto metric = metrics_manager.get_entry_reference(keys.front());
    metric->set_component_uuid(port_uuid);
    metric->set_unique_key(port_uuid);
    metric->make_persistent_uuid();
    log_persistent_uuid_generated(model::Metric::get_component().to_string(), keys.front(), metric->get_uuid());
}


const std::string PncTreeStabilizer::stabilize_pcie_zone(const std::string& in_zone_uuid) const {
    auto& zones_manager = get_manager<model::Zone>();
    const auto& zone = zones_manager.get_entry(in_zone_uuid);
    const auto& parent_switch = get_manager<model::Switch>().get_entry(zone.get_switch_uuid());
    std::string zone_uuid{in_zone_uuid};

    try {
        const std::string& zone_unique_key = PncKeyGenerator::generate_key(zone, parent_switch);

        zone_uuid = stabilize_single_resource(zone_uuid, zones_manager, zone_unique_key);

        helpers::update_pcie_zone_in_relations(in_zone_uuid, zone_uuid);
    }
    catch (const KeyValueMissingError&) {
        log_key_value_missing(zone.get_component().to_string(), in_zone_uuid);
    }

    return zone_uuid;
}


const std::string PncTreeStabilizer::stabilize_pcie_endpoint(const std::string& in_endpoint_uuid) const {
    auto& endpoints_manager = get_manager<model::Endpoint>();
    const auto& endpoint = endpoints_manager.get_entry(in_endpoint_uuid);
    std::string endpoint_uuid{in_endpoint_uuid};

    try {
        auto& ports_manager = get_manager<model::Port>();
        std::vector<model::Port> endpoint_ports{};

        for (const auto& port_uuid : get_m2m_manager<model::Endpoint, model::Port>().get_children(in_endpoint_uuid)) {
            endpoint_ports.push_back(ports_manager.get_entry(port_uuid));
        }

        const std::string& endpoint_unique_key = PncKeyGenerator::generate_key(endpoint, endpoint_ports);

        endpoint_uuid = stabilize_single_resource(endpoint_uuid, endpoints_manager, endpoint_unique_key);

        helpers::update_pcie_endpoint_in_relations(in_endpoint_uuid, endpoint_uuid);
    }
    catch (const KeyValueMissingError&) {
        log_key_value_missing(endpoint.get_component().to_string(), in_endpoint_uuid);
    }

    return endpoint_uuid;
}


const std::string PncTreeStabilizer::stabilize(const std::string& in_module_uuid) {
    log_info("pnc-agent", "Generating persistent UUID for resources for Manager " << in_module_uuid);

    auto& module_manager = get_manager<model::Manager>();
    const auto& manager = module_manager.get_entry(in_module_uuid);
    std::string module_uuid{in_module_uuid};

    try {
        auto& fabrics_manager = get_manager<model::Fabric>();
        auto& switch_manager = get_manager<model::Switch>();
        auto& chassis_manager = get_manager<model::Chassis>();
        auto& systems_manager = get_manager<model::System>();
        auto& devices_manager = get_manager<model::PcieDevice>();

        const auto& switch_keys = switch_manager.get_keys();
        if (1 != switch_keys.size()) {
            throw std::runtime_error("No managed switch found.");
        }

        const auto& managed_switch = switch_manager.get_entry(switch_keys.front());
        const std::string& module_unique_key = PncKeyGenerator::generate_key(manager, managed_switch);

        module_uuid = stabilize_single_resource(module_uuid, module_manager, module_unique_key);

        for (const auto& chassis_uuid : chassis_manager.get_keys(in_module_uuid)) {
            chassis_manager.get_entry_reference(chassis_uuid)->set_parent_uuid(module_uuid);
            stabilize_chassis(chassis_uuid);
        }

        for (const auto& fabric_uuid : fabrics_manager.get_keys(in_module_uuid)) {
            fabrics_manager.get_entry_reference(fabric_uuid)->set_parent_uuid(module_uuid);
            stabilize_fabric(fabric_uuid);
        }

        for (const auto& system_uuid : systems_manager.get_keys(in_module_uuid)) {
            systems_manager.get_entry_reference(system_uuid)->set_parent_uuid(module_uuid);
            stabilize_system(system_uuid);
        }

        for (const auto& device_uuid : devices_manager.get_keys(in_module_uuid)) {
            devices_manager.get_entry_reference(device_uuid)->set_parent_uuid(module_uuid);
            stabilize_pcie_device(device_uuid);
        }

    }
    catch (const KeyValueMissingError&) {
        log_key_value_missing(agent_framework::model::Manager::get_component().to_string(), in_module_uuid);
    }

    return module_uuid;
}

const std::string PncTreeStabilizer::stabilize_metric_definition(const std::string& in_definition_uuid) const {
    auto& definition_manager = get_manager<model::MetricDefinition>();
    const auto& definition = definition_manager.get_entry(in_definition_uuid);
    std::string definition_uuid{in_definition_uuid};

    try {
        const auto& key = PncKeyGenerator::generate_key(definition);
        definition_uuid = stabilize_single_resource(in_definition_uuid, definition_manager, key);
    }
    catch (const KeyValueMissingError&) {
        log_key_value_missing(definition.get_component().to_string(), in_definition_uuid);
    }

    return definition_uuid;
}
