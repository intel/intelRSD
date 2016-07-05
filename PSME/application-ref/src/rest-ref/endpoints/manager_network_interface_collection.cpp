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

#include "psme/rest-ref/endpoints/manager_network_interface_collection.hpp"
#include "psme/rest-ref/constants/constants.hpp"
#include "psme/rest-ref/endpoints/utils.hpp"
#include "agent-framework/module-ref/model/manager.hpp"

using namespace psme::rest::endpoint;
using namespace app::rest::constants;
using agent_framework::model::enums::ManagerInfoType;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#EthernetInterfaceCollection.EthernetInterfaceCollection";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#EthernetInterfaceCollection.EthernetInterfaceCollection";
    r[Common::NAME] = "Ethernet Network Interface Collection";
    r[Common::DESCRIPTION] = "Collection of Nanager Ethernet Interfaces";
    r[Collection::ODATA_COUNT] = json::Value::Type::NIL;
    r[Collection::MEMBERS] = json::Value::Type::ARRAY;

    return r;
}
}

ManagerNetworkInterfaceCollection::ManagerNetworkInterfaceCollection(const std::string& path) : Endpoint(path) {}
ManagerNetworkInterfaceCollection::~ManagerNetworkInterfaceCollection() {}

void ManagerNetworkInterfaceCollection::get(const server::Request& req, server::Response& res) {
    auto json = ::make_prototype();

    json[Common::ODATA_ID] = PathBuilder(req).build();

    // each manager except for fabric module manager has precisely one EthernetInterface
    auto manager = psme::rest::endpoint::utils::get_manager(req.params[PathParam::MANAGER_ID]);
    auto switch_ids = agent_framework::module::NetworkManager::get_instance()->get_switch_manager().get_ids(manager.get_uuid());

    if(!switch_ids.empty()) {
        json[Collection::ODATA_COUNT] = 0;
    } else {
        json[Collection::ODATA_COUNT] = 1;
        json::Value link_elem(json::Value::Type::OBJECT);
        link_elem[Common::ODATA_ID] = PathBuilder(req).append(1).build();
        json[Collection::MEMBERS].push_back(std::move(link_elem));
    }

    set_response(res, json);
}
