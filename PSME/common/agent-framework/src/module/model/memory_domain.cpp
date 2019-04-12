/*!
 * @brief Memory Domain model interface
 *
 * @copyright Copyright (c) 2019 Intel Corporation.
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
 * @file memory_domain.cpp
 */

#include "agent-framework/module/model/memory_domain.hpp"
#include "agent-framework/module/constants/compute.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component MemoryDomain::component = enums::Component::MemoryDomain;
const enums::CollectionName MemoryDomain::collection_name = enums::CollectionName::MemoryDomains;

MemoryDomain::MemoryDomain(const Uuid& parent_uuid, enums::Component parent_type) :
                           Resource{parent_uuid, parent_type} {}

MemoryDomain::~MemoryDomain() {}

json::Json MemoryDomain::to_json() const {
    json::Json result = json::Json();
    result[literals::MemoryDomain::STATUS] = get_status().to_json();
    result[literals::MemoryDomain::NAME] = get_name();
    result[literals::MemoryDomain::DESCRIPTION] = get_description();
    result[literals::MemoryDomain::ALLOWS_MEMORY_CHUNK_CREATION] = get_allows_memory_chunk_creation();
    result[literals::MemoryDomain::ALLOWS_BLOCK_PROVISIONING] = get_allows_block_provisioning();
    result[literals::MemoryDomain::ALLOWS_MIRRORING] = get_allows_mirroring();
    result[literals::MemoryDomain::ALLOWS_SPARING] = get_allows_sparing();
    result[literals::MemoryDomain::INTERLEAVABLE_MEMORY_SETS] = get_interleavable_memory_sets().to_json();
    result[literals::MemoryDomain::COLLECTIONS] = get_collections().to_json();
    result[literals::MemoryDomain::OEM] = get_oem().to_json();
    return result;
}

MemoryDomain MemoryDomain::from_json(const json::Json& json) {
    using namespace agent_framework::model::attribute;
    MemoryDomain memory_domain{};
    memory_domain.set_status(attribute::Status::from_json(json[literals::MemoryDomain::STATUS]));
    memory_domain.set_name(json[literals::MemoryDomain::NAME]);
    memory_domain.set_description(json[literals::MemoryDomain::DESCRIPTION]);
    memory_domain.set_allows_memory_chunk_creation(json[literals::MemoryDomain::ALLOWS_MEMORY_CHUNK_CREATION]);
    memory_domain.set_allows_block_provisioning(json[literals::MemoryDomain::ALLOWS_BLOCK_PROVISIONING]);
    memory_domain.set_allows_mirroring(json[literals::MemoryDomain::ALLOWS_MIRRORING]);
    memory_domain.set_allows_sparing(json[literals::MemoryDomain::ALLOWS_SPARING]);
    memory_domain.set_interleavable_memory_sets(attribute::Array<attribute::MemorySet>::from_json(json[literals::MemoryDomain::INTERLEAVABLE_MEMORY_SETS]));
    memory_domain.set_collections(Collections::from_json(json[literals::MemoryDomain::COLLECTIONS]));
    memory_domain.set_oem(Oem::from_json(json[literals::MemoryDomain::OEM]));
    return memory_domain;
}
