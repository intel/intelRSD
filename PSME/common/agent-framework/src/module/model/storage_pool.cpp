/*!
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
 *
 * @file storage_pool.cpp
 * @brief Storage pool model implementation
 * */

#include "agent-framework/module/model/storage_pool.hpp"
#include "agent-framework/module/constants/storage.hpp"



using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component StoragePool::component = enums::Component::StoragePool;
const enums::CollectionName StoragePool::collection_name = enums::CollectionName::StoragePools;

StoragePool::StoragePool(const std::string& parent_uuid, enums::Component parent_type) :
    Resource(parent_uuid, parent_type) {}


StoragePool::~StoragePool() { }


json::Json StoragePool::to_json() const {
    json::Json result = json::Json();
    result[literals::StoragePool::STATUS] = get_status().to_json();
    result[literals::StoragePool::NAME] = get_name();
    result[literals::StoragePool::DESCRIPTION] = get_description();
    result[literals::StoragePool::COLLECTIONS] = get_collections().to_json();
    result[literals::StoragePool::BLOCK_SIZE_BYTES] = get_block_size_bytes();
    result[literals::StoragePool::IDENTIFIERS] = get_identifiers().to_json();
    result[literals::StoragePool::CAPACITY] = get_capacity().to_json();
    result[literals::StoragePool::CAPACITY_SOURCES] = get_capacity_sources().to_json();
    result[literals::StoragePool::OEM] = get_oem().to_json();
    return result;
}


StoragePool StoragePool::from_json(const json::Json& json) {
    StoragePool pool{};

    pool.set_status(attribute::Status::from_json(json[literals::StoragePool::STATUS]));
    pool.set_name(json[literals::StoragePool::NAME]);
    pool.set_description(json[literals::StoragePool::DESCRIPTION]);
    pool.set_collections(Collections::from_json(json[literals::StoragePool::COLLECTIONS]));
    pool.set_block_size_bytes(json[literals::StoragePool::BLOCK_SIZE_BYTES]);
    pool.set_capacity(attribute::Capacity::from_json(json[literals::StoragePool::CAPACITY]));
    pool.set_capacity_sources(CapacitySources::from_json(json[literals::StoragePool::CAPACITY_SOURCES]));
    pool.set_identifiers(Identifiers::from_json(json[literals::StoragePool::IDENTIFIERS]));
    pool.set_oem(attribute::Oem::from_json(json[literals::StoragePool::OEM]));

    return pool;
}

bool StoragePool::can_allocate_bytes(const agent_framework::model::StoragePool& pool,
                                     std::int64_t requested_capacity_bytes) {

    auto free_bytes = pool.get_capacity().get_guaranteed_bytes().value_or(0);
    return requested_capacity_bytes <= free_bytes;
}

