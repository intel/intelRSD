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
#include "agent-framework/module/model/attributes/capacity.hpp"

using namespace agent_framework::model::attribute;
using namespace agent_framework::model;

Capacity::~Capacity() { }

json::Json Capacity::to_json() const {
    json::Json result{};
    result[literals::Capacity::ALLOCATED_BYTES] = get_allocated_bytes();
    result[literals::Capacity::CONSUMED_BYTES] = get_consumed_bytes();
    result[literals::Capacity::GUARANTEED_BYTES] = get_guaranteed_bytes();
    result[literals::Capacity::PROVISIONED_BYTES] = get_provisioned_bytes();
    result[literals::Capacity::IS_THIN_PROVISIONED] = is_thin_provisioned();
    return result;
}

Capacity Capacity::from_json(const json::Json& json) {
    attribute::Capacity data{};
    data.set_allocated_bytes(json[literals::Capacity::ALLOCATED_BYTES]);
    data.set_consumed_bytes(json[literals::Capacity::CONSUMED_BYTES]);
    data.set_guaranteed_bytes(json[literals::Capacity::GUARANTEED_BYTES]);
    data.set_provisioned_bytes(json[literals::Capacity::PROVISIONED_BYTES]);
    data.set_thin_provisioned(json[literals::Capacity::IS_THIN_PROVISIONED]);
    return data;
}
