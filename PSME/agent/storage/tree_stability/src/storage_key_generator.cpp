/*!
 * @brief Provides class for stabilizing storage agent resources
 *
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
    {Drive::get_component(),            "_RSS_Drive_"},
    {Volume::get_component(),           "_RSS_Volume_"},
    {StoragePool::get_component(),      "_RSS_StoragePool_"},
    {Endpoint::get_component(),         "_RSS_Endpoint_"}
};

StorageKeyGenerator::KeyType StorageKeyGenerator::generate_key_base(const enums::Component& component) {
    return m_keys_base_map.at(component);
}

}
}
