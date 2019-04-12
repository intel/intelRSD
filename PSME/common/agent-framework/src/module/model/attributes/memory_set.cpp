/*!
 * @brief Memory Set attribute for Memory Chunks model
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
 * @file memory_set.cpp
 */
#include "agent-framework/module/model/attributes/memory_set.hpp"
#include "agent-framework/module/constants/compute.hpp"

using namespace agent_framework::model::attribute;
using namespace agent_framework::model;

MemorySet::MemorySet() { }

MemorySet::~MemorySet() { }

json::Json MemorySet::to_json() const {
    json::Json result = json::Json();
    result[literals::MemorySet::MEMORY_SET] = get_memory_set().to_json();
    return result;
}

MemorySet MemorySet::from_json(const json::Json& json) {
    attribute::MemorySet memory_set{};
    memory_set.set_memory_set(Array<Uuid>::from_json(json[literals::MemorySet::MEMORY_SET]));
    return memory_set;
}