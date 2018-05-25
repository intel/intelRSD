/*!
 * @copyright
 * Copyright (c) 2017-2018 Intel Corporation
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

#include "psme/rest/constants/constants.hpp"
#include "psme/rest/endpoints/storage/volume.hpp"
#include "psme/rest/endpoints/task_service/monitor_content_builder.hpp"
#include "psme/rest/endpoints/task_service/task_service_utils.hpp"
#include "psme/rest/utils/status_helpers.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/volume.hpp"
#include "psme/rest/server/error/error_factory.hpp"

#include "agent-framework/module/requests/storage.hpp"
#include "agent-framework/module/responses/storage.hpp"
#include "agent-framework/module/utils/json_transformations.hpp"


using namespace psme::rest;
using namespace psme::rest::endpoint;
using namespace psme::rest::constants;
using namespace psme::rest::validators;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Volume.Volume";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#Volume.v1_1_0.Volume";
    r[Common::NAME] = "Swordfish Volume";
    r[Common::DESCRIPTION] = "Volume description";
    r[Common::ID] = json::Value::Type::NIL;

    r[Common::STATUS][Common::STATE] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Value::Type::NIL;

    r[Common::MODEL] = json::Value::Type::NIL;
    r[Common::MANUFACTURER] = json::Value::Type::NIL;
    r[Swordfish::BLOCK_SIZE_BYTES] = json::Value::Type::NIL;
    r[Swordfish::OPTIMUM_IO_SIZE_BYTES] = json::Value::Type::NIL;
    r[Swordfish::ACCESS_CAPABILITIES] = json::Value::Type::ARRAY;
    r[Swordfish::CAPACITY_BYTES] = json::Value::Type::NIL;
    r[Swordfish::CAPACITY] = json::Value::Type::NIL;
    r[Swordfish::CAPACITY_SOURCES] = json::Value::Type::ARRAY;
    r[Swordfish::REPLICA_INFOS] = json::Value::Type::ARRAY;
    r[Common::IDENTIFIERS] = json::Value::Type::ARRAY;

    r[Swordfish::CAPACITY][Data::DATA][Data::ALLOCATED_BYTES] = json::Value::Type::NIL;
    r[Swordfish::CAPACITY_SOURCES] = json::Value::Type::ARRAY;

    r[Common::ACTIONS] = json::Value::Type::OBJECT;

    r[Common::LINKS][Common::OEM][Common::RACKSCALE][Common::ODATA_TYPE] = "#Intel.Oem.VolumeLinks";
    r[Common::LINKS][StorageService::DRIVES] = json::Value::Type::ARRAY;

    r[Common::OEM] = json::Value::Type::OBJECT;
    r[Common::OEM][Common::RACKSCALE][Common::ODATA_TYPE] = "#Intel.Oem.Volume";
    r[Common::OEM][Common::RACKSCALE][Swordfish::BOOTABLE] = json::Value::Type::NIL;
    r[Common::OEM][Common::RACKSCALE][Swordfish::ERASED] = json::Value::Type::NIL;
    r[Common::OEM][Common::RACKSCALE][Swordfish::ERASE_ON_DETACH] = json::Value::Type::NIL;
    r[Common::OEM][Common::RACKSCALE][Common::METRICS] = json::Value::Type::OBJECT;

    return r;
}

static const std::map<std::string, std::string> gami_to_rest_attributes = {
    {agent_framework::model::literals::Volume::BOOTABLE, constants::Swordfish::BOOTABLE},
    {agent_framework::model::literals::Volume::ERASED, constants::Swordfish::ERASED},
};

}


endpoint::Volume::Volume(const std::string& path) : EndpointBase(path) {}


endpoint::Volume::~Volume() {}


void endpoint::Volume::get(const server::Request& req, server::Response& res) {
    auto r = ::make_prototype();

    auto volume =
        psme::rest::model::Find<agent_framework::model::Volume>(req.params[PathParam::VOLUME_ID])
            .via<agent_framework::model::StorageService>(req.params[PathParam::SERVICE_ID]).get();

    r[Common::ODATA_ID] = PathBuilder(req).build();
    r[Common::ID] = req.params[PathParam::VOLUME_ID];

    r[Swordfish::BLOCK_SIZE_BYTES] = volume.get_block_size_bytes();
    r[Swordfish::OPTIMUM_IO_SIZE_BYTES] = volume.get_optimum_io_size_bytes();

    if (utils::has_resource_capability(volume, Capability::NVME)) {
        r[Common::ACTIONS][Swordfish::VOLUME_INITIALIZE][Common::TARGET] = PathBuilder(req)
            .append(Common::ACTIONS)
            .append(Swordfish::VOLUME_INITIALIZE_ENDPOINT).build();
    }

    endpoint::status_to_json(volume, r);

    r[Swordfish::CAPACITY_BYTES] = volume.get_capacity().get_allocated_bytes();
    r[Swordfish::CAPACITY][Data::DATA][Data::ALLOCATED_BYTES] = volume.get_capacity().get_allocated_bytes();

    for (const auto& capacity_source : volume.get_capacity_sources()) {
        json::Value cs{};

        cs[Swordfish::PROVIDED_CAPACITY][Data::DATA][Data::ALLOCATED_BYTES] = capacity_source.get_allocated_bytes();
        cs[Swordfish::PROVIDED_CAPACITY][Data::DATA][Data::CONSUMED_BYTES] = capacity_source.get_consumed_bytes();

        for (const auto& drive : capacity_source.get_providing_drives()) {
            auto drive_url = utils::get_component_url(agent_framework::model::enums::Component::Drive, drive);
            json::Value link{};
            link[Common::ODATA_ID] = std::move(drive_url);
            cs[Swordfish::PROVIDING_DRIVES].push_back(std::move(link));
        }
        for (const auto& pool : capacity_source.get_providing_pools()) {
            auto pool_url = utils::get_component_url(agent_framework::model::enums::Component::StoragePool, pool);
            json::Value link{};
            link[Common::ODATA_ID] = std::move(pool_url);
            cs[Swordfish::PROVIDING_POOLS].push_back(std::move(link));
        }
        for (const auto& v : capacity_source.get_providing_volumes()) {
            auto volume_url = utils::get_component_url(agent_framework::model::enums::Component::Volume, v);
            json::Value link{};
            link[Common::ODATA_ID] = std::move(volume_url);
            cs[Swordfish::PROVIDING_VOLUMES].push_back(std::move(link));
        }

        r[Swordfish::CAPACITY_SOURCES].push_back(std::move(cs));
    }

    for (const auto& access_capability : volume.get_access_capabilities()) {
        r[Swordfish::ACCESS_CAPABILITIES].push_back(access_capability.to_string());
    }

    for (const auto& identifier : volume.get_identifiers()) {
        json::Value id{};
        id[Common::DURABLE_NAME] = identifier.get_durable_name();
        id[Common::DURABLE_NAME_FORMAT] = identifier.get_durable_name_format();
        r[Common::IDENTIFIERS].push_back(std::move(id));
    }

    for (const auto& replica_info : volume.get_replica_infos()) {
        json::Value replica{};
        replica[ReplicaInfo::REPLICA][Common::ODATA_ID] =
            utils::get_component_url(agent_framework::model::enums::Component::Volume, replica_info.get_replica());

        replica[ReplicaInfo::REPLICA_TYPE] = replica_info.get_replica_type();
        replica[ReplicaInfo::REPLICA_ROLE] = replica_info.get_replica_role();
        replica[ReplicaInfo::REPLICA_READ_ONLY_ACCESS] = replica_info.get_replica_read_only_access();
        r[Swordfish::REPLICA_INFOS].push_back(std::move(replica));
    }

    r[Common::OEM][Common::RACKSCALE][Swordfish::BOOTABLE] = volume.is_bootable();
    r[Common::OEM][Common::RACKSCALE][Swordfish::ERASED] = volume.is_erased();

    r[Common::LINKS][Common::OEM][Common::RACKSCALE][StorageService::ENDPOINTS] = json::Value::Type::ARRAY;
    for (const auto& endpoint : agent_framework::module::get_manager<agent_framework::model::Endpoint>().get_entries()) {
        for (const auto& entity : endpoint.get_connected_entities()) {
            if (volume.get_uuid() == entity.get_entity()) {
                json::Value link{};
                link[Common::ODATA_ID] = endpoint::PathBuilder(
                    endpoint::utils::get_component_url(agent_framework::model::enums::Component::Endpoint,
                                                       endpoint.get_uuid())).build();
                r[Common::LINKS][Common::OEM][Common::RACKSCALE][StorageService::ENDPOINTS].push_back(std::move(link));
            }
        }
    }

    r[Common::OEM][Common::RACKSCALE][Common::METRICS][Common::ODATA_ID] = endpoint::PathBuilder(req)
        .append(Common::METRICS).build();
    set_response(res, r);
}

void Volume::patch(const server::Request& request, server::Response& response) {

    auto volume = psme::rest::model::Find<agent_framework::model::Volume>(request.params[PathParam::VOLUME_ID])
        .via<agent_framework::model::StorageService>(request.params[PathParam::SERVICE_ID]).get();

    const auto& json = JsonValidator::validate_request_body<schema::VolumePatchSchema>(request);
    const auto& rackscale_json = json[constants::Common::OEM][constants::Common::RACKSCALE];
    agent_framework::model::attribute::Attributes attributes{};

    if (rackscale_json.is_member(constants::Swordfish::BOOTABLE)) {
        const auto value = agent_framework::model::utils::to_json_rpc(rackscale_json[constants::Swordfish::BOOTABLE]);
        attributes.set_value(agent_framework::model::literals::Volume::BOOTABLE, value);
    }

    if (rackscale_json.is_member(constants::Swordfish::ERASED)) {
        const auto value = agent_framework::model::utils::to_json_rpc(rackscale_json[constants::Swordfish::ERASED]);
        attributes.set_value(agent_framework::model::literals::Volume::ERASED, value);
    }

    if (!attributes.empty()) {
        agent_framework::model::requests::SetComponentAttributes
                set_component_attributes_request{volume.get_uuid(), attributes};

        const auto& gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(volume.get_agent_id());

        auto set_volume_attributes = [&, gami_agent] {
            const auto& set_component_attributes_response = gami_agent->
                execute<agent_framework::model::responses::SetComponentAttributes>(set_component_attributes_request);

            const auto& result_statuses = set_component_attributes_response.get_statuses();
            if (!result_statuses.empty()) {
                const auto& error = error::ErrorFactory::create_error_from_set_component_attributes_results(
                    result_statuses, gami_to_rest_attributes);
                throw error::ServerException(error);
            }

            psme::rest::model::handler::HandlerManager::get_instance()->get_handler(
                agent_framework::model::enums::Component::Volume)->
                load(gami_agent,
                     volume.get_parent_uuid(),
                     agent_framework::model::enums::Component::StorageService,
                     volume.get_uuid(),
                     false);
        };
        gami_agent->execute_in_transaction(set_volume_attributes);
    }
    get(request, response);
}

void Volume::del(const server::Request& request, server::Response& response) {

    auto storage_service =
        psme::rest::model::Find<agent_framework::model::StorageService>(request.params[PathParam::SERVICE_ID])
            .get();

    auto volume =
        psme::rest::model::Find<agent_framework::model::Volume>(request.params[PathParam::VOLUME_ID])
            .via<agent_framework::model::StorageService>(request.params[PathParam::SERVICE_ID])
            .get();

    auto gami_request = agent_framework::model::requests::DeleteVolume(volume.get_uuid());
    const auto& gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(volume.get_agent_id());

    auto completion_notifier = [storage_service, gami_agent](const Uuid task_uuid) {
        auto task = agent_framework::module::get_manager<agent_framework::model::Task>()
            .get_entry_reference(task_uuid);

        if (task->get_state() == agent_framework::model::enums::TaskState::Completed) {
            task->set_messages(task_service_utils::build_success_message());
        }

        /* Reload knowledge about current Volume and Storage Pool */
        psme::rest::model::handler::HandlerManager::get_instance()->
            get_handler(agent_framework::model::enums::Component::StorageService)->
            load(gami_agent, storage_service.get_parent_uuid(),
                 agent_framework::model::enums::Component::Manager,
                 storage_service.get_uuid(), true);

    };

    auto remove_drive = [&, gami_agent] {
        /* Removing volume from agent's model */
        auto delete_response = gami_agent->execute<agent_framework::model::responses::DeleteVolume>(gami_request);

        if (!delete_response.get_task().has_value()) {
            /* No task -> just reload knowledge about current Volume and its Storage Pool */
            psme::rest::model::handler::HandlerManager::get_instance()->
                get_handler(agent_framework::model::enums::Component::StorageService)->
                load(gami_agent, storage_service.get_parent_uuid(),
                     agent_framework::model::enums::Component::Manager,
                     storage_service.get_uuid(), true);

            response.set_status(server::status_2XX::NO_CONTENT);
        }
        else {
            const auto task_uuid = delete_response.get_task();
            auto task_handler = psme::rest::model::handler::HandlerManager::get_instance()->
                get_handler(agent_framework::model::enums::Component::Task);

            task_handler->load(gami_agent,
                               "", agent_framework::model::enums::Component::Task,
                               task_uuid, false);

            MonitorContentBuilder::get_instance()->
                add_builder(task_uuid, [](json::Json /* gami_response */) -> server::Response {
                    server::Response r{};
                    r.set_status(server::status_2XX::NO_CONTENT);
                    return r;
                });

            agent_framework::module::get_manager<agent_framework::model::Task>()
                .get_entry_reference(task_uuid)->
                add_completion_notifier(std::bind(completion_notifier, task_uuid));

            std::string task_monitor_url =
                PathBuilder(utils::get_component_url(agent_framework::model::enums::Component::Task, task_uuid))
                    .append(Monitor::MONITOR).build();

            psme::rest::endpoint::utils::set_location_header(request, response, task_monitor_url);
            response.set_body(psme::rest::endpoint::task_service_utils::call_task_get(task_uuid).get_body());
            response.set_status(server::status_2XX::ACCEPTED);
        }
    };

    gami_agent->execute_in_transaction(remove_drive);
}
