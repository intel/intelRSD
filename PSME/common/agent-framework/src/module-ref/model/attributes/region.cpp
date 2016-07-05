/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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

#include "agent-framework/module-ref/model/attributes/region.hpp"
#include "agent-framework/module-ref/constants/compute.hpp"

using namespace agent_framework::model::attribute;
using namespace agent_framework::model;

Region::Region() { }

Region::~Region() { }

Json::Value Region::to_json() const {
    Json::Value result{};
    result[literals::Region::REGION_ID] = get_region_id();
    result[literals::Region::MEMORY_TYPE] = get_memory_type();
    result[literals::Region::OFFSET_MB] = get_offset_mb();
    result[literals::Region::SIZE_MB] = get_size_mb();
    return result;
}

Region Region::from_json(const Json::Value& json) {
    attribute::Region region{};
    if (!json) {
        return region;
    }
    region.set_region_id(json[literals::Region::REGION_ID]);
    region.set_memory_type(json[literals::Region::MEMORY_TYPE]);
    region.set_offset_mb(json[literals::Region::OFFSET_MB]);
    region.set_size_mb(json[literals::Region::SIZE_MB]);
    return region;
}
