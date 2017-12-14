/*!
 * @brief Port metrics endpoint
 *
 * @header{License}
 * @copyright Copyright (c) 2017 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @header{Filesystem}
 * @file port_metrics.cpp
 */

#include "psme/rest/endpoints/fabric/port_metrics.hpp"

using namespace psme::rest;
using namespace psme::rest::constants;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#PortMetrics.PortMetrics";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#PortMetrics.v1_0_0.PortMetrics";
    r[Common::ID] = json::Value::Type::NIL;
    r[Common::NAME] = "Port Metrics";
    r[Common::DESCRIPTION] = "Fabric Port Metrics";

    r[constants::Common::HEALTH] = json::Value::Type::NIL;

    return r;
}
}


endpoint::PortMetrics::PortMetrics(const std::string& path) : EndpointBase(path) {}


endpoint::PortMetrics::~PortMetrics() {}


void endpoint::PortMetrics::get(const server::Request& request, server::Response& response) {
    auto port_uuid = psme::rest::model::Find<agent_framework::model::Port>(request.params[PathParam::PORT_ID])
        .via<agent_framework::model::Fabric>(request.params[PathParam::FABRIC_ID])
        .via<agent_framework::model::Switch>(request.params[PathParam::SWITCH_ID]).get_uuid();

    auto json = ::make_prototype();
    json[Common::ODATA_ID] = PathBuilder(request).build();
    json[Common::ID] = "Port " + request.params[PathParam::PORT_ID] + " Metrics";

    utils::populate_metrics(json, port_uuid);

    set_response(response, json);
}
