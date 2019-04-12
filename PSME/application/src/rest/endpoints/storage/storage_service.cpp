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
#include "psme/rest/utils/status_helpers.hpp"
#include "psme/rest/endpoints/storage/storage_service.hpp"


using namespace psme::rest;
using namespace psme::rest::constants;
using namespace database;


namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#StorageService.StorageService";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#StorageService.v1_0_0.StorageService";
    r[Common::ID] = json::Json::value_t::null;
    r[Common::NAME] = "Storage Service";
    r[Common::DESCRIPTION] = "Storage Service description";
    r[Common::STATUS][Common::STATE] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Json::value_t::null;

    r[constants::StorageService::ENDPOINTS] = json::Json::value_t::null;
    r[constants::StorageService::VOLUMES] = json::Json::value_t::null;
    r[constants::StorageService::DRIVES] = json::Json::value_t::null;
    r[constants::StorageService::STORAGE_POOLS] = json::Json::value_t::null;

    r[Common::LINKS][constants::StorageService::HOSTING_SYSTEM] = json::Json::value_t::null;
    r[Common::LINKS][Common::OEM][Common::RACKSCALE][Common::ODATA_TYPE] = "#Intel.Oem.StorageServiceLinks";
    r[Common::LINKS][Common::OEM][Common::RACKSCALE][constants::Common::MANAGED_BY] = json::Json::value_t::array;

    r[Common::OEM] = json::Json::value_t::object;

    return r;
}
}

endpoint::StorageService::StorageService(const std::string& path) : EndpointBase(path) {}

endpoint::StorageService::~StorageService() {}

void endpoint::StorageService::get(const server::Request& request, server::Response& response) {
    auto r = ::make_prototype();
    auto storage_service =
            model::find<agent_framework::model::StorageService>(request.params).get();

    r[constants::Common::ID] = request.params[PathParam::SERVICE_ID];
    r[constants::Common::ODATA_ID] = PathBuilder(request).build();

    endpoint::status_to_json(storage_service, r);

    const Uuid& manager_uuid = storage_service.get_parent_uuid();
    auto fabrics = agent_framework::module::get_manager<agent_framework::model::Fabric>().get_entries(manager_uuid);
    if (!fabrics.empty()) {
        r[constants::StorageService::ENDPOINTS][constants::Common::ODATA_ID] =
                endpoint::PathBuilder(PathParam::BASE_URL)
                        .append(Root::FABRICS)
                        .append(fabrics.front().get_id())
                        .append(constants::Fabric::ENDPOINTS)
                        .build();
    }

    const auto systems_uuid = agent_framework::module::get_manager<agent_framework::model::System>().get_keys();
    if (1 <= systems_uuid.size()) {
        const auto& system_uuid = systems_uuid.front();
        r[Common::LINKS][constants::StorageService::HOSTING_SYSTEM][constants::Common::ODATA_ID] =
            PathBuilder(endpoint::utils::get_component_url(agent_framework::model::enums::Component::System,
                                                           system_uuid)).build();
    }

    json::Json manager = json::Json();
    manager[Common::ODATA_ID] =
            utils::get_component_url(agent_framework::model::enums::Component::Manager,
                                     storage_service.get_parent_uuid());
    r[Common::LINKS][Common::OEM][Common::RACKSCALE][constants::Common::MANAGED_BY].push_back(std::move(manager));

    r[constants::StorageService::DRIVES][constants::Common::ODATA_ID] =
        PathBuilder(request).append(constants::Chassis::DRIVES).build();

    r[constants::StorageService::STORAGE_POOLS][constants::Common::ODATA_ID] =
        PathBuilder(request).append(constants::StorageService::STORAGE_POOLS).build();

    r[constants::StorageService::VOLUMES][constants::Common::ODATA_ID] =
        PathBuilder(request).append(constants::StorageService::VOLUMES).build();

    set_response(response, r);
}
