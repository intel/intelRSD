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
 *
 * @file module/requests/psme/attach.cpp
 *
 * @brief PSME attach request
 * */

#include "agent-framework/module/requests/psme/attach.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::requests;
using namespace agent_framework::model;

Attach::Attach() {}

json::Json Attach::to_json() const {
    json::Json value = json::Json();
    value[literals::Attach::VERSION] = get_version();
    value[literals::Attach::IPV4_ADDRESS] = get_ipv4_address();
    value[literals::Attach::PORT] = get_port();
    value[literals::Attach::VENDOR] = get_vendor();
    value[literals::Attach::GAMI_ID] = get_gami_id();
    value[literals::Attach::CAPABILITIES] = get_capabilities().to_json();
    return value;
}

Attach Attach::from_json(const json::Json& value) {
    Attach attach{};
    attach.m_version = value[literals::Attach::VERSION];
    attach.m_ipv4_address = value[literals::Attach::IPV4_ADDRESS];
    attach.m_port = value[literals::Attach::PORT];
    attach.m_vendor = value[literals::Attach::VENDOR];
    attach.m_gami_id = value[literals::Attach::GAMI_ID];
    attach.m_capabilities = Capabilities::from_json(value[literals::Attach::CAPABILITIES]);
    return attach;
}
