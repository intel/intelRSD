/*!
 * @brief Definition of Network Interfaces Collection endpoint
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file network_interface_collection.cpp
 */

#include "psme/rest/endpoints/system/network_interfaces_collection.hpp"
#include "agent-framework/module/compute_components.hpp"

using namespace psme::rest::endpoint;
using namespace psme::rest::constants;
using namespace agent_framework::module;



namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#NetworkInterfaceCollection.NetworkInterfaceCollection";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#NetworkInterfaceCollection.NetworkInterfaceCollection";
    r[Common::NAME] = "Network Interfaces Collection";
    r[Common::DESCRIPTION] = "Collection of Network Interfaces";
    r[Collection::ODATA_COUNT] = json::Json::value_t::null;
    r[Collection::MEMBERS] = json::Json::value_t::array;

    return r;
}
}

NetworkInterfacesCollection::NetworkInterfacesCollection(const std::string& path) : EndpointBase(path) {}
NetworkInterfacesCollection::~NetworkInterfacesCollection() {}

void NetworkInterfacesCollection::get(const server::Request& req, server::Response& res) {
    auto json = ::make_prototype();

    json[Common::ODATA_ID] = PathBuilder(req).build();
    auto system_uuid = psme::rest::model::find<agent_framework::model::System>(req.params).get_uuid();

    auto keys = ComputeComponents::get_instance()->
        get_network_device_manager().get_ids(system_uuid);

    json[Collection::ODATA_COUNT] =
        static_cast<std::uint32_t>(keys.size());
    for (const auto& key : keys) {
        json::Json link_elem(json::Json::value_t::object);
        link_elem[Common::ODATA_ID] = PathBuilder(req).append(key).build();
        json[Collection::MEMBERS].push_back(std::move(link_elem));
    }
    set_response(res, json);
}
