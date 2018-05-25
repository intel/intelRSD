/*!
 * @brief Volume metrics endpoint
 *
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation.
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
 * @file volume_metrics.cpp
 */

#include "psme/rest/endpoints/storage/volume_metrics.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#VolumeMetrics.VolumeMetrics";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#VolumeMetrics.v1_0_0.VolumeMetrics";
    r[Common::ID] = json::Value::Type::NIL;
    r[Common::NAME] = "Volume Metrics";
    r[Common::DESCRIPTION] = "Metrics for Volume";

    r[Swordfish::CAPACITY_USED_BYTES] = json::Value::Type::NIL;

    return r;
}
}


endpoint::VolumeMetrics::VolumeMetrics(const std::string& path) : EndpointBase(path) {}


endpoint::VolumeMetrics::~VolumeMetrics() {}


void endpoint::VolumeMetrics::get(const server::Request& req, server::Response& res) {

    auto volume = psme::rest::model::Find<agent_framework::model::Volume>(req.params[PathParam::VOLUME_ID])
        .via<agent_framework::model::StorageService>(req.params[PathParam::SERVICE_ID]).get();

    auto json = ::make_prototype();
    json[Common::ODATA_ID] = PathBuilder(req).build();
    json[Common::ID] = "Volume " + req.params[PathParam::VOLUME_ID] + " Metrics";

    set_response(res, json);
}
