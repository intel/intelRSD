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

#include "agent-framework/module/model/attributes/memory_location.hpp"
#include "agent-framework/module/constants/compute.hpp"

using namespace agent_framework::model::attribute;
using namespace agent_framework::model;

MemoryLocation::MemoryLocation() { }

MemoryLocation::~MemoryLocation() { }

json::Json MemoryLocation::to_json() const {
    json::Json result = json::Json();
    result[literals::MemoryLocation::SOCKET] = get_socket();
    result[literals::MemoryLocation::CONTROLLER] = get_controller();
    result[literals::MemoryLocation::CHANNEL] = get_channel();
    result[literals::MemoryLocation::SLOT] = get_slot();
    return result;
}

MemoryLocation MemoryLocation::from_json(const json::Json& json) {
    attribute::MemoryLocation location{};
    location.set_socket(json[literals::MemoryLocation::SOCKET]);
    location.set_controller(json[literals::MemoryLocation::CONTROLLER]);
    location.set_channel(json[literals::MemoryLocation::CHANNEL]);
    location.set_slot(json[literals::MemoryLocation::SLOT]);
    return location;
}
