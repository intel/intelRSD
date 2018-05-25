/*!
 * @brief ProcessorMetrics endpoint implementation
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
 * @file processor_metrics.cpp
 */

#include "psme/rest/endpoints/system/processor_metrics.hpp"
#include "psme/rest/endpoints/utils.hpp"


using namespace psme::rest;
using namespace psme::rest::constants;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#ProcessorMetrics.ProcessorMetrics";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#ProcessorMetrics.v1_0_0.ProcessorMetrics";
    r[Common::ID] = json::Value::Type::NIL;
    r[Common::NAME] = "Processor Metrics";
    r[Common::DESCRIPTION] = "Processor Metrics";
    r[ProcessorMetrics::AVERAGE_FREQUENCY_MHZ] = json::Value::Type::NIL;
    r[ProcessorMetrics::BANDWIDTH_PERCENT] = json::Value::Type::NIL;
    r[ProcessorMetrics::CONSUMED_POWER_WATT] = json::Value::Type::NIL;
    r[ProcessorMetrics::HEALTH] = json::Value::Type::ARRAY;
    r[ProcessorMetrics::TEMPERATURE_CELSIUS] = json::Value::Type::NIL;
    r[ProcessorMetrics::THROTTLING_CELSIUS] = json::Value::Type::NIL;

    return r;
}

}


endpoint::ProcessorMetrics::ProcessorMetrics(const std::string& path) : EndpointBase(path) {}


endpoint::ProcessorMetrics::~ProcessorMetrics() {}


void endpoint::ProcessorMetrics::get(const server::Request& req, server::Response& res) {

    auto processor_uuid = psme::rest::model::Find<agent_framework::model::Processor>(req.params[PathParam::PROCESSOR_ID]).
        via<agent_framework::model::System>(req.params[PathParam::SYSTEM_ID]).get_uuid();

    auto json = make_prototype();
    json[Common::ODATA_ID] = PathBuilder(req).build();
    json[Common::ID] = "Processor " + req.params[PathParam::PROCESSOR_ID] + " Metrics";

    utils::populate_metrics(json, processor_uuid);

    set_response(res, json);
}
