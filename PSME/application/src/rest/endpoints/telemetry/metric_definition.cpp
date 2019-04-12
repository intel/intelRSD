/*!
 * @brief Metric definition endpoint
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
 * @file metric_definition.cpp
 */

#include "psme/rest/endpoints/telemetry/metric_definition.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;


namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Intel_RackScale.MetricDefinition.MetricDefinition";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#Intel_RackScale.MetricDefinition.v1_0_0.MetricDefinition";
    r[Common::ID] = json::Json::value_t::null;
    r[Common::NAME] = "Metric Definition";
    r[Common::DESCRIPTION] = "Metric Definition description";
    r[MetricDefinition::IMPLEMENTATION] = json::Json::value_t::null;
    r[MetricDefinition::CALCULABLE] = json::Json::value_t::null;
    r[MetricDefinition::UNITS] = json::Json::value_t::null;
    r[MetricDefinition::DATA_TYPE] = json::Json::value_t::null;
    r[MetricDefinition::IS_LINEAR] = json::Json::value_t::null;
    r[MetricDefinition::METRIC_TYPE] = json::Json::value_t::null;
    r[MetricDefinition::WILDCARDS] = json::Json::value_t::array;
    r[MetricDefinition::METRIC_PROPERTIES] = json::Json::value_t::array;
    r[MetricDefinition::CALCULATION_PARAMETERS] = json::Json::value_t::array;
    r[MetricDefinition::PHYSICAL_CONTEXT] = json::Json::value_t::null;
    r[MetricDefinition::SENSOR_TYPE] = json::Json::value_t::null;
    r[MetricDefinition::SENSING_INTERVAL] = json::Json::value_t::null;
    r[MetricDefinition::DISCRETE_VALUES] = json::Json::value_t::array;
    r[MetricDefinition::PRECISION] = json::Json::value_t::null;
    r[MetricDefinition::ACCURACY] = json::Json::value_t::null;
    r[MetricDefinition::CALIBRATION] = json::Json::value_t::null;
    r[MetricDefinition::TIMESTAMP_ACCURACY] = json::Json::value_t::null;
    r[MetricDefinition::MIN_READING_RANGE] = json::Json::value_t::null;
    r[MetricDefinition::MAX_READING_RANGE] = json::Json::value_t::null;
    r[MetricDefinition::CALCULATION_ALGORITHM] = json::Json::value_t::null;
    r[MetricDefinition::CALCULATION_TIME_INTERVAL] = json::Json::value_t::null;
    r[Common::OEM][Common::RACKSCALE][Common::ODATA_TYPE] = "#Intel.Oem.MetricDefinition";
    r[Common::OEM][Common::RACKSCALE][MetricDefinition::CALCULATION_PRECISION] = json::Json::value_t::null;
    r[Common::OEM][Common::RACKSCALE][MetricDefinition::DISCRETE_METRIC_TYPE] = json::Json::value_t::null;

    return r;
}

void make_metric_links(const agent_framework::model::MetricDefinition& definition, json::Json& response) {
    const auto metric_uuids = agent_framework::module::get_manager<agent_framework::model::Metric>().get_keys(
        [&](const agent_framework::model::Metric& m) {
            return m.get_metric_definition_uuid() == definition.get_uuid();
        }
    );
    for (const auto& metric_uuid : metric_uuids) {
        try {
            response[MetricDefinition::METRIC_PROPERTIES].push_back(
                endpoint::utils::get_component_url(agent_framework::model::Metric::get_component(), metric_uuid));
        }
        catch (const agent_framework::exceptions::NotFound& ex) {
            log_error("rest", "MetricDefinition/Metric link error " << ex.what());
        }
    }
}

}

endpoint::MetricDefinition::MetricDefinition(const std::string& path) : EndpointBase(path) {}


endpoint::MetricDefinition::~MetricDefinition() {}


void endpoint::MetricDefinition::get(const server::Request& request, server::Response& response) {
    auto json = make_prototype();
    json[Common::ODATA_ID] = PathBuilder(request).build();
    auto definition = psme::rest::model::find<agent_framework::model::MetricDefinition>(request.params).get();

    json[constants::Common::ID] = request.params[PathParam::METRIC_DEFINITION_ID];
    json[constants::Common::NAME] = definition.get_name();
    json[constants::MetricDefinition::IMPLEMENTATION] = definition.get_implementation();
    json[constants::MetricDefinition::CALCULABLE] = definition.get_calculable();
    json[constants::MetricDefinition::UNITS] = definition.get_units();
    json[constants::MetricDefinition::DATA_TYPE] = definition.get_data_type();
    json[constants::MetricDefinition::IS_LINEAR] = definition.get_is_linear();
    json[constants::MetricDefinition::METRIC_TYPE] = definition.get_metric_type();
    for(const auto& wildcard : definition.get_wildcards()) {
        json::Json wildcard_json = json::Json();
        wildcard_json[constants::Common::NAME] = wildcard.get_name();
        wildcard_json[constants::MetricDefinition::VALUES] = wildcard.get_values().to_json();
        json[constants::MetricDefinition::WILDCARDS].push_back(std::move(wildcard_json));
    }
    make_metric_links(definition, json);
    for(const auto& calculation_parameters : definition.get_calculation_parameters()) {
        json::Json params = json::Json();
        params[constants::MetricDefinition::SOURCE_METRIC] = calculation_parameters.get_source_metric();
        params[constants::MetricDefinition::RESULT_METRIC] = calculation_parameters.get_result_metric();
        json[constants::MetricDefinition::CALCULATION_PARAMETERS].push_back(std::move(params));
    }
    json[constants::MetricDefinition::PHYSICAL_CONTEXT] = definition.get_physical_context();
    json[constants::MetricDefinition::SENSOR_TYPE] = definition.get_sensor_type();
    json[constants::MetricDefinition::SENSING_INTERVAL] = definition.get_sensing_interval();
    json[constants::MetricDefinition::DISCRETE_VALUES] = definition.get_discrete_values().to_json();
    json[constants::MetricDefinition::PRECISION] = definition.get_precision();
    json[constants::MetricDefinition::ACCURACY] = definition.get_accuracy();
    json[constants::MetricDefinition::CALIBRATION] = definition.get_calibration();
    json[constants::MetricDefinition::TIMESTAMP_ACCURACY] = definition.get_time_stamp_accuracy();
    json[constants::MetricDefinition::MIN_READING_RANGE] = definition.get_min_reading_range();
    json[constants::MetricDefinition::MAX_READING_RANGE] = definition.get_max_reading_range();
    json[constants::MetricDefinition::CALCULATION_ALGORITHM] = definition.get_calculation_algorithm();
    json[constants::MetricDefinition::CALCULATION_TIME_INTERVAL] = definition.get_calculation_time_interval();

    json[Common::OEM][Common::RACKSCALE][constants::MetricDefinition::CALCULATION_PRECISION] = definition.get_calculation_precision();
    json[Common::OEM][Common::RACKSCALE][constants::MetricDefinition::DISCRETE_METRIC_TYPE] = definition.get_discrete_metric_type();

    set_response(response, json);
}
