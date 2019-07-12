/*!
 * @brief Provides class for generating unique keys for resources under compute agent.
 *
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file compute_key_generator.cpp
 * */


#include "tree_stability/compute_key_generator.hpp"

#include "agent-framework/module/managers/utils/manager_utils.hpp"
#include "agent-framework/module/model/model_compute.hpp"
#include "agent-framework/module/model/model_common.hpp"
#include "agent-framework/module/model/model_chassis.hpp"
#include "agent-framework/module/service_uuid.hpp"



using namespace agent_framework::model;
using namespace agent_framework::module;

namespace agent {
namespace compute {

using map_value_type = std::map<std::string, std::string>::value_type;
using agent_framework::KeyValueMissingError;

const std::map<std::string, std::string> ComputeKeyGenerator::m_keys_base_map{
    map_value_type(Manager::get_component().to_string(), "_Manager_"),
    map_value_type(Chassis::get_component().to_string(), "_Chassis_"),
    map_value_type(ThermalZone::get_component().to_string(), "_ThermalZone_"),
    map_value_type(PowerZone::get_component().to_string(), "_PowerZone_"),
    map_value_type(Drive::get_component().to_string(), "_Drive_"),
    map_value_type(System::get_component().to_string(), "_System_"),
    map_value_type(Processor::get_component().to_string(), "_Processor_"),
    map_value_type(Memory::get_component().to_string(), "_Memory_"),
    map_value_type(MemoryDomain::get_component().to_string(), "_MemoryDomain_"),
    map_value_type(MemoryChunks::get_component().to_string(), "_MemoryChunks_"),
    map_value_type(StorageSubsystem::get_component().to_string(), "_StorageSubsystem_"),
    map_value_type(StorageController::get_component().to_string(), "_StorageController_"),
    map_value_type(NetworkInterface::get_component().to_string(), "_NetworkInterface_"),
    map_value_type(NetworkDevice::get_component().to_string(), "_NetworkDevice_"),
    map_value_type(NetworkDeviceFunction::get_component().to_string(), "_NetworkDeviceFunction_"),
    map_value_type(TrustedModule::get_component().to_string(), "_TrustedPlatformModule_"),
    map_value_type(PcieDevice::get_component().to_string(), "_PcieDevice_"),
    map_value_type(PcieFunction::get_component().to_string(), "_PcieFunction_"),
    map_value_type(LogService::get_component().to_string(), "_LogService_"),
    map_value_type(LogEntry::get_component().to_string(), "_LogEntry_")
};

std::string ComputeKeyGenerator::m_agent_id{};


template<>
const std::string ComputeKeyGenerator::generate_key(const Manager& manager) {
    const auto manager_guid = manager.get_guid();

    if (!manager_guid.has_value()) {
        throw KeyValueMissingError("Manager GUID is missing.");
    }

    return generate_key_base(manager) + manager_guid.value();
}


template<>
const std::string ComputeKeyGenerator::generate_key(const Chassis& chassis, const Manager& manager) {
    const auto& manager_unique_key = generate_key(manager);

    return generate_key_base(chassis) + manager_unique_key;
}


template<>
const std::string ComputeKeyGenerator::generate_key(const ThermalZone& zone, const Chassis& parent_chassis) {
    return generate_key_base(zone) + parent_chassis.get_uuid();
}


template<>
const std::string ComputeKeyGenerator::generate_key(const PowerZone& zone, const Chassis& parent_chassis) {
    return generate_key_base(zone) + parent_chassis.get_uuid();
}


template<>
const std::string ComputeKeyGenerator::generate_key(const PcieDevice& device, const Chassis& parent_chassis) {
    auto key =  generate_key_base(device) + parent_chassis.get_uuid();
    const auto& fru = device.get_fru_info();

    if (!(fru.get_manufacturer().has_value() && fru.get_model_number().has_value())) {
        throw KeyValueMissingError("PCIeDevice model number or manufacturer is missing.");
    }
    key += fru.get_manufacturer() + fru.get_model_number();
    return key;
}


template<>
const std::string ComputeKeyGenerator::generate_key(const PcieFunction& function) {
    return generate_key_base(function) + function.get_parent_uuid();
}


template<>
const std::string ComputeKeyGenerator::generate_key(const Drive& drive) {
    const auto serial_number = drive.get_fru_info().get_serial_number();

    if (!serial_number.has_value()) {
        throw KeyValueMissingError("Serial number is missing.");
    }

    return generate_key_base(drive) + serial_number.value();
}


template<>
const std::string ComputeKeyGenerator::generate_key(const System& system) {
    const auto system_guid = system.get_guid();

    if (!system_guid.has_value()) {
        throw KeyValueMissingError("System GUID is missing.");
    }

    return generate_key_base(system) + system_guid;
}


template<>
const std::string ComputeKeyGenerator::generate_key(const Processor& processor, const System& parent_system) {
    const auto socket = processor.get_socket();
    const auto parent_system_key = generate_key(parent_system);

    if (!socket.has_value()) {
        throw KeyValueMissingError("Socket is missing.");
    }

    return generate_key_base(processor) + parent_system_key + socket.value();
}


template<>
const std::string ComputeKeyGenerator::generate_key(const Memory& memory, const System& parent_system) {
    const auto device_locator = memory.get_device_locator();
    const auto parent_system_key = generate_key(parent_system);

    if (!device_locator.has_value()) {
        throw KeyValueMissingError("Device locator is missing.");
    }

    return generate_key_base(memory) + parent_system_key + device_locator.value();
}


template<>
const std::string ComputeKeyGenerator::generate_key(const MemoryDomain& memory_domain, const System& parent_system) {
    // TODO: Key must be extended with some unique property
    const auto parent_system_key = generate_key(parent_system);
    return generate_key_base(memory_domain) + parent_system_key;
}


template<>
const std::string ComputeKeyGenerator::generate_key(const MemoryChunks& memory_chunks) {
    // TODO: Key must be extended with some unique property
    return generate_key_base(memory_chunks) + memory_chunks.get_parent_uuid();
}


template<>
const std::string
ComputeKeyGenerator::generate_key(const StorageSubsystem& storage_subsystem, const System& parent_system) {
    const auto& parent_system_key = generate_key(parent_system);
    return generate_key_base(storage_subsystem) + parent_system_key;
}


template<>
const std::string
ComputeKeyGenerator::generate_key(const StorageController& storage_controller, const System& parent_system) {
    const auto& parent_system_key = generate_key(parent_system);
    return generate_key_base(storage_controller) + parent_system_key;
}


template<>
const std::string ComputeKeyGenerator::generate_key(const NetworkInterface& network_interface) {
    const auto& factory_mac_address = network_interface.get_factory_mac_address();

    if (!factory_mac_address.has_value()) {
        throw KeyValueMissingError("Factory MAC address is missing.");
    }

    return generate_key_base(network_interface) + factory_mac_address.value();
}


template<>
const std::string ComputeKeyGenerator::generate_key(const NetworkDevice& network_device, const System& parent_system) {
    const auto& parent_system_key = generate_key(parent_system);
    return generate_key_base(network_device) + parent_system_key;
}


template<>
const std::string
ComputeKeyGenerator::generate_key(const NetworkDeviceFunction& network_device_function, const System& parent_system) {
    const auto& parent_system_key = generate_key(parent_system);
    return generate_key_base(network_device_function) + parent_system_key;
}


template<>
const std::string ComputeKeyGenerator::generate_key(const TrustedModule& trusted_module, const System& parent_system) {
    const auto& interface_type = trusted_module.get_interface_type();
    const auto& parent_system_key = generate_key(parent_system);

    if (!interface_type.has_value()) {
        throw KeyValueMissingError("Interface type is missing.");
    }

    return generate_key_base(trusted_module) + parent_system_key + interface_type.value().to_string();
}


template<>
const std::string ComputeKeyGenerator::generate_key(const Metric& metric, const Resource& resource) {
    const auto& resource_key = resource.get_unique_key();

    return resource_key + metric.get_component().to_string() + metric.get_metric_definition_uuid() + metric.get_name();
}


template<>
const std::string ComputeKeyGenerator::generate_key(const MetricDefinition& metric_def) {
    std::string key = metric_def.get_component().to_string() + metric_def.get_metric_jsonptr();
    if (metric_def.get_name().has_value()) {
        key += metric_def.get_name();
    }
    return key;
}


template<>
const std::string
ComputeKeyGenerator::generate_key(const LogService& log_service) {
    return generate_key_base(log_service) + log_service.get_parent_uuid();
}


template<>
const std::string
ComputeKeyGenerator::generate_key(const LogEntry& log_entry) {
    return generate_key_base(log_entry) + log_entry.get_parent_uuid() + std::to_string(log_entry.get_record_id());
}

}
}
