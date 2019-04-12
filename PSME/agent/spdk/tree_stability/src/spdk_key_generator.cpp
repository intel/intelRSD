/*!
 * @brief Provides class for stabilizing spdk agent resources
 *
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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
 * @file spdk_key_generator.cpp
 * */



#include "agent-framework/module/managers/utils/manager_utils.hpp"
#include "agent-framework/module/model/model_storage.hpp"
#include "agent-framework/module/model/fabric.hpp"
#include "agent-framework/module/model/system.hpp"
#include "agent-framework/module/model/chassis.hpp"
#include "spdk_key_generator.hpp"
#include "agent-framework/module/model/manager.hpp"
#include "agent-framework/tree_stability/stabilizer.hpp"



using namespace agent_framework::model;
using namespace agent_framework::module;

namespace agent {
namespace spdk {

const SpdkKeyGenerator::KeysBaseMap SpdkKeyGenerator::m_keys_base_map{
    {Manager::get_component(),          "_SPDK_Manager_"},
    {Chassis::get_component(),          "_SPDK_Chassis"},
    {Fabric::get_component(),           "_SPDK_Fabric"},
    {StorageService::get_component(),   "_SPDK_StorageService"},
    {Drive::get_component(),            "_SPDK_Drive"},
    {Volume::get_component(),           "_SPDK_Volume"},
    {StoragePool::get_component(),      "_SPDK_StoragePool"},
    {System::get_component(),           "_SPDK_System"},
    {NetworkInterface::get_component(), "_SPDK_Interface"},
    {Zone::get_component(),             "_SPDK_Zone"},
    {Endpoint::get_component(),         "_SPDK_Endpoint"}
};


SpdkKeyGenerator::KeyType SpdkKeyGenerator::generate_key_base(const enums::Component& component) {
    return m_keys_base_map.at(component);
}


SpdkKeyGenerator::KeyType SpdkKeyGenerator::generate_key(const StoragePool& storage_pool) {
    try {
        return generate_key_base(storage_pool.get_component()) + attribute::Identifier::get_uuid(storage_pool);
    }
    catch (const std::logic_error&) {
        throw agent_framework::KeyValueMissingError("Storage pool's UUID is missing.");
    }
}


SpdkKeyGenerator::KeyType SpdkKeyGenerator::generate_key(const Drive& drive) {
    if (drive.get_name().has_value()) {
        return generate_key_base(drive.get_component()) + drive.get_name();
    }
    else {
        throw agent_framework::KeyValueMissingError("Drives's SPDK name is missing.");
    }
}



}
}