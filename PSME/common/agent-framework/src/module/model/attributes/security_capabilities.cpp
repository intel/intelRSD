/*!
 * @brief Memory: Security Capabilities attribute
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file security_capabilities.cpp
 */

#include "agent-framework/module/model/attributes/security_capabilities.hpp"
#include "agent-framework/module/constants/compute.hpp"

using namespace agent_framework::model::attribute;
using namespace agent_framework::model;

SecurityCapabilities::SecurityCapabilities() { }

SecurityCapabilities::~SecurityCapabilities() { }

json::Json SecurityCapabilities::to_json() const {
    json::Json result = json::Json();
    result[literals::SecurityCapabilities::PASSPHRASE_CAPABLE] = get_passphrase_capable();
    result[literals::SecurityCapabilities::MAX_PASSPHRASE_COUNT] = get_max_passphrase_count();
    result[literals::SecurityCapabilities::SECURITY_STATES] = get_security_states().to_json();
    return result;
}

SecurityCapabilities SecurityCapabilities::from_json(const json::Json& json) {
    attribute::SecurityCapabilities region{};
    region.set_passphrase_capable(json[literals::SecurityCapabilities::PASSPHRASE_CAPABLE]);
    region.set_max_passphrase_count(json[literals::SecurityCapabilities::MAX_PASSPHRASE_COUNT]);
    region.set_security_states(Array<enums::SecurityStates>::from_json(json[literals::SecurityCapabilities::SECURITY_STATES]));
    return region;
}