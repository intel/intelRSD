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

#include "agent-framework/module/network_components.hpp"
#include "agent-framework/module/model/manager.hpp"
#include "psme/rest/constants/constants.hpp"
#include "psme/rest/endpoints/utils.hpp"
#include "psme/rest/endpoints/manager/manager_network_interface_collection.hpp"



using namespace psme::rest;
using namespace psme::rest::endpoint;
using namespace psme::rest::constants;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#EthernetInterfaceCollection.EthernetInterfaceCollection";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#EthernetInterfaceCollection.EthernetInterfaceCollection";
    r[Common::NAME] = "Ethernet Network Interface Collection";
    r[Common::DESCRIPTION] = "Collection of Manager Ethernet Interfaces";
    r[Collection::ODATA_COUNT] = json::Value::Type::NIL;
    r[Collection::MEMBERS] = json::Value::Type::ARRAY;

    return r;
}
}


ManagerNetworkInterfaceCollection::ManagerNetworkInterfaceCollection(const std::string& path) : EndpointBase(path) {}


ManagerNetworkInterfaceCollection::~ManagerNetworkInterfaceCollection() {}


void ManagerNetworkInterfaceCollection::get(const server::Request& request, server::Response& response) {
    auto json = ::make_prototype();

    json[Common::ODATA_ID] = PathBuilder(request).build();

    auto manager_uuid = psme::rest::model::Find<agent_framework::model::Manager>(request.params[PathParam::MANAGER_ID])
        .get_uuid();
    auto nic_ids = agent_framework::module::get_manager<agent_framework::model::NetworkInterface>()
        .get_ids(manager_uuid);

    if (!nic_ids.empty()) {
        // Managers can have Ethernet Interfaces as a collection of ManagerNetworkInterface
        json[Collection::ODATA_COUNT] = nic_ids.size();

        for (const auto& nic_id : nic_ids) {
            json::Value link(json::Value::Type::OBJECT);
            link[Common::ODATA_ID] = PathBuilder(request).append(nic_id).build();
            json[Collection::MEMBERS].push_back(std::move(link));
        }
    }
    else {
        // If not - Manager has precisely one EthernetInterface
        json[Collection::ODATA_COUNT] = 1;
        json::Value link(json::Value::Type::OBJECT);
        link[Common::ODATA_ID] = PathBuilder(request).append(1).build();
        json[Collection::MEMBERS].push_back(std::move(link));
    }

    set_response(response, json);
}
