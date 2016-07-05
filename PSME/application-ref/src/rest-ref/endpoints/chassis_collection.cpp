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

#include "psme/rest-ref/endpoints/utils.hpp"
#include "psme/rest-ref/constants/constants.hpp"
#include "psme/rest-ref/endpoints/chassis_collection.hpp"

using namespace psme::rest::endpoint;
using namespace app::rest::constants;



namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Chassis";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#ChassisCollection.ChassisCollection";
    r[Common::NAME] = "Chassis Collection";
    r[Common::DESCRIPTION] = "Collection of Chassis";
    r[Collection::ODATA_COUNT] = json::Value::Type::NIL;
    r[Collection::MEMBERS] = json::Value::Type::ARRAY;

    return r;
}
}

ChassisCollection::ChassisCollection(const std::string& path) : Endpoint(path) {}
ChassisCollection::~ChassisCollection() {}

void ChassisCollection::get(const server::Request& req, server::Response& res) {
    auto json = ::make_prototype();

    json[Common::ODATA_ID] = PathBuilder(req).build();

    auto chassis_ids = agent_framework::module::ComputeManager::get_instance()
        ->get_chassis_manager().get_ids();
    auto network_chassis_ids =
        agent_framework::module::NetworkManager::get_instance()->
        get_chassis_manager().get_ids();
    chassis_ids.insert(chassis_ids.end(), network_chassis_ids.begin(),
        network_chassis_ids.end());
    auto chassis_chassis_ids =
        agent_framework::module::ChassisManager::get_instance()->
        get_chassis_manager().get_ids();
    chassis_ids.insert(chassis_ids.end(), chassis_chassis_ids.begin(),
        chassis_chassis_ids.end());

    json[Collection::ODATA_COUNT] = std::uint32_t(chassis_ids.size());

    for (const auto& id : chassis_ids) {
        json::Value link;
        link[Common::ODATA_ID] = PathBuilder(req).append(id).build();
        json[Collection::MEMBERS].push_back(std::move(link));
    }

    set_response(res, json);
}
