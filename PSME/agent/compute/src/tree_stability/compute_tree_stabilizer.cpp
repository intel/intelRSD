/*!
 * @brief Provides implementation of ComputeTreeStabilizer class
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
 * @file compute_tree_stabilizer.cpp
 * */



#include "tree_stability/compute_tree_stabilizer.hpp"
#include "tree_stability/helpers/update_relations.hpp"

#include "agent-framework/module/managers/generic_manager.hpp"
#include "agent-framework/module/compute_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/enum/common.hpp"

#include <algorithm>
#include <vector>



using namespace agent_framework::module;
using namespace agent_framework::model::enums;
using namespace agent::compute;

using agent_framework::model::attribute::Array;

/*! Auxiliary namespace to hold prefixes for unique key construction, needed to maintain data integrity */
namespace literals {

constexpr const char MODULE_KEY_BASE[] = "Module_";
constexpr const char SYSTEM_KEY_BASE[] = "System_";
constexpr const char CHASSIS_KEY_BASE[] = "Chassis_";
constexpr const char PROCESSOR_KEY_BASE[] = "Processor_";
constexpr const char MEMORY_KEY_BASE[] = "Memory_";
constexpr const char NETWORK_INTERFACE_KEY_BASE[] = "Interface_";
constexpr const char NETWORK_DEVICE_KEY_BASE[] = "Device_";
constexpr const char NETWORK_DEVICE_FUNCTION_KEY_BASE[] = "Function_";
constexpr const char STORAGE_CONTROLLER_KEY_BASE[] = "StorageController_";
constexpr const char STORAGE_SUBSYSTEM_KEY_BASE[] = "StorageSubsystem_";
constexpr const char DRIVE_KEY_BASE[] = "Drive_";
constexpr const char TRUSTED_MODULE_KEY_BASE[] = "TrustedModule_";

}


ComputeTreeStabilizer::~ComputeTreeStabilizer() {}


const std::string ComputeTreeStabilizer::stabilize_network_device_function(
    const std::string& network_device_function_uuid) {
    auto& network_device_function_manager = ComputeComponents::get_instance()->get_network_device_function_manager();
    auto& network_device_manager = ComputeComponents::get_instance()->get_network_device_manager();

    const auto& network_device_function = network_device_function_manager.get_entry(network_device_function_uuid);
    const auto& network_device = network_device_manager.get_entry(network_device_function.get_parent_uuid());

    if (network_device.has_persistent_uuid()) {
        const std::string& network_device_function_persistent_uuid = stabilize_single_resource(
            network_device_function_uuid,
            network_device_function_manager,
            literals::NETWORK_DEVICE_FUNCTION_KEY_BASE +
            network_device.get_uuid());
        return network_device_function_persistent_uuid;
    }
    else {
        agent_framework::log_key_value_missing(network_device_function.get_component().to_string(), network_device_function_uuid);
        return network_device_function_uuid;
    }
}


const std::string ComputeTreeStabilizer::stabilize_network_device(const std::string& network_device_uuid) {
    auto& network_device_function_manager = ComputeComponents::get_instance()->get_network_device_function_manager();
    auto& network_device_manager = ComputeComponents::get_instance()->get_network_device_manager();
    auto& system_manager = CommonComponents::get_instance()->get_system_manager();

    const auto& network_device = network_device_manager.get_entry(network_device_uuid);
    const auto& parent_system = system_manager.get_entry(network_device.get_parent_uuid());

    if (parent_system.has_persistent_uuid()) {
        const std::string& network_device_persistent_uuid = stabilize_single_resource(network_device_uuid,
                                                                                      network_device_manager,
                                                                                      literals::NETWORK_DEVICE_KEY_BASE +
                                                                                      parent_system.get_uuid());

        const auto& network_device_function_uuids = network_device_function_manager.get_keys(network_device.get_uuid());
        for (const auto& network_device_function_uuid : network_device_function_uuids) {
            network_device_function_manager.get_entry_reference(network_device_function_uuid)->set_parent_uuid(
                network_device_persistent_uuid);
            stabilize_network_device_function(network_device_function_uuid);
        }

        return network_device_persistent_uuid;
    }
    else {
        agent_framework::log_key_value_missing(network_device.get_component().to_string(), network_device_uuid);
        return network_device_uuid;
    }
}


const std::string ComputeTreeStabilizer::stabilize_storage_controller(const std::string& storage_controller_uuid) {
    auto& storage_controller_manager = ComputeComponents::get_instance()->get_storage_controller_manager();
    auto& storage_subsystem_manager = CommonComponents::get_instance()->get_storage_subsystem_manager();

    const auto& storage_controller = storage_controller_manager.get_entry(storage_controller_uuid);
    const auto& parent_storage_subsystem = storage_subsystem_manager.get_entry(storage_controller.get_parent_uuid());

    if (parent_storage_subsystem.has_persistent_uuid()) {
        const std::string& storage_controller_persistent_uuid = stabilize_single_resource(storage_controller_uuid,
                                                                                          storage_controller_manager,
                                                                                          literals::STORAGE_CONTROLLER_KEY_BASE +
                                                                                          parent_storage_subsystem.get_uuid());
        return storage_controller_persistent_uuid;
    }
    else {
        agent_framework::log_key_value_missing(storage_controller.get_component().to_string(), storage_controller_uuid);
        return storage_controller_uuid;
    }
}


const std::string ComputeTreeStabilizer::stabilize_storage_subsystem(const std::string& storage_subsystem_uuid) {
    auto& storage_subsystem_manager = CommonComponents::get_instance()->get_storage_subsystem_manager();
    auto& storage_controller_manager = ComputeComponents::get_instance()->get_storage_controller_manager();
    auto& system_manager = CommonComponents::get_instance()->get_system_manager();

    const auto& storage_subsystem = storage_subsystem_manager.get_entry(storage_subsystem_uuid);
    const auto& parent_system = system_manager.get_entry(storage_subsystem.get_parent_uuid());

    if (parent_system.has_persistent_uuid()) {
        const std::string& storage_subsystem_persistent_uuid = stabilize_single_resource(storage_subsystem_uuid,
                                                                                         storage_subsystem_manager,
                                                                                         literals::STORAGE_SUBSYSTEM_KEY_BASE +
                                                                                         parent_system.get_uuid());

        const auto& storage_controllers_uuids = storage_controller_manager.get_keys(storage_subsystem.get_uuid());
        for (const auto& storage_controller_uuid : storage_controllers_uuids) {
            storage_controller_manager.get_entry_reference(storage_controller_uuid)->set_parent_uuid(
                storage_subsystem_persistent_uuid);
            stabilize_storage_controller(storage_controller_uuid);
        }

        return storage_subsystem_persistent_uuid;
    }
    else {
        agent_framework::log_key_value_missing(storage_subsystem.get_component().to_string(), storage_subsystem_uuid);
        return storage_subsystem_uuid;
    }
}


const std::string ComputeTreeStabilizer::stabilize_nic(const std::string& nic_uuid) {
    auto& nic_manager = get_manager<agent_framework::model::NetworkInterface>();
    auto& system_manager = CommonComponents::get_instance()->get_system_manager();

    const auto& nic = nic_manager.get_entry(nic_uuid);
    const auto& parent_system = system_manager.get_entry(nic.get_parent_uuid());

    if (parent_system.has_persistent_uuid() && nic.get_mac_address().has_value()) {
        const auto mac_address = nic.get_mac_address().value();
        const std::string& nic_persistent_uuid = stabilize_single_resource(nic_uuid, nic_manager,
                                                                           literals::NETWORK_INTERFACE_KEY_BASE +
                                                                           parent_system.get_uuid() + mac_address);
        return nic_persistent_uuid;
    }
    else {
        agent_framework::log_key_value_missing(nic.get_component().to_string(), nic_uuid);
        return nic_uuid;
    }
}


const std::string ComputeTreeStabilizer::stabilize_memory(const std::string& memory_uuid) {
    auto& memory_manager = ComputeComponents::get_instance()->get_memory_manager();
    auto& system_manager = CommonComponents::get_instance()->get_system_manager();

    const auto& memory = memory_manager.get_entry(memory_uuid);
    const auto& parent_system = system_manager.get_entry(memory.get_parent_uuid());

    if (parent_system.has_persistent_uuid() && memory.get_device_locator().has_value()) {
        const auto device_locator = memory.get_device_locator().value();
        const std::string& memory_persistent_uuid = stabilize_single_resource(memory_uuid, memory_manager,
                                                                              literals::MEMORY_KEY_BASE +
                                                                              parent_system.get_uuid() +
                                                                              device_locator);
        return memory_persistent_uuid;
    }
    else {
        agent_framework::log_key_value_missing(memory.get_component().to_string(), memory_uuid);
        return memory_uuid;
    }
}


const std::string ComputeTreeStabilizer::stabilize_processor(const std::string& processor_uuid) {
    auto& processor_manager = get_manager<agent_framework::model::Processor>();
    auto& system_manager = CommonComponents::get_instance()->get_system_manager();

    const auto& processor = processor_manager.get_entry(processor_uuid);
    const auto& parent_system = system_manager.get_entry(processor.get_parent_uuid());

    if (parent_system.has_persistent_uuid() && processor.get_socket().has_value()) {
        const auto socket = processor.get_socket().value();
        const std::string& processor_persistent_uuid = stabilize_single_resource(processor_uuid, processor_manager,
                                                                                  literals::PROCESSOR_KEY_BASE +
                                                                                  parent_system.get_uuid() + socket);
        return processor_persistent_uuid;
    }
    else {
        agent_framework::log_key_value_missing(processor.get_component().to_string(), processor_uuid);
        return processor_uuid;
    }
}


const std::string ComputeTreeStabilizer::stabilize_trusted_module(const std::string& uuid) {
    auto& manager = get_manager<agent_framework::model::TrustedModule>();
    auto& system_manager = get_manager<agent_framework::model::System>();

    const auto& module = manager.get_entry(uuid);
    const auto& parent_system = system_manager.get_entry(module.get_parent_uuid());

    if (parent_system.has_persistent_uuid() && module.get_interface_type().has_value()) {
        const auto interface_type = module.get_interface_type().value().to_string();
        const std::string& persistent_uuid = stabilize_single_resource(uuid, manager,
                                                                       literals::TRUSTED_MODULE_KEY_BASE +
                                                                       parent_system.get_uuid() + interface_type);
        return persistent_uuid;
    }
    else {
        agent_framework::log_key_value_missing(module.get_component().to_string(), uuid);
        return uuid;
    }
}


const std::string ComputeTreeStabilizer::stabilize_system(const std::string& system_uuid) {
    auto& system_manager = CommonComponents::get_instance()->get_system_manager();
    auto& processor_manager = get_manager<agent_framework::model::Processor>();
    auto& memory_manager = ComputeComponents::get_instance()->get_memory_manager();
    auto& nic_manager = get_manager<agent_framework::model::NetworkInterface>();
    auto& storage_subsystem_manager = CommonComponents::get_instance()->get_storage_subsystem_manager();
    auto& network_device_manager = ComputeComponents::get_instance()->get_network_device_manager();
    auto& trusted_module_manager = get_manager<agent_framework::model::TrustedModule>();

    const auto& system = system_manager.get_entry(system_uuid);

    if (system.get_guid().has_value()) {
        const auto system_guid = system.get_guid();
        const auto& processors_uuids = processor_manager.get_keys(system_uuid);
        const auto& memories_uuids = memory_manager.get_keys(system_uuid);
        const auto& nics_uuids = nic_manager.get_keys(system_uuid);
        const auto& storage_subsystem_uuids = storage_subsystem_manager.get_keys(system_uuid);
        const auto& network_device_uuids = network_device_manager.get_keys(system_uuid);
        const auto& trusted_module_uuids = trusted_module_manager.get_keys(system_uuid);

        const std::string& system_persistent_uuid = stabilize_single_resource(system_uuid, system_manager,
                                                                              literals::SYSTEM_KEY_BASE +
                                                                              system_guid.value());

        for (const auto& processor_uuid : processors_uuids) {
            processor_manager.get_entry_reference(processor_uuid)->set_parent_uuid(system_persistent_uuid);
            stabilize_processor(processor_uuid);
        }

        for (const auto& memory_uuid : memories_uuids) {
            memory_manager.get_entry_reference(memory_uuid)->set_parent_uuid(system_persistent_uuid);
            stabilize_memory(memory_uuid);
        }

        for (const auto& nic_uuid : nics_uuids) {
            nic_manager.get_entry_reference(nic_uuid)->set_parent_uuid(system_persistent_uuid);
            stabilize_nic(nic_uuid);
        }

        for (const auto& storage_subsystem_uuid : storage_subsystem_uuids) {
            storage_subsystem_manager.get_entry_reference(storage_subsystem_uuid)->set_parent_uuid(
                system_persistent_uuid);
            stabilize_storage_subsystem(storage_subsystem_uuid);
        }

        for (const auto& network_device_uuid : network_device_uuids) {
            network_device_manager.get_entry_reference(network_device_uuid)->set_parent_uuid(
                system_persistent_uuid);
            stabilize_network_device(network_device_uuid);
        }

        for (const auto& trusted_module_uuid: trusted_module_uuids) {
            trusted_module_manager.get_entry_reference(trusted_module_uuid)->set_parent_uuid(system_persistent_uuid);
            stabilize_trusted_module(trusted_module_uuid);
        }
        return system_persistent_uuid;
    }
    else {
        agent_framework::log_key_value_missing(system.get_component().to_string(), system_uuid);
        return system_uuid;
    }
}


const std::string ComputeTreeStabilizer::stabilize_drive(const std::string& drive_uuid) {
    auto& drive_manager = CommonComponents::get_instance()->get_drive_manager();
    auto& chassis_manager = CommonComponents::get_instance()->get_chassis_manager();

    const auto& drive = drive_manager.get_entry(drive_uuid);
    const auto& parent_chassis = chassis_manager.get_entry(drive.get_parent_uuid());

    if (parent_chassis.has_persistent_uuid() && drive.get_fru_info().get_model_number().has_value()) {
        const auto model = drive.get_fru_info().get_model_number().value();
        const std::string& drive_persistent_uuid = stabilize_single_resource(drive_uuid, drive_manager,
                                                                             literals::DRIVE_KEY_BASE +
                                                                             parent_chassis.get_uuid() + model);
        return drive_persistent_uuid;
    }
    else {
        agent_framework::log_key_value_missing(drive.get_component().to_string(), drive_uuid);
        return drive_uuid;
    }
}


const std::string ComputeTreeStabilizer::stabilize_chassis(const std::string& chassis_uuid) {
    auto& module_manager = CommonComponents::get_instance()->get_module_manager();
    auto& chassis_manager = CommonComponents::get_instance()->get_chassis_manager();
    auto& drive_manager = CommonComponents::get_instance()->get_drive_manager();

    const auto& chassis = chassis_manager.get_entry(chassis_uuid);
    const auto& parent_manager = module_manager.get_entry(chassis.get_parent_uuid());

    if (parent_manager.get_guid().has_value()) {
        const auto manager_guid = parent_manager.get_guid().value();
        const auto& drive_uuids = drive_manager.get_keys(chassis_uuid);

        const std::string& chassis_persistent_uuid = stabilize_single_resource(chassis_uuid, chassis_manager,
                                                                               literals::CHASSIS_KEY_BASE +
                                                                               manager_guid);

        for (const auto& drive_uuid : drive_uuids) {
            drive_manager.get_entry_reference(drive_uuid)->set_parent_uuid(chassis_persistent_uuid);
            stabilize_drive(drive_uuid);
        }

        helpers::update_chassis_in_relations(chassis_uuid, chassis_persistent_uuid);
        return chassis_persistent_uuid;
    }
    else {
        agent_framework::log_key_value_missing(chassis.get_component().to_string(), chassis_uuid);
        return chassis_uuid;
    }
}


const std::string ComputeTreeStabilizer::stabilize(const std::string& module_uuid) {
    auto& module_manager = CommonComponents::get_instance()->get_module_manager();
    auto& system_manager = CommonComponents::get_instance()->get_system_manager();
    auto& chassis_manager = CommonComponents::get_instance()->get_chassis_manager();
    const auto& system_keys = system_manager.get_keys(module_uuid);

    if (system_keys.empty()) {
        throw std::runtime_error("No system found for module " + module_uuid);
    }

    const auto& module = module_manager.get_entry(module_uuid);
    const auto& system = system_manager.get_entry(system_keys.front());
    const auto& chassis = chassis_manager.get_entry(system.get_chassis());

    if (module.get_guid().has_value()) {
        const auto manager_guid = module.get_guid().value();

        const std::string& module_persistent_uuid = stabilize_single_resource(module_uuid, module_manager,
                                                                              literals::MODULE_KEY_BASE + manager_guid);

        system_manager.get_entry_reference(system.get_uuid())->set_parent_uuid(module_persistent_uuid);
        stabilize_system(system.get_uuid());

        chassis_manager.get_entry_reference(chassis.get_uuid())->set_parent_uuid(module_persistent_uuid);
        stabilize_chassis(chassis.get_uuid());

        return module_persistent_uuid;
    }
    else {
        agent_framework::log_key_value_missing(module.get_component().to_string(), module_uuid);
        return module_uuid;
    }
}
