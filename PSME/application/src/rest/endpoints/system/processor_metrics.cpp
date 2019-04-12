/*!
 * @brief ProcessorMetrics endpoint implementation
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file processor_metrics.cpp
 */

#include "psme/rest/endpoints/system/processor_metrics.hpp"
#include "psme/rest/endpoints/utils.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;

namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Intel_RackScale.ProcessorMetrics.ProcessorMetrics";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#Intel_RackScale.ProcessorMetrics.v1_0_0.ProcessorMetrics";
    r[Common::ID] = json::Json::value_t::null;
    r[Common::NAME] = "Processor Metrics";
    r[Common::DESCRIPTION] = "Processor Metrics";
    r[ProcessorMetrics::AVERAGE_FREQUENCY_MHZ] = json::Json::value_t::null;
    r[ProcessorMetrics::BANDWIDTH_PERCENT] = json::Json::value_t::null;
    r[ProcessorMetrics::CONSUMED_POWER_WATT] = json::Json::value_t::null;
    r[ProcessorMetrics::HEALTH] = json::Json::value_t::array;
    r[ProcessorMetrics::TEMPERATURE_CELSIUS] = json::Json::value_t::null;
    r[ProcessorMetrics::THROTTLING_CELSIUS] = json::Json::value_t::null;

    return r;
}

}


endpoint::ProcessorMetrics::ProcessorMetrics(const std::string& path) : EndpointBase(path) {}


endpoint::ProcessorMetrics::~ProcessorMetrics() {}


void endpoint::ProcessorMetrics::get(const server::Request& req, server::Response& res) {

    auto processor_uuid = psme::rest::model::find<agent_framework::model::System, agent_framework::model::Processor>(
        req.params).get_uuid();

    auto json = make_prototype();
    json[Common::ODATA_ID] = PathBuilder(req).build();
    json[Common::ID] = constants::Common::METRICS;
    json[Common::NAME] = "Processor " + req.params[PathParam::PROCESSOR_ID] + " Metrics";

    utils::populate_metrics(json, processor_uuid);

    set_response(res, json);
}
