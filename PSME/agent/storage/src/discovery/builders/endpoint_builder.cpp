/*!
 * @brief EndpointBuilder implementation
 *
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation.
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
 * @header{Filesystem}
 * @file endpoint_builder.cpp
 */

#include <algorithm>
#include "discovery/builders/endpoint_builder.hpp"

using namespace agent::storage::discovery;
using namespace agent_framework;
using namespace agent_framework::model;


model::Endpoint EndpointBuilder::build_default(const Uuid& parent_uuid) {
    model::Endpoint endpoint{parent_uuid};

    model::attribute::Status status{};
    status.set_health(model::enums::Health::OK);
    status.set_state(model::enums::State::Enabled);
    endpoint.set_status(std::move(status));

    endpoint.set_protocol(enums::StorageProtocol::iSCSI);

    return endpoint;
}
