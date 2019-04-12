/*!
 * @brief Interleave Set attribute for Memory Chunks model
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
 * @file interleave_set.cpp
 */
#include "agent-framework/module/model/attributes/interleave_set.hpp"
#include "agent-framework/module/constants/compute.hpp"

using namespace agent_framework::model::attribute;
using namespace agent_framework::model;

InterleaveSet::InterleaveSet() { }

InterleaveSet::~InterleaveSet() { }

json::Json InterleaveSet::to_json() const {
    json::Json result = json::Json();
    result[literals::InterleaveSet::MEMORY] = get_memory();
    result[literals::InterleaveSet::REGION_ID] = get_region_id();
    result[literals::InterleaveSet::OFFSET_MIB] = get_offset_mib();
    result[literals::InterleaveSet::SIZE_MIB] = get_size_mib();
    result[literals::InterleaveSet::MEMORY_LEVEL] = get_memory_level();
    return result;
}

InterleaveSet InterleaveSet::from_json(const json::Json& json) {
    attribute::InterleaveSet interleave_set{};
    interleave_set.set_memory(json[literals::InterleaveSet::MEMORY]);
    interleave_set.set_region_id(json[literals::InterleaveSet::REGION_ID]);
    interleave_set.set_offset_mib(json[literals::InterleaveSet::OFFSET_MIB]);
    interleave_set.set_size_mib(json[literals::InterleaveSet::SIZE_MIB]);
    interleave_set.set_memory_level(json[literals::InterleaveSet::MEMORY_LEVEL]);
    return interleave_set;
}