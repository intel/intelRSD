/*!
 * @brief Definition of Network Interfaces endpoint
 *
 * @copyright Copyright (c) 2019 Intel Corporation
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
 * @file network_adapter.cpp
 */

#include "psme/rest/endpoints/chassis/network_adapter.hpp"
#include "psme/rest/utils/status_helpers.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;

namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#NetworkAdapter.NetworkAdapter";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#NetworkAdapter.v1_0_0.NetworkAdapter";
    r[Common::ID] = json::Json::value_t::null;
    r[Common::NAME] = "Network Adapter";
    r[Common::DESCRIPTION] = "Network Adapter";
    r[Common::STATUS][Common::STATE] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Json::value_t::null;
    r[NetworkInterface::NETWORK_DEVICE_FUNCTIONS] = json::Json::value_t::null;
    r[Common::LINKS] = json::Json::value_t::object;

    return r;
}

}


endpoint::NetworkAdapter::NetworkAdapter(const std::string& path) : EndpointBase(path) {}


endpoint::NetworkAdapter::~NetworkAdapter() {}


void endpoint::NetworkAdapter::get(const server::Request& req, server::Response& res) {
    auto json = make_prototype();

    psme::rest::server::Parameters request_parameters;
    auto chassis = psme::rest::model::find<agent_framework::model::Chassis>(req.params).get();
    auto &system_manager = agent_framework::module::get_manager<agent_framework::model::System>();
    auto system_uuids = system_manager.get_keys();
    for (const auto& system_uuid : system_uuids) {
        auto system = system_manager.get_entry(system_uuid);
        if (system.get_chassis() == chassis.get_uuid()) {
            request_parameters.set(PathParam::SYSTEM_ID, std::to_string(system_manager.uuid_to_rest_id(system_uuid)));
        }
    }

    request_parameters.set(PathParam::NETWORK_INTERFACE_ID, req.params[PathParam::NETWORK_ADAPTER_ID]);
    auto device = psme::rest::model::find<agent_framework::model::System, agent_framework::model::NetworkDevice>(
            request_parameters).get();

    json[Common::ODATA_ID] = PathBuilder(req).build();

    endpoint::status_to_json(device, json);
    json[Common::ID] = req.params[PathParam::NETWORK_ADAPTER_ID];
    json[NetworkInterface::NETWORK_DEVICE_FUNCTIONS][Common::ODATA_ID] = endpoint::PathBuilder(req)
            .append(NetworkInterface::NETWORK_DEVICE_FUNCTIONS).build();

    set_response(res, json);
}
