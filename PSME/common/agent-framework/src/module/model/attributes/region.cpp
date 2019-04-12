/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * @section DESCRIPTION
 *
 * */

#include "agent-framework/module/model/attributes/region.hpp"
#include "agent-framework/module/constants/compute.hpp"

using namespace agent_framework::model::attribute;
using namespace agent_framework::model;

Region::Region() { }

Region::~Region() { }

json::Json Region::to_json() const {
    json::Json result = json::Json();
    result[literals::Region::REGION_ID] = get_region_id();
    result[literals::Region::MEMORY_TYPE] = get_memory_type();
    result[literals::Region::OFFSET_MIB] = get_offset_mib();
    result[literals::Region::SIZE_MIB] = get_size_mib();
    return result;
}

Region Region::from_json(const json::Json& json) {
    attribute::Region region{};
    region.set_region_id(json[literals::Region::REGION_ID]);
    region.set_memory_type(json[literals::Region::MEMORY_TYPE]);
    region.set_offset_mib(json[literals::Region::OFFSET_MIB]);
    region.set_size_mib(json[literals::Region::SIZE_MIB]);
    return region;
}
