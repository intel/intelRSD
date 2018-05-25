/*!
 * @brief Provides class for stabilizing storage agent resources
 *
 * @copyright
 * Copyright (c) 2017-2018 Intel Corporation
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
 * @file storage_key_generator.cpp
 * */

#include "tree_stability/storage_key_generator.hpp"
#include "agent-framework/module/storage_components.hpp"



using namespace agent_framework::model;

namespace agent {
namespace storage {

const StorageKeyGenerator::KeysBaseMap StorageKeyGenerator::m_keys_base_map{
    {Manager::get_component(),          "_RSS_Manager_"},
    {Chassis::get_component(),          "_RSS_Chassis_"},
    {System::get_component(),           "_RSS_System_"},
    {NetworkInterface::get_component(), "_RSS_Interface_"},
    {Fabric::get_component(),           "_RSS_Fabric_"},
    {StorageService::get_component(),   "_RSS_StorageService_"},
    {Drive::get_component(),            "_RSS_HardDrive_"},
    {Volume::get_component(),           "_RSS_Volume_"},
    {StoragePool::get_component(),      "_RSS_StoragePool_"}
};


template<>
const StorageKeyGenerator::KeyType StorageKeyGenerator::generate_key(const Manager& manager) {
    return generate_key_base(manager);
}


template<>
const StorageKeyGenerator::KeyType StorageKeyGenerator::generate_key(const Chassis& chassis,
                                                                     const Manager& parent_manager) {
    return generate_key_base(chassis) + parent_manager.get_uuid();
}


template<>
const StorageKeyGenerator::KeyType StorageKeyGenerator::generate_key(const System& system,
                                                                     const Manager& parent_manager) {
    return generate_key_base(system) + parent_manager.get_uuid() + system.get_guid().value_or("");
}


template<>
const StorageKeyGenerator::KeyType StorageKeyGenerator::generate_key(const NetworkInterface& interface,
                                                                     const System& parent_system) {
    return generate_key_base(interface) + parent_system.get_uuid() + interface.get_mac_address().value_or("");
}


template<>
const StorageKeyGenerator::KeyType StorageKeyGenerator::generate_key(const StorageService& storage_service,
                                                                     const Manager& parent_manager) {
    return generate_key_base(storage_service) + parent_manager.get_uuid();
}


template<>
const StorageKeyGenerator::KeyType StorageKeyGenerator::generate_key(const Drive& drive) {
    const auto serial_number = drive.get_fru_info().get_serial_number();
    if (!serial_number.has_value()) {
        throw agent_framework::KeyValueMissingError("Drive's serial number is missing.");
    }
    return generate_key_base(drive) + serial_number.value();
}


template<>
const StorageKeyGenerator::KeyType StorageKeyGenerator::generate_key(const StoragePool& pool) {
    try {
        return generate_key_base(pool) + attribute::Identifier::get_system_path(pool);
    }
    catch (const std::logic_error&) {
        throw agent_framework::KeyValueMissingError("Storage Pool's device path is missing.");
    }
}


template<>
const StorageKeyGenerator::KeyType StorageKeyGenerator::generate_key(const Volume& volume) {
    try {
        return generate_key_base(volume) + attribute::Identifier::get_system_path(volume);
    }
    catch (const std::logic_error&) {
        throw agent_framework::KeyValueMissingError("Volume's device path is missing.");
    }
}


template<>
const StorageKeyGenerator::KeyType StorageKeyGenerator::generate_key(const Fabric& fabric,
                                                                     const Manager& parent_manager) {
    return generate_key_base(fabric) + parent_manager.get_uuid();
}

}
}
