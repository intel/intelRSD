/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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

#include "psme/rest-ref/endpoints/storage_adapter.hpp"
#include "psme/rest-ref/constants/constants.hpp"
#include "agent-framework/module-ref/compute_manager.hpp"
#include <regex>

using namespace psme::rest;
using namespace app::rest::constants;
using namespace app::rest;



namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Systems/Members/__SYSTEM_ID__/Adapters/Members/__ADAPTER_ID__/$entity";
    r[Common::ODATA_ID] = json::Value::Type::NIL;;
    r[Common::ODATA_TYPE] = "#Adapter.1.0.0.Adapter";
    r[Common::ID] = json::Value::Type::NIL;;
    r[Common::NAME] = "Storage Adapter";
    r[Common::DESCRIPTION] = "Storage Adapter description";
    r[Common::MODEL] = json::Value::Type::NIL;
    r[Common::MANUFACTURER] = json::Value::Type::NIL;
    r[StorageAdapter::INTERFACE] = json::Value::Type::NIL;
    r[StorageAdapter::BUS_INFO] = json::Value::Type::NIL;
    r[Common::STATUS][Common::STATE] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Value::Type::NIL;
    r[StorageAdapter::DEVICES] = json::Value::Type::NIL;
    r[Common::LINKS][Common::CONTAINED_BY] = json::Value::Type::NIL;
    r[Common::LINKS][Common::OEM] = json::Value::Type::OBJECT;

    return r;
}

}

endpoint::StorageAdapter::StorageAdapter(const std::string& path) : Endpoint(path) {}
endpoint::StorageAdapter::~StorageAdapter() {}

void endpoint::StorageAdapter::get(const server::Request& req, server::Response& res) {
    auto r = make_prototype();
    auto system_id = psme::rest::model::Find<agent_framework::model::System>(req.params[PathParam::SYSTEM_ID])
        .get_one()->get_id();
    auto sc =
        psme::rest::model::Find<agent_framework::model::StorageController>(req.params[PathParam::ADAPTER_ID]).
        via<agent_framework::model::System>(req.params[PathParam::SYSTEM_ID]).get();

    endpoint::utils::status_to_json(sc,r);

    r[constants::Common::ODATA_ID] = PathBuilder(req).build();
    r[Common::ODATA_CONTEXT] = std::regex_replace(r[Common::ODATA_CONTEXT].as_string(),
                                                  std::regex("__SYSTEM_ID__"), std::to_string(system_id));
    r[Common::ODATA_CONTEXT] = std::regex_replace(r[Common::ODATA_CONTEXT].as_string(),
                                                  std::regex("__ADAPTER_ID__"), std::to_string(sc.get_id()));
    r[constants::Common::ID] = req.params[PathParam::ADAPTER_ID];

    r[constants::StorageAdapter::DEVICES][Common::ODATA_ID] =
            PathBuilder(req).append(constants::StorageAdapter::DEVICES).build();

    r[Common::STATUS][Common::HEALTH_ROLLUP] =
        psme::rest::endpoint::utils::rollup_health
            <agent_framework::model::StorageController,
                agent_framework::model::Drive> (sc).to_string();


    r[constants::StorageAdapter::BUS_INFO] = sc.get_physical_id();
    r[Common::MANUFACTURER] = sc.get_fru_info().get_manufacturer();
    r[constants::StorageAdapter::INTERFACE] = sc.get_interface();

    r[Common::LINKS]
     [Common::CONTAINED_BY]
     [Common::ODATA_ID] = PathBuilder(PathParam::BASE_URL)
                            .append(Root::SYSTEMS)
                            .append(req.params[PathParam::SYSTEM_ID]).build();

     set_response(res, r);
}
