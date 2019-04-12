/*!
 * @brief Definition of Network Interfaces endpoint
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
 * @file network_interface.cpp
 */

#include "psme/rest/endpoints/system/network_interface.hpp"
#include "psme/rest/utils/status_helpers.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;

namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#NetworkInterface.NetworkInterface";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#NetworkInterface.v1_0_0.NetworkInterface";
    r[Common::ID] = json::Json::value_t::null;
    r[Common::NAME] = "Network Interface";
    r[Common::DESCRIPTION] = "Network Interface";
    r[Common::STATUS][Common::STATE] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Json::value_t::null;
    r[NetworkInterface::NETWORK_DEVICE_FUNCTIONS] = json::Json::value_t::null;
    r[Common::LINKS] = json::Json::value_t::object;

    return r;
}

}


endpoint::NetworkInterface::NetworkInterface(const std::string& path) : EndpointBase(path) {}


endpoint::NetworkInterface::~NetworkInterface() {}


void endpoint::NetworkInterface::get(const server::Request& req, server::Response& res) {
    auto json = make_prototype();
    auto device = psme::rest::model::find<agent_framework::model::System, agent_framework::model::NetworkDevice>(
        req.params).get();

    json[Common::ODATA_ID] = PathBuilder(req).build();

    endpoint::status_to_json(device, json);
    json[constants::Common::ID] = req.params[PathParam::NETWORK_INTERFACE_ID];
    json[constants::NetworkInterface::NETWORK_DEVICE_FUNCTIONS][Common::ODATA_ID] = endpoint::PathBuilder(req).append(
        constants::NetworkInterface::NETWORK_DEVICE_FUNCTIONS).build();

    set_response(res, json);
}
