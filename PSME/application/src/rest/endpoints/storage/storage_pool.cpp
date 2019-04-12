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

#include "agent-framework/module/requests/storage/delete_storage_pool.hpp"
#include "agent-framework/module/responses/storage/delete_storage_pool.hpp"
#include "psme/rest/endpoints/storage/storage_pool.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/utils/status_helpers.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::validators;

namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#StoragePool.StoragePool";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#StoragePool.v1_1_1.StoragePool";
    r[Common::ID] = json::Json::value_t::null;
    r[Common::NAME] = "StoragePool";
    r[Common::DESCRIPTION] = "Storage Pool description";

    r[Common::STATUS][Common::STATE] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Json::value_t::null;

    r[Swordfish::IDENTIFIER] = json::Json::value_t::null;
    r[Swordfish::BLOCK_SIZE_BYTES] = json::Json::value_t::null;

    r[Swordfish::ALLOCATED_VOLUMES] = json::Json::value_t::object;
    r[Swordfish::ALLOCATED_POOLS] = json::Json::value_t::object;

    r[Swordfish::CAPACITY][Data::DATA][Data::ALLOCATED_BYTES] = json::Json::value_t::null;
    r[Swordfish::CAPACITY][Data::DATA][Data::CONSUMED_BYTES] = json::Json::value_t::null;
    r[Swordfish::CAPACITY_SOURCES] = json::Json::value_t::array;

    r[Common::OEM] = json::Json::value_t::object;

    return r;
}
}

endpoint::StoragePool::StoragePool(const std::string& path) : EndpointBase(path) {}


endpoint::StoragePool::~StoragePool() {}


void endpoint::StoragePool::get(const server::Request& req, server::Response& res) {
    auto r = ::make_prototype();

    auto storage_pool =
        model::find<agent_framework::model::StorageService, agent_framework::model::StoragePool>(req.params).get();

    r[constants::Common::ODATA_ID] = PathBuilder(req).build();
    r[constants::Common::ID] = req.params[PathParam::STORAGE_POOL_ID];
    r[Swordfish::BLOCK_SIZE_BYTES] = storage_pool.get_block_size_bytes();

    endpoint::status_to_json(storage_pool, r);
    utils::fill_name_and_description(storage_pool, r);

    if (!storage_pool.get_identifiers().empty()) {
        json::Json id = json::Json();
        const auto& identifier = storage_pool.get_identifiers().front();
        id[Common::DURABLE_NAME] = identifier.get_durable_name();
        id[Common::DURABLE_NAME_FORMAT] = identifier.get_durable_name_format();
        r[Swordfish::IDENTIFIER] = std::move(id);
    }

    r[Swordfish::CAPACITY][Data::DATA][Data::ALLOCATED_BYTES] = storage_pool.get_capacity().get_allocated_bytes();
    r[Swordfish::CAPACITY][Data::DATA][Data::CONSUMED_BYTES] = storage_pool.get_capacity().get_consumed_bytes();
    r[Swordfish::CAPACITY][Data::DATA][Data::GUARANTEED_BYTES] = storage_pool.get_capacity().get_guaranteed_bytes();

    for (std::size_t i = 0; i < storage_pool.get_capacity_sources().size(); i++) {
        json::Json cs = json::Json();

        cs[Common::ODATA_ID] = endpoint::PathBuilder(req)
            .append(constants::Swordfish::CAPACITY_SOURCES)
            .append(i + 1)
            .build();

        r[Swordfish::CAPACITY_SOURCES].push_back(std::move(cs));
    }

    r[Swordfish::ALLOCATED_VOLUMES][Common::ODATA_ID] = endpoint::PathBuilder(req)
        .append(constants::Swordfish::ALLOCATED_VOLUMES).build();
    r[Swordfish::ALLOCATED_POOLS][Common::ODATA_ID] = endpoint::PathBuilder(req)
        .append(constants::Swordfish::ALLOCATED_POOLS).build();

    set_response(res, r);
}
void endpoint::StoragePool::del(const server::Request& req, server::Response& res) {
    static const constexpr char TRANSACTION_NAME[] = "DeleteStoragePool";

    const auto storage_pool = psme::rest::model::find<agent_framework::model::StorageService, agent_framework::model::StoragePool>(
        req.params).get();

    auto gami_req = agent_framework::model::requests::DeleteStoragePool(storage_pool.get_uuid());

    const auto& gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(storage_pool.get_agent_id());

    auto remove_storage_pool = [&, gami_agent] {
        // try removing storage pool from agent's model
        gami_agent->execute<agent_framework::model::responses::DeleteStoragePool>(gami_req);

        // remove storage pool from REST model, DO NOT use storage pool reference after this line!
        psme::rest::model::handler::HandlerManager::get_instance()->get_handler(
            agent_framework::model::enums::Component::StoragePool)->
            remove(storage_pool.get_uuid());

        res.set_status(server::status_2XX::NO_CONTENT);
    };

    gami_agent->execute_in_transaction(TRANSACTION_NAME, remove_storage_pool);

}



