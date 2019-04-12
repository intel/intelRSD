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
 * */

#include "psme/rest/endpoints/storage/volume_collection.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/volume.hpp"
#include "psme/rest/server/multiplexer.hpp"

#include "psme/rest/model/handlers/handler_manager.hpp"
#include "psme/rest/model/handlers/generic_handler_deps.hpp"
#include "psme/rest/model/handlers/generic_handler.hpp"

#include "psme/rest/endpoints/task_service/monitor_content_builder.hpp"
#include "psme/rest/endpoints/task_service/task_service_utils.hpp"

#include "agent-framework/module/requests/storage.hpp"
#include "agent-framework/module/responses/storage.hpp"



using namespace psme::rest;
using namespace psme::rest::model;
using namespace psme::rest::endpoint;
using namespace psme::rest::constants;
using namespace psme::rest::validators;

namespace {

using namespace agent_framework::model;


json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#VolumeCollection.VolumeCollection";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#VolumeCollection.VolumeCollection";
    r[Common::NAME] = "Volume Collection";
    r[Common::DESCRIPTION] = "Volume Collection description";
    r[Collection::ODATA_COUNT] = json::Json::value_t::null;
    r[Collection::MEMBERS] = json::Json::value_t::array;

    return r;
}


attribute::Array<attribute::Identifier> get_identifiers(const json::Json& json_identifiers) {
    attribute::Array<attribute::Identifier> result{};
    for (const auto& json_identifier : json_identifiers) {
        attribute::Identifier identifier{};
        identifier.set_durable_name(json_identifier[Common::DURABLE_NAME]);
        identifier.set_durable_name_format(json_identifier[Common::DURABLE_NAME_FORMAT]);
        result.add_entry(std::move(identifier));
    }
    return result;
}


attribute::Array<attribute::CapacitySource> read_capacity_sources(const json::Json& json) {
    attribute::Array<attribute::CapacitySource> result{};
    if (json.count(Swordfish::CAPACITY_SOURCES) && !json.value(Swordfish::CAPACITY_SOURCES, json::Json()).is_null()) {
        const auto& array_capacity_sources = json[Swordfish::CAPACITY_SOURCES];
        for (const auto& json_capacity_source : array_capacity_sources) {
            if (!json_capacity_source.count(Swordfish::PROVIDING_POOLS) ||
                !json_capacity_source.value(Swordfish::PROVIDING_POOLS, json::Json()).is_array()) {
                continue;
            }
            attribute::CapacitySource capacity_source{};
            const auto& array_providing_pools = json_capacity_source[Swordfish::PROVIDING_POOLS];
            for (const auto& json_providing_pool : array_providing_pools) {
                const auto& providing_pool_url = json_providing_pool[Common::ODATA_ID].get<std::string>();

                auto params = server::Multiplexer::get_instance()->get_params(providing_pool_url,
                                                                              constants::Routes::STORAGE_POOL_PATH);

                auto pool_uuid =
                    model::try_find<agent_framework::model::StorageService, agent_framework::model::StoragePool>(
                        params).get_uuid();

                if (pool_uuid.has_value()) {
                    capacity_source.add_providing_pool(std::move(pool_uuid));
                }
                else {
                    log_error("rest", "Cannot find pool: " << providing_pool_url);
                    throw agent_framework::exceptions::InvalidValue("Cannot find pool: " + providing_pool_url);
                }
            }
            result.add_entry(std::move(capacity_source));
        }
    }
    return result;
}


attribute::Array<enums::AccessCapability> read_access_capabilities(const json::Json& json) {
    if (!json.count(Swordfish::ACCESS_CAPABILITIES) ||
        json.value(Swordfish::ACCESS_CAPABILITIES, json::Json()).is_null()) {
        return attribute::Array<enums::AccessCapability>();
    }
    else {
        return attribute::Array<enums::AccessCapability>::from_json(json[Swordfish::ACCESS_CAPABILITIES]);
    }
}


attribute::Array<attribute::ReplicaInfo> read_replica_infos(const json::Json& json) {
    attribute::Array<attribute::ReplicaInfo> result{};
    if (json.count(Swordfish::REPLICA_INFOS) && !json.value(Swordfish::REPLICA_INFOS, json::Json()).is_null()) {
        const auto& json_replica_infos = json[Swordfish::REPLICA_INFOS];
        for (const auto& json_replica_info : json_replica_infos) {
            attribute::ReplicaInfo replica_info{};

            if (!json_replica_info.value(ReplicaInfo::REPLICA, json::Json()).is_null()) {
                const auto& replica_info_url = json_replica_info[ReplicaInfo::REPLICA][Common::ODATA_ID].get<std::string>();

                auto params = server::Multiplexer::get_instance()->get_params(replica_info_url,
                                                                              constants::Routes::VOLUME_PATH);

                auto volume_uuid =
                    model::try_find<agent_framework::model::StorageService, agent_framework::model::Volume>(
                        params).get_uuid();

                if (volume_uuid.has_value()) {
                    replica_info.set_replica(std::move(volume_uuid.value()));
                }
                else {
                    log_error("rest", "Cannot find volume: " << replica_info_url);
                    throw agent_framework::exceptions::InvalidValue("Cannot find volume: " + replica_info_url);
                }
            }
            if (!json_replica_info.value(ReplicaInfo::REPLICA_TYPE, json::Json()).is_null()) {
                replica_info.set_replica_type(
                    enums::ReplicaType::from_string(json_replica_info[ReplicaInfo::REPLICA_TYPE])
                );
            }

            result.add_entry(std::move(replica_info));
        }
    }
    return result;
}

}


VolumeCollection::VolumeCollection(const std::string& path) : EndpointBase(path) {}


VolumeCollection::~VolumeCollection() {}


void VolumeCollection::get(const server::Request& request, server::Response& response) {
    auto json = ::make_prototype();

    json[Common::ODATA_ID] = PathBuilder(request).build();

    auto storage_service_uuid =
        model::find<agent_framework::model::StorageService>(request.params).get_uuid();

    auto keys = agent_framework::module::get_manager<agent_framework::model::Volume>().get_ids(storage_service_uuid);

    json[Collection::ODATA_COUNT] = static_cast<std::uint32_t>(keys.size());
    for (const auto& key : keys) {
        json::Json link(json::Json::value_t::object);
        link[Common::ODATA_ID] = PathBuilder(request).append(key).build();
        json[Collection::MEMBERS].push_back(std::move(link));
    }

    set_response(response, json);
}


void VolumeCollection::post(const server::Request& request, server::Response& response) {
    static const constexpr char TRANSACTION_NAME[] = "PostVolumeCollection";
    auto json = JsonValidator::validate_request_body<schema::VolumePostSchema>(request);

    auto parent_service =
        psme::rest::model::find<agent_framework::model::StorageService>(request.params).get();

    auto identifiers = json.value(Common::IDENTIFIERS, json::Json::array());

    agent_framework::model::requests::AddVolume add_volume_request{
        enums::VolumeType::RawDevice,
        ::get_identifiers(identifiers),
        json[Swordfish::CAPACITY_BYTES].get<std::int64_t>(),
        ::read_capacity_sources(json),
        ::read_access_capabilities(json),
        json.value(Common::OEM, json::Json::object())
            .value(Common::RACKSCALE, json::Json::object())
            .value(Swordfish::BOOTABLE, OptionalField<bool>()),
        ::read_replica_infos(json)
    };

    auto agent_id = parent_service.get_agent_id();
    auto gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(agent_id);
    auto add_volume = [&, gami_agent] {
        auto add_volume_response = gami_agent->
            execute<agent_framework::model::responses::AddVolume>(add_volume_request);

        /* Prepare response which will be presented on task completion */
        auto response_renderer = [request, add_volume_response](json::Json in_json) -> server::Response {
            auto command_response = agent_framework::model::responses::AddVolume::from_json(in_json);
            auto created_volume_id = agent_framework::module::get_manager<Volume>()
                .get_entry(command_response.get_volume()).get_id();

            Response promised_response{};
            promised_response.set_status(server::status_2XX::CREATED);
            endpoint::utils::set_location_header(request, promised_response,
                                                 PathBuilder(request).append(created_volume_id).build());

            return promised_response;
        };

        /* Reload Storage Service resource (on task completion) */
        auto completion_notifier = [agent_id, parent_service](const Uuid& task_uuid) {
            {
                auto task = agent_framework::module::get_manager<agent_framework::model::Task>()
                    .get_entry_reference(task_uuid);
                if (task->get_state() == agent_framework::model::enums::TaskState::Completed) {
                    task->set_messages(task_service_utils::build_created_message());
                }
            }

            handler::HandlerManager::get_instance()->
                get_handler(agent_framework::model::enums::Component::Manager)->
                load(psme::core::agent::AgentManager::get_instance()->get_agent(agent_id), {},
                     agent_framework::model::enums::Component::None, parent_service.get_parent_uuid(), true);
        };

        /*
         * If the response object does not contain "task" field,
         * then the request resulted in an action,
         * otherwise a task needs to be created
         * */
        if (add_volume_response.get_task().empty()) {
            /* Reload full storage service starting from manager */
            handler::HandlerManager::get_instance()->
                get_handler(agent_framework::model::enums::Component::Manager)->
                load(gami_agent, {}, agent_framework::model::enums::Component::None,
                     parent_service.get_parent_uuid(), true);

            auto created_volume_id = agent_framework::module::get_manager<Volume>()
                .get_entry(add_volume_response.get_volume()).get_id();

            endpoint::utils::set_location_header(request, response,
                                                 PathBuilder(request).append(created_volume_id).build());
            response.set_status(server::status_2XX::CREATED);
        }
        else {
            const auto& task_uuid = add_volume_response.get_task();

            auto task_handler =
                handler::HandlerManager::get_instance()->get_handler(agent_framework::model::enums::Component::Task);
            task_handler->load(gami_agent, {}, agent_framework::model::enums::Component::Task, task_uuid, false);

            MonitorContentBuilder::get_instance()->add_builder(task_uuid, response_renderer);
            {
                agent_framework::module::get_manager<agent_framework::model::Task>()
                    .get_entry_reference(task_uuid)->add_completion_notifier(std::bind(completion_notifier, task_uuid));
            }

            auto task_monitor_url =
                PathBuilder(utils::get_component_url(agent_framework::model::enums::Component::Task, task_uuid))
                    .append(Monitor::MONITOR).build();

            psme::rest::endpoint::utils::set_location_header(request, response, task_monitor_url);
            response.set_body(psme::rest::endpoint::task_service_utils::call_task_get(task_uuid).get_body());
            response.set_status(server::status_2XX::ACCEPTED);
        }
    };

    gami_agent->execute_in_transaction(TRANSACTION_NAME, add_volume);
}
