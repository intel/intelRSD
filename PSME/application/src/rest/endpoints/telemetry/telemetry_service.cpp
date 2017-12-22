/*!
 * @brief Telemetry service endpoint
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
 * @file telemetry_service.cpp
 */

#include "psme/rest/endpoints/telemetry/telemetry_service.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;


namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#TelemetryService.TelemetryService";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#TelemetryService.v1_0_0.TelemetryService";
    r[Common::ID] = "TelemetryService";
    r[Common::NAME] = "Telemetry Service";

    r[Common::STATUS][Common::STATE] = "Enabled";
    r[Common::STATUS][Common::HEALTH] = "OK";
    r[Common::STATUS][Common::HEALTH_ROLLUP] = "OK";

    r[TelemetryService::METRIC_DEFINITIONS][Common::ODATA_ID] = endpoint::PathBuilder(PathParam::BASE_URL)
        .append(Root::TELEMETRY_SERVICE)
        .append(TelemetryService::METRIC_DEFINITIONS).build();

    return r;
}
}

endpoint::TelemetryService::TelemetryService(const std::string& path) : EndpointBase(path) {}


endpoint::TelemetryService::~TelemetryService() {}


void endpoint::TelemetryService::get(const server::Request& request, server::Response& response) {
    auto r = make_prototype();
    r[Common::ODATA_ID] = PathBuilder(request).build();

    set_response(response, r);
}
