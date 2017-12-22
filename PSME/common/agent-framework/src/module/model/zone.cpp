/*!
 * @copyright
 * Copyright (c) 2016-2017 Intel Corporation
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
 * */

#include "agent-framework/module/model/zone.hpp"
#include "agent-framework/module/constants/pnc.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::attribute;

const enums::Component Zone::component = enums::Component::Zone;
const enums::CollectionName Zone::collection_name =
    enums::CollectionName::Zones;

Zone::Zone(const std::string& parent_uuid, enums::Component parent_type) :
    Resource{parent_uuid, parent_type} {}

Zone::~Zone() {}

json::Json Zone::to_json() const {
    json::Json result{};
    result[literals::Zone::COLLECTIONS] = get_collections().to_json();
    result[literals::Zone::STATUS] = get_status().to_json();
    result[literals::Zone::OEM] = get_oem().to_json();
    return result;
}

Zone Zone::from_json(const json::Json& json) {
    Zone fabric_zone{};
    fabric_zone.set_collections(Collections::from_json(json[literals::Zone::COLLECTIONS]));
    fabric_zone.set_status(Status::from_json(json[literals::Zone::STATUS]));
    fabric_zone.set_oem(Oem::from_json(json[literals::Zone::OEM]));

    fabric_zone.set_resource_hash(json);
    return fabric_zone;
}
