/*!
 * @brief Provides class for stabilizing compute agent resource tree
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
 * @file pnc_key_generator.cpp
 * */

#include "tree_stability/pnc_key_generator.hpp"
#include "agent-framework/module/managers/utils/manager_utils.hpp"
#include "agent-framework/module/service_uuid.hpp"
#include "agent-framework/module/model/model_pnc.hpp"
#include "agent-framework/module/model/model_common.hpp"



using namespace agent_framework::model;
using namespace agent_framework::module;

namespace agent {
namespace pnc {

using map_value_type = std::map<std::string, std::string>::value_type;
using agent_framework::KeyValueMissingError;

const std::map<std::string, std::string> PncKeyGenerator::m_keys_base_map{
    map_value_type(Manager::get_component().to_string(), "_PCIeModule_"),
    map_value_type(PcieDevice::get_component().to_string(), "_PCIeDevice_"),
    map_value_type(PcieFunction::get_component().to_string(), "_PCIeFunction_"),
    map_value_type(Chassis::get_component().to_string(), "_PCIeChassis_"),
    map_value_type(Drive::get_component().to_string(), "_PCIeDrive_"),
    map_value_type(Fabric::get_component().to_string(), "_PCIeFabric_"),
    map_value_type(Switch::get_component().to_string(), "_PCIeSwitch_"),
    map_value_type(Port::get_component().to_string(), "_PCIePort_"),
    map_value_type(Zone::get_component().to_string(), "_PCIeZone_"),
    map_value_type(Endpoint::get_component().to_string(), "_PCIeEndpoint_"),
    map_value_type(System::get_component().to_string(), "_PCIeSystem_"),
    map_value_type(StorageSubsystem::get_component().to_string(), "PCIeStorageSubsystem_"),
    map_value_type(Processor::get_component().to_string(), "_PCIeProcessor_")
};

std::string PncKeyGenerator::m_agent_id{};


template<>
const std::string PncKeyGenerator::generate_key(const Manager& manager, const Switch& pcie_switch) {
    const auto pcie_switch_serial_number = pcie_switch.get_fru_info().get_serial_number();

    if (!pcie_switch_serial_number.has_value()) {
        throw KeyValueMissingError("Switch serial number is missing.");
    }

    return generate_key_base(manager) + pcie_switch_serial_number.value();
}


template<>
const std::string PncKeyGenerator::generate_key(const PcieDevice& pcie_device) {
    const auto pcie_device_serial_number = pcie_device.get_fru_info().get_serial_number();

    if (!pcie_device_serial_number.has_value()) {
        throw KeyValueMissingError("Device serial number is missing.");
    }

    return generate_key_base(pcie_device) + pcie_device_serial_number.value();
}


template<>
const std::string PncKeyGenerator::generate_key(const PcieDevice& pcie_device, const std::string& device_unique_key) {

    return generate_key_base(pcie_device) + device_unique_key;
}


template<>
const std::string PncKeyGenerator::generate_key(const PcieFunction& pcie_function, const PcieDevice& pcie_device) {
    auto& parent_device_serial_key = pcie_device.get_fru_info().get_serial_number();

    if (!parent_device_serial_key.has_value()) {
        throw KeyValueMissingError("Parent device serial number is missing.");
    }

    return generate_key_base(pcie_function) + parent_device_serial_key.value();
}


template<>
const std::string
PncKeyGenerator::generate_key(const PcieFunction& pcie_function, const std::string& device_unique_key) {

    return generate_key_base(pcie_function) + device_unique_key;
}


template<>
const std::string PncKeyGenerator::generate_key(const Chassis& chassis, const Switch& pcie_switch) {
    const auto pcie_switch_serial_number = pcie_switch.get_fru_info().get_serial_number();

    if (!pcie_switch_serial_number.has_value()) {
        throw KeyValueMissingError("No serial number found.");
    }

    return generate_key_base(chassis) + pcie_switch_serial_number.value();
}


template<>
const std::string PncKeyGenerator::generate_key(const Drive& drive) {
    const auto drive_serial_number = drive.get_fru_info().get_serial_number();

    if (!drive_serial_number.has_value()) {
        throw KeyValueMissingError("Serial number is missing.");
    }

    return generate_key_base(drive) + drive_serial_number.value();
}


template<>
const std::string PncKeyGenerator::generate_key(const Fabric& fabric) {
    return generate_key_base(fabric);
}


template<>
const std::string PncKeyGenerator::generate_key(const Switch& pcie_switch) {
    const auto pcie_switch_serial_number =
        pcie_switch.get_fru_info().get_serial_number();

    if (!pcie_switch_serial_number.has_value()) {
        throw KeyValueMissingError("Serial number is missing.");
    }

    return generate_key_base(pcie_switch) + pcie_switch_serial_number.value();
}


template<>
const std::string PncKeyGenerator::generate_key(const Port& port, const Switch& pcie_switch) {
    const auto parent_switch_serial_number = pcie_switch.get_fru_info().get_serial_number();

    if (!parent_switch_serial_number.has_value()) {
        throw KeyValueMissingError("Switch serial number is missing.");
    }

    if (!port.get_port_id().has_value()) {
        throw KeyValueMissingError("Port ID is missing.");
    }

    return generate_key_base(port) + port.get_port_id().value();
}


template<>
const std::string PncKeyGenerator::generate_key(const Zone& zone, const Switch& pcie_switch) {
    const auto switch_serial_number = pcie_switch.get_fru_info().get_serial_number();

    if (!switch_serial_number.has_value()) {
        throw KeyValueMissingError("Switch serial number is missing.");
    }

    if (!zone.get_zone_id().has_value()) {
        throw KeyValueMissingError("Zone ID is missing.");
    }

    return generate_key_base(zone) + std::to_string(static_cast<unsigned int>(zone.get_zone_id().value()));
}


template<>
const std::string
PncKeyGenerator::generate_key(const Endpoint& endpoint, const std::vector<Port>& in_endpoint_ports) {
    auto connected_entities = endpoint.get_connected_entities();
    std::vector<Port> endpoint_ports = in_endpoint_ports;

    if (connected_entities.empty()) {
        if (endpoint_ports.empty()) {
            throw KeyValueMissingError("No connected entities and unique key ports.");
        }
        throw KeyValueMissingError("No connected entities.");
    }

    using ConnectedEntity = agent_framework::model::attribute::ConnectedEntity;
    auto entity_lexicographical_compare = [](ConnectedEntity lhs, ConnectedEntity rhs) -> bool {
        if (!lhs.get_entity().has_value() || !rhs.get_entity().has_value()) {
            return lhs.get_entity() < rhs.get_entity();
        }
        else {
            return lhs.get_entity_role().value().to_string() < rhs.get_entity_role().value().to_string();
        }
    };

    auto port_lexicographical_compare = [](const Port& lhs, const Port& rhs) -> bool {
        return lhs.get_unique_key() < rhs.get_unique_key();
    };

    std::sort(endpoint_ports.begin(), endpoint_ports.end(), port_lexicographical_compare);
    std::sort(connected_entities.begin(), connected_entities.end(), entity_lexicographical_compare);

    std::vector<std::string> connected_entities_data{};
    constexpr std::size_t data_multiplier = 2u;
    constexpr std::size_t uuid_string_length = 36u;
    connected_entities_data.reserve(data_multiplier * connected_entities.size());
    std::size_t unique_key_size = 0u;

    for (const auto& connected_entity : connected_entities) {
        if (connected_entity.get_entity().has_value()) {
            connected_entities_data.push_back(connected_entity.get_entity().value());
            unique_key_size += uuid_string_length;
        }
        if (connected_entity.get_entity_role().has_value()) {
            connected_entities_data.push_back(connected_entity.get_entity_role().value().to_string());
            unique_key_size += connected_entities_data.back().size();
        }
    }

    for (const auto& endpoint_port : endpoint_ports) {
        unique_key_size += endpoint_port.get_unique_key().has_value() ? endpoint_port.get_unique_key().value().size()
                                                                      : 0u;
    }

    std::string endpoint_unique_key{generate_key_base(endpoint)};
    endpoint_unique_key.reserve(endpoint_unique_key.size() + unique_key_size);

    for (const auto& connected_entity_data_chunk : connected_entities_data) {
        endpoint_unique_key += connected_entity_data_chunk;
    }

    for (const auto& endpoint_port : endpoint_ports) {
        endpoint_unique_key += endpoint_port.get_unique_key().has_value() ? endpoint_port.get_unique_key().value()
                                                                          : std::string();
    }

    return endpoint_unique_key;
}


template<>
const std::string PncKeyGenerator::generate_key(const System& system) {
    return generate_key_base(system);
}


template<>
const std::string PncKeyGenerator::generate_key(const StorageSubsystem& storage_subsystem) {
    return generate_key_base(storage_subsystem);
}


template<>
const std::string PncKeyGenerator::generate_key(const MetricDefinition& metric_def) {
    std::string key = metric_def.get_component().to_string() + metric_def.get_metric_jsonptr();
    if (metric_def.get_name().has_value()) {
        key += metric_def.get_name();
    }
    return key;
}


template<>
const std::string PncKeyGenerator::generate_key(const Processor& processor) {

    if (!processor.get_fru_info().get_serial_number().has_value()) {
        throw KeyValueMissingError("Processor serial number is missing.");
    }

    return generate_key_base(processor) + processor.get_fru_info().get_serial_number();
}

}
}
