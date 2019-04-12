/*!
 * @brief Implementation of AddStoragePool class.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file add_storage_pool.cpp
 */

#include "agent-framework/module/requests/storage/add_storage_pool.hpp"



using namespace agent_framework::model;
using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;


AddStoragePool::AddStoragePool(
    uint64_t block_size_bytes,
    const attribute::Array<attribute::CapacitySource>& capacity_sources,
    const attribute::Oem& oem) :
    m_block_size_bytes(block_size_bytes),
    m_capacity_sources(capacity_sources),
    m_oem(oem) {}


json::Json AddStoragePool::to_json() const {
    json::Json json{};
    json[literals::StoragePool::BLOCK_SIZE_BYTES] = get_block_size_bytes();
    json[literals::StoragePool::CAPACITY_SOURCES] = get_capacity_sources().to_json();
    json[literals::StoragePool::OEM] = get_oem().to_json();
    return json;
}


AddStoragePool AddStoragePool::from_json(const json::Json& json) {
    return AddStoragePool{
        json[literals::StoragePool::BLOCK_SIZE_BYTES],
        attribute::Array<attribute::CapacitySource>::from_json(json[literals::StoragePool::CAPACITY_SOURCES]),
        attribute::Oem::from_json(json[literals::StoragePool::OEM])
    };
}
