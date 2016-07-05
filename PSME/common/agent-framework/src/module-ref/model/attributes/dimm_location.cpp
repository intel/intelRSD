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

#include "agent-framework/module-ref/model/attributes/dimm_location.hpp"
#include "agent-framework/module-ref/constants/compute.hpp"

using namespace agent_framework::model::attribute;
using namespace agent_framework::model;

DimmLocation::DimmLocation() { }

DimmLocation::~DimmLocation() { }

Json::Value DimmLocation::to_json() const {
    Json::Value result{};
    result[literals::DimmLocation::SOCKET] = get_socket();
    result[literals::DimmLocation::CONTROLLER] = get_controller();
    result[literals::DimmLocation::CHANNEL] = get_channel();
    result[literals::DimmLocation::SLOT] = get_slot();
    return result;
}

DimmLocation DimmLocation::from_json(const Json::Value& json) {
    attribute::DimmLocation location{};
    if (!json) {
        return location;
    }
    location.set_socket(json[literals::DimmLocation::SOCKET]);
    location.set_controller(json[literals::DimmLocation::CONTROLLER]);
    location.set_channel(json[literals::DimmLocation::CHANNEL]);
    location.set_slot(json[literals::DimmLocation::SLOT]);
    return location;
}
