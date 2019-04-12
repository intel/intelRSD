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



using namespace psme::rest;
using namespace psme::rest::endpoint;
using namespace psme::rest::constants;
using namespace psme::rest::validators;

namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Volume.Volume";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#Volume.v1_1_0.Volume";
    r[Common::NAME] = "Swordfish Volume";
    r[Common::DESCRIPTION] = "Volume description";
    r[Common::ID] = json::Json::value_t::null;

    r[Common::STATUS][Common::STATE] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Json::value_t::null;

    r[Common::MODEL] = json::Json::value_t::null;
    r[Common::MANUFACTURER] = json::Json::value_t::null;
    r[Swordfish::BLOCK_SIZE_BYTES] = json::Json::value_t::null;
    r[Swordfish::OPTIMUM_IO_SIZE_BYTES] = json::Json::value_t::null;
    r[Swordfish::ACCESS_CAPABILITIES] = json::Json::value_t::array;
    r[Swordfish::CAPACITY_BYTES] = json::Json::value_t::null;
    r[Swordfish::REPLICA_INFOS] = json::Json::value_t::array;
    r[Common::IDENTIFIERS] = json::Json::value_t::array;

    r[Swordfish::CAPACITY] = json::Json::value_t::null;
    r[Swordfish::CAPACITY][Data::DATA][Data::ALLOCATED_BYTES] = json::Json::value_t::null;
    r[Swordfish::CAPACITY][Swordfish::IS_THIN_PROVISIONED] = json::Json{};
    r[Swordfish::CAPACITY_SOURCES] = json::Json::value_t::array;

    r[Common::ACTIONS] = json::Json::value_t::object;

    r[Common::LINKS][Common::OEM][Common::RACKSCALE][Common::ODATA_TYPE] = "#Intel.Oem.VolumeLinks";
    r[Common::LINKS][StorageService::DRIVES] = json::Json::value_t::array;

    r[Common::OEM] = json::Json::value_t::object;
    r[Common::OEM][Common::RACKSCALE][Common::ODATA_TYPE] = "#Intel.Oem.Volume";
    r[Common::OEM][Common::RACKSCALE][Swordfish::BOOTABLE] = json::Json::value_t::null;
    r[Common::OEM][Common::RACKSCALE][Common::METRICS] = json::Json::value_t::object;

    return r;
}


static const std::map<std::string, std::string> gami_to_rest_attributes = {
    {agent_framework::model::literals::Volume::BOOTABLE, constants::Swordfish::BOOTABLE},
};

}


endpoint::Volume::Volume(const std::string& path) : EndpointBase(path) {}


endpoint::Volume::~Volume() {}


void endpoint::Volume::get(const server::Request& req, server::Response& res) {
    auto r = ::make_prototype();

    auto volume =
        model::find<agent_framework::model::StorageService, agent_framework::model::Volume>(req.params).get();

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
    utils::fill_name_and_description(volume, r);

    r[Swordfish::CAPACITY_BYTES] = volume.get_capacity().get_allocated_bytes();
    r[Swordfish::CAPACITY][Common::ODATA_TYPE] = "#Capacity.v1_0_0.Capacity";
    r[Swordfish::CAPACITY][Data::DATA][Data::ALLOCATED_BYTES] = volume.get_capacity().get_allocated_bytes();
    r[Swordfish::CAPACITY][Swordfish::IS_THIN_PROVISIONED] = volume.get_capacity().is_thin_provisioned();

    for (std::size_t i = 0; i < volume.get_capacity_sources().size(); i++) {
        json::Json cs = json::Json();

        cs[Common::ODATA_ID] = endpoint::PathBuilder(req)
            .append(constants::Swordfish::CAPACITY_SOURCES)
            .append(i + 1)
            .build();

        r[Swordfish::CAPACITY_SOURCES].push_back(std::move(cs));
    }

    for (const auto& access_capability : volume.get_access_capabilities()) {
        r[Swordfish::ACCESS_CAPABILITIES].push_back(access_capability.to_string());
    }

    for (const auto& identifier : volume.get_identifiers()) {
        json::Json id = json::Json();
        id[Common::ODATA_TYPE] = "#Resource.v1_1_0.Identifier";
        id[Common::DURABLE_NAME] = identifier.get_durable_name();
        id[Common::DURABLE_NAME_FORMAT] = identifier.get_durable_name_format();
        r[Common::IDENTIFIERS].push_back(std::move(id));
    }

    for (const auto& replica_info : volume.get_replica_infos()) {
        try {
            json::Json replica{};
            replica[ReplicaInfo::REPLICA] = json::Json{};
            replica[Common::ODATA_TYPE] = "#StorageReplicaInfo.v1_0_0.ReplicaInfo";
            if (replica_info.get_replica().has_value()) {
                replica[ReplicaInfo::REPLICA][Common::ODATA_ID] =
                    utils::get_component_url(agent_framework::model::enums::Component::Volume,
                                             replica_info.get_replica());
            }

            replica[ReplicaInfo::REPLICA_TYPE] = replica_info.get_replica_type();
            replica[ReplicaInfo::REPLICA_ROLE] = replica_info.get_replica_role();
            replica[ReplicaInfo::REPLICA_READ_ONLY_ACCESS] = replica_info.get_replica_read_only_access();
            r[Swordfish::REPLICA_INFOS].push_back(std::move(replica));
        }
        catch (const agent_framework::exceptions::InvalidUuid& exception) {
            log_warning("rest", "Could not read replica info: " << exception.get_message());
        }
    }

    r[Common::OEM][Common::RACKSCALE][Swordfish::BOOTABLE] = volume.is_bootable();

    r[Common::OEM][Common::RACKSCALE][Swordfish::ASSIGNED] =
        agent_framework::model::Volume::is_volume_shared_over_fabrics(volume.get_uuid());

    r[Common::LINKS][Common::OEM][Common::RACKSCALE][StorageService::ENDPOINTS] = json::Json::value_t::array;
    for (const auto& endpoint : agent_framework::module::get_manager<agent_framework::model::Endpoint>().get_entries()) {
        for (const auto& entity : endpoint.get_connected_entities()) {
            if (volume.get_uuid() == entity.get_entity()) {
                json::Json link = json::Json();
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
    static const constexpr char TRANSACTION_NAME[] = "PatchVolume";

    auto volume = psme::rest::model::find<agent_framework::model::StorageService, agent_framework::model::Volume>(
        request.params).get();

    const auto& json = JsonValidator::validate_request_body<schema::VolumePatchSchema>(request);

    agent_framework::model::attribute::Attributes attributes{};

    const auto& rackscale_json = json
        .value(constants::Common::OEM, json::Json::object())
        .value(constants::Common::RACKSCALE, json::Json::object());

    if (rackscale_json.count(constants::Swordfish::BOOTABLE)) {
        attributes.set_value(agent_framework::model::literals::Volume::BOOTABLE,
                             rackscale_json[constants::Swordfish::BOOTABLE]);
    }

    const auto& capacity_data = json
        .value(constants::Swordfish::CAPACITY, json::Json::object())
        .value(constants::Data::DATA, json::Json::object());

    if (capacity_data.count(constants::Data::ALLOCATED_BYTES)) {
        attributes.set_value(agent_framework::model::literals::Volume::CAPACITY_BYTES,
                             capacity_data[constants::Data::ALLOCATED_BYTES].get<std::uint64_t>());
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
        gami_agent->execute_in_transaction(TRANSACTION_NAME, set_volume_attributes);
    }
    get(request, response);
}


void Volume::del(const server::Request& request, server::Response& response) {
    static const constexpr char TRANSACTION_NAME[] = "DeleteVolume";

    auto storage_service = psme::rest::model::find<agent_framework::model::StorageService>(request.params).get();

    auto volume = psme::rest::model::find<agent_framework::model::StorageService, agent_framework::model::Volume>(
        request.params).get();

    auto gami_request = agent_framework::model::requests::DeleteVolume(volume.get_uuid());
    const auto& gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(volume.get_agent_id());

    auto completion_notifier = [storage_service, gami_agent](const Uuid task_uuid) {
        {
            auto task = agent_framework::module::get_manager<agent_framework::model::Task>()
                .get_entry_reference(task_uuid);

            if (task->get_state() == agent_framework::model::enums::TaskState::Completed) {
                task->set_messages(task_service_utils::build_success_message());
            }
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

            {
                agent_framework::module::get_manager<agent_framework::model::Task>()
                    .get_entry_reference(task_uuid)->add_completion_notifier(std::bind(completion_notifier, task_uuid));
            }

            std::string task_monitor_url =
                PathBuilder(utils::get_component_url(agent_framework::model::enums::Component::Task, task_uuid))
                    .append(Monitor::MONITOR).build();

            psme::rest::endpoint::utils::set_location_header(request, response, task_monitor_url);
            response.set_body(psme::rest::endpoint::task_service_utils::call_task_get(task_uuid).get_body());
            response.set_status(server::status_2XX::ACCEPTED);
        }
    };

    gami_agent->execute_in_transaction(TRANSACTION_NAME, remove_drive);
}
