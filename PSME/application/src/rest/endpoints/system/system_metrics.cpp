/*!
 * @copyright
 * Copyright (c) 2017-2018 Intel Corporation
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
#include "psme/rest/endpoints/system/system_metrics.hpp"


using namespace psme::rest;
using namespace psme::rest::constants;
using namespace agent_framework::module;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#ComputerSystemMetrics.ComputerSystemMetrics";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#ComputerSystemMetrics.v1_0_0.ComputerSystemMetrics";
    r[Common::ID] = json::Value::Type::NIL;
    r[Common::NAME] = "System Metrics";
    r[Common::DESCRIPTION] = "Computer System Metrics";

    r[constants::SystemMetrics::PROCESSOR_BANDWIDTH_PERCENT] = json::Value::Type::NIL;
    r[constants::SystemMetrics::IO_BANDWIDTH_GBPS] = json::Value::Type::NIL;
    r[constants::SystemMetrics::MEMORY_BANDWIDTH_PERCENT] = json::Value::Type::NIL;
    r[constants::SystemMetrics::MEMORY_THROTTLED_CYCLES_PERCENT] = json::Value::Type::NIL;
    r[constants::SystemMetrics::MEMORY_POWER_WATT] = json::Value::Type::NIL;
    r[constants::SystemMetrics::PROCESSOR_POWER_WATT] = json::Value::Type::NIL;
    r[constants::SystemMetrics::HEALTH] = json::Value::Type::ARRAY;

    return r;
}
}


endpoint::SystemMetrics::SystemMetrics(const std::string& path) : EndpointBase(path) {}


endpoint::SystemMetrics::~SystemMetrics() {}


void endpoint::SystemMetrics::get(const server::Request& request, server::Response& response) {
    auto json = ::make_prototype();
    json[Common::ODATA_ID] = PathBuilder(request).build();
    json[Common::ID] = "System " + request.params[PathParam::SYSTEM_ID] + " Metrics";

    // check if system exists
    auto system_uuid =
        psme::rest::model::Find<agent_framework::model::System>(request.params[PathParam::SYSTEM_ID]).get_uuid();

    utils::populate_metrics(json, system_uuid);

    set_response(response, json);
}
