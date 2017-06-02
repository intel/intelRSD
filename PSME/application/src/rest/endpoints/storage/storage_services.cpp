/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
#include "psme/rest/endpoints/storage/storage_services.hpp"

using namespace psme::rest;
using namespace psme::rest::constants;
using namespace agent_framework::model;



namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Services/Members/__SERVICE_ID__/$entity";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#StorageService.v1_0_0.StorageService";
    r[Common::ID] = json::Value::Type::NIL;
    r[Common::NAME] = "Storage Service";
    r[Common::DESCRIPTION] = "Storage Service description";
    r[Common::STATUS][Common::STATE] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Value::Type::NIL;
    r[Common::OEM] = json::Value::Type::OBJECT;
    r[constants::StorageService::REMOTE_TARGETS] = json::Value::Type::NIL;
    r[constants::StorageService::LOGICAL_DRIVES] = json::Value::Type::NIL;
    r[constants::StorageService::DRIVES] = json::Value::Type::NIL;
    r[Common::LINKS][constants::Common::MANAGED_BY] = json::Value::Type::ARRAY;
    r[Common::LINKS][Common::OEM] = json::Value::Type::OBJECT;
    return r;
}
}

endpoint::StorageService::StorageService(const std::string& path) : EndpointBase(path) {}
endpoint::StorageService::~StorageService() {}

void endpoint::StorageService::get(const server::Request& req, server::Response& res) {
    auto r = ::make_prototype();

    r[constants::Common::ODATA_ID] = PathBuilder(req).build();
    auto service =  psme::rest::model::Find<agent_framework::model::StorageServices>(req.params[PathParam::SERVICE_ID]).get();
    r[Common::ODATA_CONTEXT] = std::regex_replace(r[Common::ODATA_CONTEXT].as_string(),
                                                  std::regex("__SERVICE_ID__"), std::to_string(service.get_id()));
    r[constants::Common::ID] = req.params[PathParam::SERVICE_ID];

    endpoint::status_to_json(service, r);
    r[Common::STATUS][Common::HEALTH_ROLLUP] =
            endpoint::HealthRollup<agent_framework::model::StorageServices>().get(service.get_uuid());

    r[constants::StorageService::REMOTE_TARGETS]
     [constants::Common::ODATA_ID] = PathBuilder(req).append(constants::StorageService::TARGETS).build();

    r[constants::StorageService::LOGICAL_DRIVES]
     [constants::Common::ODATA_ID] = PathBuilder(req).append(constants::StorageService::LOGICAL_DRIVES).build();

    r[constants::StorageService::DRIVES]
     [constants::Common::ODATA_ID] = PathBuilder(req).append(constants::StorageService::DRIVES).build();

    json::Value manager;
    manager[Common::ODATA_ID] =
        utils::get_component_url(agent_framework::model::enums::Component::Manager, service.get_parent_uuid());

    r[Common::LINKS]
     [Common::MANAGED_BY]
     [Collection::MEMBERS].push_back(std::move(manager));

    set_response(res, r);
}
