/*!
 * @brief Implementation of DeleteEndpoint class.
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
 * @file delete_endpoint.cpp
 */

#include "agent-framework/module/requests/common/delete_endpoint.hpp"
#include "agent-framework/module/constants/common.hpp"

#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;

DeleteEndpoint::DeleteEndpoint(const Uuid& endpoint) : m_endpoint{endpoint} { }

json::Json DeleteEndpoint::to_json() const {
    json::Json value = json::Json();
    value[Endpoint::ENDPOINT] = m_endpoint;
    return value;
}

DeleteEndpoint DeleteEndpoint::from_json(const json::Json& json) {
    return DeleteEndpoint{json[Endpoint::ENDPOINT].get<std::string>()};
}
