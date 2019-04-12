/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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

#include "agent-framework/module/model/endpoint.hpp"
#include "agent-framework/module/model/zone.hpp"
#include "agent-framework/module/constants/pnc.hpp"
#include "json-wrapper/json-wrapper.hpp"
#include "agent-framework/module/managers/utils/manager_utils.hpp"



using namespace agent_framework::model;
using namespace agent_framework::model::attribute;

const enums::Component Zone::component = enums::Component::Zone;
const enums::CollectionName Zone::collection_name = enums::CollectionName::Zones;


Zone::Zone(const std::string& parent_uuid, enums::Component parent_type) :
    Resource{parent_uuid, parent_type} {}


Zone::~Zone() {}


json::Json Zone::to_json() const {
    json::Json result = json::Json();
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

    return fabric_zone;
}


bool Zone::is_initiator_in_zone(const Uuid& zone) {
    auto endpoints = module::get_m2m_manager<Zone, Endpoint>().get_children(zone);
    auto& manager = module::get_manager<Endpoint>();
    const auto& it = find_if(endpoints.begin(), endpoints.end(),
                             [&manager](const Uuid& uuid) { return Endpoint::is_initiator(manager.get_entry(uuid)); });
    return it != endpoints.end();
}


bool Zone::is_target_in_zone(const Uuid& zone) {
    auto endpoints = module::get_m2m_manager<Zone, Endpoint>().get_children(zone);
    auto& manager = module::get_manager<Endpoint>();
    const auto& it = find_if(endpoints.begin(), endpoints.end(),
                             [&manager](const Uuid& uuid) { return Endpoint::is_target(manager.get_entry(uuid)); });
    return it != endpoints.end();
}


std::vector<Uuid> Zone::get_connected_devices(const Uuid& zone_uuid) {
    auto endpoint_uuids = module::get_m2m_manager<Zone, Endpoint>().get_children(zone_uuid);
    std::vector<Uuid> devices;
    for (const auto& endpoint_uuid : endpoint_uuids) {
        if (Endpoint::is_target(endpoint_uuid)) {
            auto connected_entities = module::get_manager<Endpoint>().get_entry(endpoint_uuid).get_connected_entities();
            for (const auto& entity : connected_entities) {
                devices.push_back(entity.get_entity());
            }
        }
    }
    return devices;
}


OptionalField<Endpoint> Zone::get_initiator_endpoint(const Uuid& zone_uuid) {
    auto endpoints = module::get_m2m_manager<Zone, Endpoint>().get_children(zone_uuid);
    auto it = std::find_if(std::begin(endpoints), std::end(endpoints),
                           [](const auto& endpoint) { return Endpoint::is_initiator(endpoint); });

    return it != std::end(endpoints) ?
           OptionalField<Endpoint>(module::get_manager<Endpoint>().get_entry(*it)) : OptionalField<Endpoint>();
}