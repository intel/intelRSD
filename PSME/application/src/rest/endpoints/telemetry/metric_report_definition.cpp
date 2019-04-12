/*!
 * @brief Metric report definition endpoint
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
 * @file metric_report_definition.cpp
 */

#include "psme/rest/endpoints/telemetry/metric_report_definition.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;


namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Intel_RackScale.MetricReportDefinition.MetricReportDefinition";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#Intel_RackScale.MetricReportDefinition.v1_0_0.MetricReportDefinition";
    r[Common::ID] = json::Json::value_t::null;
    r[Common::NAME] = "Metric Report Definition";
    r[Common::DESCRIPTION] = "Metric Report Definition description";
    r[Common::STATUS][Common::STATE] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Json::value_t::null;
    r[MetricReportDefinition::METRIC_REPORT_TYPE] = json::Json::value_t::null;
    r[TelemetryCommon::METRIC_REPORT_DESTINATION] = json::Json::value_t::null;
    r[MetricReportDefinition::METRIC_REPORT_ACTIONS] = json::Json::value_t::array;
    r[TelemetryCommon::POLLING_INTERVAL_MS] = json::Json::value_t::null;
    r[MetricReportDefinition::REPORT_INTERVAL_MS] = json::Json::value_t::null;
    r[MetricReportDefinition::TRANSMIT_FORMAT] = json::Json::value_t::null;
    r[Common::ACTIONS][Common::OEM] = json::Json::value_t::object;
    r[Common::LINKS][TelemetryCommon::METRICS] = json::Json::value_t::array;
    r[Common::LINKS][TelemetryCommon::LOG_TO] = json::Json::value_t::null;

    return r;
}
}


endpoint::MetricReportDefinition::MetricReportDefinition(const std::string& path) : EndpointBase(path) {}


endpoint::MetricReportDefinition::~MetricReportDefinition() {}


void endpoint::MetricReportDefinition::get(const server::Request& request, server::Response& response) {
    auto json = make_prototype();
    json[Common::ODATA_ID] = PathBuilder(request).build();

    set_response(response, json);
}


[[noreturn]] void endpoint::MetricReportDefinition::del(const server::Request&, server::Response&) {
    throw agent_framework::exceptions::NotImplemented("This method is not implemented.");
}
