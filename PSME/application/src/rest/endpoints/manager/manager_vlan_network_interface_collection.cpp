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
#include "psme/rest/endpoints/manager/manager_vlan_network_interface_collection.hpp"



using namespace psme::rest;
using namespace psme::rest::endpoint;
using namespace psme::rest::constants;

namespace {

json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#VLanNetworkInterfaceCollection.VLanNetworkInterfaceCollection";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#VLanNetworkInterfaceCollection.VLanNetworkInterfaceCollection";
    r[Common::NAME] = "VLAN Network Interface Collection";
    r[Common::DESCRIPTION] = "Collection of VLAN Network Interfaces";
    r[Collection::ODATA_COUNT] = json::Json::value_t::null;
    r[Collection::MEMBERS] = json::Json::value_t::array;

    return r;
}

}


ManagerVlanNetworkInterfaceCollection::ManagerVlanNetworkInterfaceCollection(const std::string& path) :
    EndpointBase(path) {}


ManagerVlanNetworkInterfaceCollection::~ManagerVlanNetworkInterfaceCollection() {}


void ManagerVlanNetworkInterfaceCollection::get(const server::Request& request, server::Response& response) {
    auto r = ::make_prototype();

    r[Common::ODATA_ID] = PathBuilder(request).build();
    r[Collection::ODATA_COUNT] = 0;

    auto manager_uuid = psme::rest::model::find<agent_framework::model::Manager>(request.params).get_uuid();
    auto nic_ids = agent_framework::module::get_manager<agent_framework::model::NetworkInterface>()
        .get_ids(manager_uuid);

    if (!nic_ids.empty()) {
        auto nic_uuid =
            psme::rest::model::find<agent_framework::model::Manager,
                                    agent_framework::model::NetworkInterface>(request.params).get_uuid();

        auto vlan_ids = agent_framework::module::get_manager<agent_framework::model::EthernetSwitchPortVlan>()
            .get_ids(nic_uuid);

        r[Collection::ODATA_COUNT] = vlan_ids.size();
        for (const auto& vlan_id : vlan_ids) {
            json::Json link(json::Json::value_t::object);
            link[Common::ODATA_ID] = PathBuilder(request).append(vlan_id).build();
            r[Collection::MEMBERS].push_back(std::move(link));
        }
    }

    set_response(response, r);
}
