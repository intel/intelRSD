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
#include "psme/rest/endpoints/system/system_metrics.hpp"


using namespace psme::rest;
using namespace psme::rest::constants;
using namespace agent_framework::module;

namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#ComputerSystemMetrics.ComputerSystemMetrics";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#ComputerSystemMetrics.v1_0_0.ComputerSystemMetrics";
    r[Common::ID] = json::Json::value_t::null;
    r[Common::NAME] = "System Metrics";
    r[Common::DESCRIPTION] = "Computer System Metrics";

    r[constants::SystemMetrics::PROCESSOR_BANDWIDTH_PERCENT] = json::Json::value_t::null;
    r[constants::SystemMetrics::IO_BANDWIDTH_GBPS] = json::Json::value_t::null;
    r[constants::SystemMetrics::MEMORY_BANDWIDTH_PERCENT] = json::Json::value_t::null;
    r[constants::SystemMetrics::MEMORY_THROTTLED_CYCLES_PERCENT] = json::Json::value_t::null;
    r[constants::SystemMetrics::MEMORY_POWER_WATT] = json::Json::value_t::null;
    r[constants::SystemMetrics::PROCESSOR_POWER_WATT] = json::Json::value_t::null;
    r[constants::SystemMetrics::HEALTH] = json::Json::value_t::array;

    return r;
}
}


endpoint::SystemMetrics::SystemMetrics(const std::string& path) : EndpointBase(path) {}


endpoint::SystemMetrics::~SystemMetrics() {}


void endpoint::SystemMetrics::get(const server::Request& request, server::Response& response) {
    auto json = ::make_prototype();
    json[Common::ODATA_ID] = PathBuilder(request).build();
    json[Common::ID] = constants::Common::METRICS;
    json[Common::NAME] = "System " + request.params[PathParam::SYSTEM_ID] + " Metrics";

    // check if system exists
    auto system_uuid =
        psme::rest::model::find<agent_framework::model::System>(request.params).get_uuid();

    utils::populate_metrics(json, system_uuid);

    set_response(response, json);
}
