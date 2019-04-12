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
 *
 * @file storage_pool_collection.cpp
 * */

#include "psme/rest/validators/schemas/storage_pool.hpp"
#include "psme/rest/constants/constants.hpp"
#include "psme/rest/endpoints/storage/storage_pool_collection.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/model/handlers/handler_manager.hpp"
#include "psme/rest/model/handlers/generic_handler_deps.hpp"
#include "agent-framework/module/requests/storage.hpp"
#include "agent-framework/module/responses/storage.hpp"
#include "psme/rest/server/multiplexer.hpp"
#include "psme/rest/model/handlers/generic_handler.hpp"



using namespace psme::rest;
using namespace psme::rest::model;
using namespace psme::rest::endpoint;
using namespace psme::rest::validators;
using namespace psme::rest::constants;

using namespace agent_framework::module;

namespace {

using namespace agent_framework::model;


json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#StoragePools";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#StoragePoolCollection.StoragePoolCollection";
    r[Common::NAME] = "StoragePools Collection";
    r[Common::DESCRIPTION] = "Collection of Storage Pools";
    r[Collection::ODATA_COUNT] = json::Json::value_t::null;
    r[Collection::MEMBERS] = json::Json::value_t::array;

    return r;
}


attribute::Array<attribute::CapacitySource> read_capacity_sources(const json::Json& json) {
    attribute::Array<attribute::CapacitySource> result{};
    const auto& array_capacity_sources = json[Swordfish::CAPACITY_SOURCES];
    for (const auto& json_capacity_source : array_capacity_sources) {
        attribute::CapacitySource capacity_source{};
        const auto& array_providing_drives = json_capacity_source[Swordfish::PROVIDING_DRIVES];
        for (const auto& json_providing_drive : array_providing_drives) {
            const auto& providing_drive_url = json_providing_drive[Common::ODATA_ID].get<std::string>();

            auto params = server::Multiplexer::get_instance()->get_params(providing_drive_url,
                                                                          constants::Routes::DRIVE_PATH);

            auto drive_uuid = model::try_find<agent_framework::model::Chassis, agent_framework::model::Drive>(
                params).get_uuid();

            if (drive_uuid.has_value()) {
                capacity_source.add_providing_drive(std::move(drive_uuid));
            }
            else {
                log_error("rest", "Cannot find drive: " << providing_drive_url);
                throw agent_framework::exceptions::InvalidValue("Cannot find drive: " + providing_drive_url);
            }
        }
        result.add_entry(std::move(capacity_source));
    }
    return result;
}

}


StoragePoolCollection::StoragePoolCollection(const std::string& path) : EndpointBase(path) {}


StoragePoolCollection::~StoragePoolCollection() {}


void StoragePoolCollection::get(const server::Request& req, server::Response& res) {
    auto json = ::make_prototype();

    json[Common::ODATA_ID] = PathBuilder(req).build();

    auto storage_service_uuid = model::find<agent_framework::model::StorageService>(req.params).get_uuid();

    auto keys = agent_framework::module::get_manager<agent_framework::model::StoragePool>().get_ids(
        storage_service_uuid);

    json[Collection::ODATA_COUNT] = static_cast<std::uint32_t>(keys.size());
    for (const auto& key : keys) {
        json::Json link(json::Json::value_t::object);
        link[Common::ODATA_ID] = PathBuilder(req).append(key).build();
        json[Collection::MEMBERS].push_back(std::move(link));
    }

    set_response(res, json);
}


void StoragePoolCollection::post(const server::Request& request, server::Response& response) {
    static const constexpr char TRANSACTION_NAME[] = "PostStoragePoolCollection";
    auto json = JsonValidator::validate_request_body<schema::StoragePoolPostSchema>(request);

    auto parent_service = psme::rest::model::find<agent_framework::model::StorageService>(request.params).get();

    agent_framework::model::requests::AddStoragePool add_storage_pool_request{
        json[Swordfish::BLOCK_SIZE_BYTES].get<std::uint64_t>(),
        ::read_capacity_sources(json)
    };

    auto agent_id = parent_service.get_agent_id();
    auto gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(agent_id);
    auto add_storage_pool = [&, gami_agent] {
        auto add_storage_pool_response = gami_agent->
            execute<agent_framework::model::responses::AddStoragePool>(add_storage_pool_request);

        model::handler::HandlerManager::get_instance()->
            get_handler(agent_framework::model::enums::Component::StoragePool)->
            load(gami_agent,
                 parent_service.get_uuid(), agent_framework::model::enums::Component::StorageService,
                 add_storage_pool_response.get_storage_pool(), false);

        const auto& created_storage_pool = agent_framework::module::get_manager<agent_framework::model::StoragePool>().get_entry(
            add_storage_pool_response.get_storage_pool());

        utils::set_location_header(request, response,
                                   PathBuilder(request).append(created_storage_pool.get_id()).build());
        response.set_status(server::status_2XX::CREATED);
    };

    gami_agent->execute_in_transaction(TRANSACTION_NAME, add_storage_pool);
}