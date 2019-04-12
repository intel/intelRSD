/*!
 * @brief Provides class for stabilizing agent resources
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file key_generator.cpp
 */

#include "agent-framework/module/model/system.hpp"
#include "agent-framework/module/model/volume.hpp"
#include "agent-framework/module/model/drive.hpp"
#include "agent-framework/tree_stability/key_generator.hpp"



namespace agent_framework {

KeyGenerator::KeyType KeyGenerator::generate_key(const model::Manager& manager) {
    return generate_key_base(manager.get_component());
}


KeyGenerator::KeyType KeyGenerator::generate_key(const model::Chassis& chassis) {
    return generate_key_base(chassis.get_component()) + chassis.get_parent_uuid();
}


KeyGenerator::KeyType KeyGenerator::generate_key(const model::Fabric& fabric) {
    return generate_key_base(fabric.get_component()) + fabric.get_parent_uuid();
}


KeyGenerator::KeyType KeyGenerator::generate_key(const model::StorageService& storage_service) {
    return generate_key_base(storage_service.get_component()) + storage_service.get_parent_uuid();
}


KeyGenerator::KeyType KeyGenerator::generate_key(const model::System& system) {
    if (!system.get_guid().has_value()) {
        throw KeyValueMissingError("System's GUID is missing.");
    }
    return generate_key_base(system.get_component()) + system.get_guid().value();
}


KeyGenerator::KeyType KeyGenerator::generate_key(const model::Volume& volume) {
    if (volume.get_name().has_value()) {
        return generate_key_base(volume.get_component()) + volume.get_name().value();
    }
    else {
        throw KeyValueMissingError("Volume's unique name is missing.");
    }
}


KeyGenerator::KeyType KeyGenerator::generate_key(const model::StoragePool& pool) {
    if (pool.get_name().has_value()) {
        return generate_key_base(pool.get_component()) + pool.get_name().value();
    }
    else {
        throw KeyValueMissingError("Storage Pool's unique name is missing.");
    }
}


KeyGenerator::KeyType KeyGenerator::generate_key(const model::Drive& drive) {
    const auto serial_number = drive.get_fru_info().get_serial_number();
    if (!serial_number.has_value()) {
        throw KeyValueMissingError("Drive's serial number is missing.");
    }
    return generate_key_base(drive.get_component()) + serial_number.value();
}


KeyGenerator::KeyType KeyGenerator::generate_key(const model::NetworkInterface& interface) {
    const auto mac = interface.get_mac_address();
    if (!mac.has_value()) {
        throw KeyValueMissingError("Ethernet interface's MAC address is missing.");
    }

    return generate_key_base(interface.get_component()) + mac.value();
}


KeyGenerator::KeyType KeyGenerator::generate_key(const model::Endpoint& endpoint) {
    std::stringstream ss{};
    ss << generate_key_base(endpoint.get_component());

    if (endpoint.get_identifiers().empty()) {
        throw KeyValueMissingError("Missing identifiers for endpoint!.");
    }

    for (const auto& identifier : endpoint.get_identifiers()) {
        ss << identifier.get_durable_name();
    }

    return ss.str();
}

}
