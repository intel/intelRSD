/*!
 * @brief ZoneBuilder implementation
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file zone_builder.cpp
 */

#include "agent-framework/discovery/builders/zone_builder.hpp"

using namespace agent_framework;
using namespace agent_framework::discovery;

model::Zone ZoneBuilder::build_default(const Uuid& parent_uuid) {
    model::Zone zone{parent_uuid};

    model::attribute::Status status{};
    status.set_health(model::enums::Health::OK);
    status.set_state(model::enums::State::Enabled);
    zone.set_status(std::move(status));

    zone.add_collection(model::attribute::Collection(
        model::enums::CollectionName::Endpoints,
        model::enums::CollectionType::Endpoints
    ));

    return zone;
}
