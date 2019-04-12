/*!
 * @brief Implementation of AddEndpoint class.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file add_endpoint.cpp
 */

#include "agent-framework/module/responses/common/add_endpoint.hpp"
#include "agent-framework/module/constants/common.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model::responses;
using namespace agent_framework::model::literals;

AddEndpoint::AddEndpoint(const std::string& endpoint, const attribute::Oem& oem):
    m_endpoint(endpoint), m_oem{oem} {}


json::Json AddEndpoint::to_json() const {
    json::Json value = json::Json();
    value[Endpoint::ENDPOINT] = m_endpoint;
    value[Endpoint::OEM] = m_oem.to_json();
    return value;
}

AddEndpoint AddEndpoint::from_json(const json::Json& json) {
    return AddEndpoint{
        json[Endpoint::ENDPOINT],
        attribute::Oem::from_json(json[Endpoint::OEM])
    };
}
