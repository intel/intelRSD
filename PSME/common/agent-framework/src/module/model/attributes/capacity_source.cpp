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
 * */

#include "agent-framework/module/constants/common.hpp"
#include "agent-framework/module/model/attributes/capacity_source.hpp"

using namespace agent_framework::model::attribute;
using namespace agent_framework::model;

CapacitySource::~CapacitySource() { }

json::Json CapacitySource::to_json() const {
    json::Json result = json::Json();
    result[literals::CapacitySource::PROVIDING_DRIVES] = get_providing_drives().to_json();
    result[literals::CapacitySource::PROVIDING_POOLS] = get_providing_pools().to_json();
    result[literals::CapacitySource::PROVIDING_VOLUMES] = get_providing_volumes().to_json();
    result[literals::Capacity::ALLOCATED_BYTES] = get_allocated_bytes();
    result[literals::Capacity::CONSUMED_BYTES] = get_consumed_bytes();
    result[literals::Capacity::GUARANTEED_BYTES] = get_guaranteed_bytes();
    result[literals::Capacity::PROVISIONED_BYTES] = get_provisioned_bytes();
    return result;
}

CapacitySource CapacitySource::from_json(const json::Json& json) {
    attribute::CapacitySource cs{};
    cs.set_providing_drives(Array<Uuid>::from_json(json[literals::CapacitySource::PROVIDING_DRIVES]));
    cs.set_providing_pools(Array<Uuid>::from_json(json[literals::CapacitySource::PROVIDING_POOLS]));
    cs.set_providing_volumes(Array<Uuid>::from_json(json[literals::CapacitySource::PROVIDING_VOLUMES]));
    cs.set_allocated_bytes(json[literals::Capacity::ALLOCATED_BYTES]);
    cs.set_consumed_bytes(json[literals::Capacity::CONSUMED_BYTES]);
    cs.set_guaranteed_bytes(json[literals::Capacity::GUARANTEED_BYTES]);
    cs.set_provisioned_bytes(json[literals::Capacity::PROVISIONED_BYTES]);
    return cs;
}
