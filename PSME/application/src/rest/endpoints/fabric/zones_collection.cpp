/*!
 * @copyright
 * Copyright (c) 2015-2018 Intel Corporation
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
 * */

#include "psme/rest/endpoints/fabric/zones_collection.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/zones_collection.hpp"
#include "psme/rest/model/handlers/handler_manager.hpp"
#include "psme/rest/utils/zone_utils.hpp"
#include "psme/rest/model/handlers/generic_handler_deps.hpp"
#include "psme/rest/model/handlers/generic_handler.hpp"
#include "agent-framework/module/requests/pnc.hpp"
#include "agent-framework/module/responses/common.hpp"



using namespace psme::rest;
using namespace psme::rest::endpoint;
using namespace psme::rest::constants;
using namespace agent_framework::model;


namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#ZoneCollection.ZoneCollection";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#ZoneCollection.ZoneCollection";
    r[Common::DESCRIPTION] = "Zone Collection";
    r[Common::NAME] = "Zone Collection";
    r[Collection::ODATA_COUNT] = json::Value::Type::NIL;
    r[Collection::MEMBERS] = json::Value::Type::ARRAY;

    return r;
}

}


ZonesCollection::ZonesCollection(const std::string& path) : EndpointBase(path) {}


ZonesCollection::~ZonesCollection() {}


void ZonesCollection::get(const server::Request& req, server::Response& res) {
    auto json = ::make_prototype();
    auto fabric_uuid = psme::rest::model::Find<agent_framework::model::Fabric>(req.params[PathParam::FABRIC_ID])
        .get_uuid();

    auto zone_ids = agent_framework::module::get_manager<agent_framework::model::Zone>().get_ids(fabric_uuid);

    json[Common::ODATA_ID] = PathBuilder(req).build();
    json[Collection::ODATA_COUNT] = static_cast<std::uint32_t>(zone_ids.size());

    for (const auto& zone_id : zone_ids) {
        json::Value link{};
        link[Common::ODATA_ID] = PathBuilder(req).append(zone_id).build();
        json[Collection::MEMBERS].push_back(std::move(link));
    }
    set_response(res, json);
}

void ZonesCollection::post(const server::Request& request, server::Response& response) {
    auto json = validators::JsonValidator::validate_request_body<rest::validators::schema::ZonesCollectionPostSchema>(request);

    auto fabric = psme::rest::model::Find<agent_framework::model::Fabric>(request.params[PathParam::FABRIC_ID]).get();
    auto fabric_uuid = fabric.get_uuid();

    using AgentManager = psme::core::agent::AgentManager;
    using HandlerManager = psme::rest::model::handler::HandlerManager;

   requests::AddZone add_zone_request{
        fabric_uuid,
        psme::rest::utils::ZoneUtils::validate_post_links_and_get_endpoint_uuids(json[Common::LINKS]),
        attribute::Oem()
    };

    auto gami_agent = AgentManager::get_instance()->get_agent(fabric.get_agent_id());

    auto add_zone = [&,gami_agent] {
        auto add_zone_response =
            gami_agent->execute<responses::AddZone>(add_zone_request);

        HandlerManager::get_instance()->
            get_handler(enums::Component::Zone)->
            load(gami_agent, fabric_uuid, enums::Component::Fabric, add_zone_response.get_zone(), true);

        auto created_zone_id = agent_framework::module::CommonComponents::get_instance()->
            get_zone_manager().get_entry_reference(add_zone_response.get_zone())->get_id();

        ::psme::rest::endpoint::utils::set_location_header(
            request, response, PathBuilder(request).append(created_zone_id).build());

        response.set_status(server::status_2XX::CREATED);
    };

    gami_agent->execute_in_transaction(add_zone);
}
