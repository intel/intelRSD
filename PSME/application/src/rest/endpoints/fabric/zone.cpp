/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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

#include "agent-framework/module/constants/pnc.hpp"
#include "agent-framework/module/requests/pnc.hpp"
#include "agent-framework/module/responses/common.hpp"

#include "psme/rest/endpoints/fabric/zone.hpp"
#include "psme/rest/model/handlers/handler_manager.hpp"
#include "psme/rest/model/handlers/generic_handler_deps.hpp"
#include "psme/rest/model/handlers/generic_handler.hpp"
#include "psme/rest/utils/zone_utils.hpp"
#include "psme/rest/utils/status_helpers.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/zone.hpp"
#include "psme/rest/endpoints/task_service/monitor_content_builder.hpp"
#include "psme/rest/endpoints/task_service/task_service_utils.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::utils;
using namespace psme::rest::validators;
using namespace agent_framework::model;
using HandlerManager = psme::rest::model::handler::HandlerManager;

namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Zone.Zone";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#Zone.v1_0_0.Zone";
    r[Common::NAME] = "Zone";
    r[Common::DESCRIPTION] = "Zone";
    r[Common::ID] = json::Json::value_t::null;

    r[Common::STATUS][Common::STATE] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Json::value_t::null;

    json::Json links = json::Json();
    links[constants::Fabric::ENDPOINTS] = json::Json::value_t::array;
    links[constants::Zone::INVOLVED_SWITCHES] = json::Json::value_t::array;
    links[Common::OEM] = json::Json::value_t::object;
    r[Common::LINKS] = std::move(links);
    r[Common::OEM] = json::Json::value_t::object;

    return r;
}


void fill_links(const agent_framework::model::Zone& zone, json::Json& json) {
    // fill Endpoint links
    for (const auto& endpoint_uuid : agent_framework::module::CommonComponents::get_instance()->get_zone_endpoint_manager().get_children(
        zone.get_uuid())) {
        json::Json endpoint_link = json::Json();
        endpoint_link[Common::ODATA_ID] = endpoint::PathBuilder(
            endpoint::utils::get_component_url(agent_framework::model::enums::Component::Endpoint,
                                               endpoint_uuid)).build();
        json[Common::LINKS][constants::Fabric::ENDPOINTS].push_back(endpoint_link);
    }

    // fill Switch links
    // the Switches are children of the Fabric which is the parent of this Zone
    const auto& fabric_uuid = zone.get_parent_uuid();
    for (const auto& switch_uuid : agent_framework::module::get_manager<agent_framework::model::Switch>().get_keys(
        fabric_uuid)) {
        json::Json switch_link = json::Json();
        switch_link[Common::ODATA_ID] = endpoint::PathBuilder(
            endpoint::utils::get_component_url(agent_framework::model::enums::Component::Switch, switch_uuid)).build();
        json[Common::LINKS][constants::Zone::INVOLVED_SWITCHES].push_back(switch_link);
    }
}
}


endpoint::Zone::Zone(const std::string& path) : EndpointBase(path) {}


endpoint::Zone::~Zone() {}


void endpoint::Zone::get(const server::Request& req, server::Response& res) {
    auto json = ::make_prototype();

    json[Common::ODATA_ID] = PathBuilder(req).build();

    const auto zone = psme::rest::model::find<agent_framework::model::Fabric, agent_framework::model::Zone>(
        req.params).get();

    json[Common::ID] = req.params[PathParam::ZONE_ID];

    fill_links(zone, json);

    endpoint::status_to_json(zone, json);
    json[Common::STATUS][Common::HEALTH_ROLLUP] =
        endpoint::HealthRollup<agent_framework::model::Zone>().get(zone.get_uuid());

    set_response(res, json);
}


void endpoint::Zone::patch(const server::Request& request, server::Response& response) {
    static const constexpr char TRANSACTION_NAME[] = "PatchZone";
    auto json = JsonValidator::validate_request_body<schema::ZonePatchSchema>(request);

    const auto zone = psme::rest::model::find<agent_framework::model::Fabric, agent_framework::model::Zone>(
        request.params).get();

    if (!json.count(Common::LINKS)) { // Schema validators ensure that Endpoints are inside Links
        get(request, response);
        return;
    }
    // else : execute PATCH

    const auto requested_endpoints = ZoneUtils::validate_patch_links_and_get_endpoint_uuids(json[Common::LINKS]);

    responses::AddZoneEndpoints add_endpoints_response;
    responses::DeleteZoneEndpoints delete_endpoints_response;
    std::string task_uuid;

    const auto& agent_id = zone.get_agent_id();
    const auto& parent_uuid = zone.get_parent_uuid();
    const auto& zone_uuid = zone.get_uuid();
    auto gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(agent_id);

    auto response_renderer = [this, request](json::Json /* in_json */) -> server::Response {
        // on finished task, the agent sends a command response. In the case od addZoneEndpoints and deleteZoneEndpoints
        // it has only an Oem field.
        server::Response finished_task_response{};
        this->get(request, finished_task_response);
        return finished_task_response;
    };

    auto completion_notifier = [zone_uuid, parent_uuid, agent_id]() {
        HandlerManager::get_instance()->get_handler(enums::Component::Zone)->
            load(psme::core::agent::AgentManager::get_instance()->get_agent(agent_id),
                 parent_uuid,
                 enums::Component::Fabric,
                 zone_uuid,
                 true);
    };

    bool send_add_endpoints = false;
    bool send_delete_endpoints = false;
    std::shared_ptr<requests::AddZoneEndpoints> add_request;
    std::shared_ptr<requests::DeleteZoneEndpoints> delete_request;

    std::vector<std::string> endpoints_to_add;
    std::vector<std::string> endpoints_to_remove;
    psme::rest::endpoint::utils::children_to_add_to_remove(agent_framework::module::CommonComponents::get_instance()->
        get_zone_endpoint_manager(), zone.get_uuid(), requested_endpoints, endpoints_to_add, endpoints_to_remove);

    if (endpoints_to_add.size() > 0) {
        auto upstream_endpoint_to_add = ZoneUtils::get_upstream_endpoint(endpoints_to_add);
        if (upstream_endpoint_to_add.has_value()) {
            THROW(agent_framework::exceptions::InvalidValue, "rest",
                  "Adding upstream endpoints to the Zone is not allowed! Do not add "
                  + endpoint::utils::get_component_url(agent_framework::model::enums::Component::Endpoint,
                                                       upstream_endpoint_to_add.value())
                  + " to the Zone."
            );
        }

        log_info("rest", "Zone PATCH: adding " + std::to_string(endpoints_to_add.size()) + " endpoints to zone.");
        add_request = std::make_shared<requests::AddZoneEndpoints>(
            zone.get_uuid(), endpoints_to_add, attribute::Oem());
        send_add_endpoints = true;
    }

    if (endpoints_to_remove.size() > 0) {
        auto upstream_endpoint_to_remove = ZoneUtils::get_upstream_endpoint(endpoints_to_remove);
        if (upstream_endpoint_to_remove.has_value()) {
            THROW(agent_framework::exceptions::InvalidValue, "rest",
                  "Removing upstream endpoint from the Zone is not allowed! Do not remove "
                  + endpoint::utils::get_component_url(agent_framework::model::enums::Component::Endpoint,
                                                       upstream_endpoint_to_remove.value())
                  + " from the Zone."
            );
        }

        log_info("rest",
                 "Zone PATCH: removing " + std::to_string(endpoints_to_remove.size()) + " endpoints from zone.");
        delete_request = std::make_shared<requests::DeleteZoneEndpoints>(
            zone.get_uuid(), endpoints_to_remove, attribute::Oem());
        send_delete_endpoints = true;
    }

    auto patch_zone_endpoints = [&, gami_agent] {
        if (send_add_endpoints) {
            add_endpoints_response = gami_agent->execute<responses::AddZoneEndpoints>(*add_request);
        }
        if (send_delete_endpoints) {
            delete_endpoints_response = gami_agent->execute<responses::DeleteZoneEndpoints>(*delete_request);
        }

        auto is_add_task = !add_endpoints_response.get_task().empty();
        auto is_delete_task = !delete_endpoints_response.get_task().empty();

        // If neither gami command returned a task - reload zone and return PATCH response.
        // If the second command returned a task, we ASSUME (!) that the commands have been executed sequentially,
        // (and if the first command returned a task, it's executed first) and we only monitor the task from
        // the second command.
        // Finally, if the first command returned a task, and the second didn't, monitor the task from the first command.

        if ((!is_add_task) && (!is_delete_task)) {
            log_info("rest", "Zone PATCH: no tasks were required to execute PATCH");
            // reload the zone and it's collections
            HandlerManager::get_instance()->get_handler(enums::Component::Zone)->
                load(gami_agent,
                     zone.get_parent_uuid(),
                     enums::Component::Fabric,
                     zone.get_uuid(),
                     true);
            get(request, response);
            return;
        }
        else if (is_delete_task) {
            if (is_add_task) {
                log_info("rest", "Zone PATCH: addZoneEndpoints returned a task,"
                                 " but it's ignored in favour of the second task returned by deleteZoneEndpoints");
            }
            log_info("rest", "Zone PATCH: deleteZoneEndpoints returned a task.");
            task_uuid = delete_endpoints_response.get_task();
        }
        else {
            log_info("rest", "Zone PATCH: addZoneEndpoints returned a task.");
            task_uuid = add_endpoints_response.get_task();
        }

        auto task_handler = psme::rest::model::handler::HandlerManager::get_instance()->
            get_handler(agent_framework::model::enums::Component::Task);
        task_handler->load(gami_agent,
                           "",
                           agent_framework::model::enums::Component::Task,
                           task_uuid,
                           false);

        endpoint::MonitorContentBuilder::get_instance()->add_builder(task_uuid, response_renderer);
        {
            agent_framework::module::CommonComponents::get_instance()->get_task_manager()
                .get_entry_reference(task_uuid)->add_completion_notifier(completion_notifier);
        }

        std::string task_monitor_url = endpoint::PathBuilder(endpoint::utils::get_component_url(
            agent_framework::model::enums::Component::Task, task_uuid)).append(Monitor::MONITOR).build();
        psme::rest::endpoint::utils::set_location_header(request, response, task_monitor_url);
        response.set_body(psme::rest::endpoint::task_service_utils::call_task_get(task_uuid).get_body());
        response.set_status(server::status_2XX::ACCEPTED);

    };

    gami_agent->execute_in_transaction(TRANSACTION_NAME, patch_zone_endpoints);
}


void endpoint::Zone::del(const server::Request& request, server::Response& response) {
    using AgentManager = psme::core::agent::AgentManager;
    static const constexpr char TRANSACTION_NAME[] = "DeleteZone";

    const auto zone_uuid = psme::rest::model::find<agent_framework::model::Fabric, agent_framework::model::Zone>(
        request.params).get_uuid();

    const auto fabric = psme::rest::model::find<agent_framework::model::Fabric>(
        request.params).get();

    auto gami_req = requests::DeleteZone(zone_uuid);

    auto gami_agent = AgentManager::get_instance()->get_agent(fabric.get_agent_id());

    auto delete_zone = [&, gami_agent] {
        // try removing the zone from agent's model
        auto delete_zone_response = gami_agent->execute<responses::DeleteZone>(gami_req);

        if (delete_zone_response.get_task().empty()) {

            // remove the resource by reload the tree
            HandlerManager::get_instance()->get_handler(enums::Component::Fabric)->
                load(gami_agent, fabric.get_parent_uuid(), enums::Component::Manager, fabric.get_uuid(), true);
            response.set_status(server::status_2XX::NO_CONTENT);
        }
        else {
            const auto& task_uuid = delete_zone_response.get_task();

            auto completion_notifier = [zone_uuid, fabric, gami_agent]() {
                HandlerManager::get_instance()->get_handler(enums::Component::Fabric)->
                    load(gami_agent,
                         fabric.get_parent_uuid(),
                         enums::Component::Manager,
                         fabric.get_uuid(),
                         true);
            };

            auto response_renderer = [this, request](json::Json /* in_json */) -> server::Response {
                server::Response finished_task_response{};
                finished_task_response.set_status(server::status_2XX::NO_CONTENT);
                return finished_task_response;
            };

            auto task_handler = psme::rest::model::handler::HandlerManager::get_instance()->
                get_handler(agent_framework::model::enums::Component::Task);
            task_handler->load(gami_agent,
                               "",
                               agent_framework::model::enums::Component::Task,
                               task_uuid,
                               false);
            endpoint::MonitorContentBuilder::get_instance()->add_builder(task_uuid, response_renderer);
            agent_framework::module::CommonComponents::get_instance()->get_task_manager()
                .get_entry_reference(task_uuid)->add_completion_notifier(completion_notifier);

            std::string task_monitor_url = endpoint::PathBuilder(endpoint::utils::get_component_url(
                agent_framework::model::enums::Component::Task, task_uuid)).append(Monitor::MONITOR).build();
            psme::rest::endpoint::utils::set_location_header(request, response, task_monitor_url);
            response.set_body(psme::rest::endpoint::task_service_utils::call_task_get(task_uuid).get_body());
            response.set_status(server::status_2XX::ACCEPTED);
        }
    };

    gami_agent->execute_in_transaction(TRANSACTION_NAME, delete_zone);
}
