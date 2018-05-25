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

#include "psme/rest/endpoints/storage/storage_pool.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/utils/status_helpers.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::validators;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#StoragePool.StoragePool";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#StoragePool.v1_0_0.StoragePool";
    r[Common::ID] = json::Value::Type::NIL;
    r[Common::NAME] = "StoragePool";
    r[Common::DESCRIPTION] = "Storage Pool description";

    r[Common::STATUS][Common::STATE] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Value::Type::NIL;

    r[Swordfish::IDENTIFIER] = json::Value::Type::NIL;
    r[Swordfish::BLOCK_SIZE_BYTES] = json::Value::Type::NIL;

    r[Swordfish::ALLOCATED_VOLUMES] = json::Value::Type::OBJECT;
    r[Swordfish::ALLOCATED_POOLS] = json::Value::Type::OBJECT;

    r[Swordfish::CAPACITY][Data::DATA][Data::ALLOCATED_BYTES] = json::Value::Type::NIL;
    r[Swordfish::CAPACITY][Data::DATA][Data::CONSUMED_BYTES] = json::Value::Type::NIL;
    r[Swordfish::CAPACITY_SOURCES] = json::Value::Type::ARRAY;

    r[Common::OEM] = json::Value::Type::OBJECT;

    return r;
}

}


endpoint::StoragePool::StoragePool(const std::string& path) : EndpointBase(path) {}


endpoint::StoragePool::~StoragePool() {}


void endpoint::StoragePool::get(const server::Request& req, server::Response& res) {
    auto r = ::make_prototype();

    auto storage_pool =
        psme::rest::model::Find<agent_framework::model::StoragePool>(req.params[PathParam::STORAGE_POOL_ID])
            .via<agent_framework::model::StorageService>(req.params[PathParam::SERVICE_ID]).get();

    r[constants::Common::ODATA_ID] = PathBuilder(req).build();
    r[constants::Common::ID] = req.params[PathParam::STORAGE_POOL_ID];
    r[Swordfish::BLOCK_SIZE_BYTES] = storage_pool.get_block_size_bytes();

    endpoint::status_to_json(storage_pool, r);

    if (!storage_pool.get_identifiers().empty()) {
        json::Value id{};
        const auto& identifier = storage_pool.get_identifiers().front();
        id[Common::DURABLE_NAME] = identifier.get_durable_name();
        id[Common::DURABLE_NAME_FORMAT] = identifier.get_durable_name_format();
        r[Swordfish::IDENTIFIER] = std::move(id);
    }

    r[Swordfish::CAPACITY][Data::DATA][Data::ALLOCATED_BYTES] = storage_pool.get_capacity().get_allocated_bytes();
    r[Swordfish::CAPACITY][Data::DATA][Data::CONSUMED_BYTES] = storage_pool.get_capacity().get_consumed_bytes();
    r[Swordfish::CAPACITY][Data::DATA][Data::GUARANTEED_BYTES] = storage_pool.get_capacity().get_guaranteed_bytes();

    for (const auto& capacity_source : storage_pool.get_capacity_sources()) {
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
        for (const auto& volume : capacity_source.get_providing_volumes()) {
            auto volume_url = utils::get_component_url(agent_framework::model::enums::Component::Volume, volume);
            json::Value link{};
            link[Common::ODATA_ID] = std::move(volume_url);
            cs[Swordfish::PROVIDING_VOLUMES].push_back(std::move(link));
        }

        r[Swordfish::CAPACITY_SOURCES].push_back(std::move(cs));
    }

    r[Swordfish::ALLOCATED_VOLUMES][Common::ODATA_ID] = endpoint::PathBuilder(req)
        .append(constants::Swordfish::ALLOCATED_VOLUMES).build();
    r[Swordfish::ALLOCATED_POOLS][Common::ODATA_ID] = endpoint::PathBuilder(req)
            .append(constants::Swordfish::ALLOCATED_POOLS).build();

    set_response(res, r);
}



