/*!
 * @brief Trigger endpoint
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
 * @file trigger.cpp
 */

#include "psme/rest/endpoints/telemetry/trigger.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;


namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Trigger.Trigger";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#Triggers.v1_0_0.Triggers";
    r[Common::ID] = json::Value::Type::NIL;
    r[Common::NAME] = "Triggers";
    r[Common::DESCRIPTION] = "Triggers description";
    r[Common::STATUS][Common::STATE] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Value::Type::NIL;
    r[TelemetryCommon::POLLING_INTERVAL_MS] = json::Value::Type::NIL;
    r[Trigger::TRIGGER_TYPE] = json::Value::Type::NIL;
    r[Trigger::TRIGGER_ACTIONS] = json::Value::Type::ARRAY;
    r[TelemetryCommon::METRIC_REPORT_DESTINATION] = json::Value::Type::NIL;
    r[Trigger::NUMERIC_TRIGGERS] = json::Value::Type::ARRAY;
    r[Trigger::DISCRETE_TRIGGERS] = json::Value::Type::ARRAY;
    r[Common::ACTIONS][Common::OEM] = json::Value::Type::OBJECT;
    r[Common::LINKS][TelemetryCommon::METRICS] = json::Value::Type::ARRAY;
    r[Common::LINKS][TelemetryCommon::LOG_TO] = json::Value::Type::NIL;

    return r;
}
}


endpoint::Trigger::Trigger(const std::string& path) : EndpointBase(path) {}


endpoint::Trigger::~Trigger() {}


void endpoint::Trigger::get(const server::Request& request, server::Response& response) {
    auto json = make_prototype();
    json[Common::ODATA_ID] = PathBuilder(request).build();

    set_response(response, json);
}


void endpoint::Trigger::del(const server::Request&, server::Response&) {
}
