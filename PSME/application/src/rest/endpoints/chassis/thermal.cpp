/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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

#include "agent-framework/module/constants/rmm.hpp"
#include "agent-framework/module/requests/common.hpp"
#include "agent-framework/module/responses/common.hpp"

#include "psme/rest/endpoints/chassis/thermal.hpp"
#include "psme/rest/server/error/error_factory.hpp"
#include "psme/rest/utils/status_helpers.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/thermal.hpp"


using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::validators;

namespace {

json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Thermal.Thermal";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#Thermal.v1_1_0.Thermal";
    r[Common::NAME] = "Thermal";
    r[Common::DESCRIPTION] = "Thermal";
    r[Common::ID] = "Thermal";

    r[ThermalZone::TEMPERATURES] = json::Value::Type::ARRAY;
    r[ThermalZone::FANS] = json::Value::Type::ARRAY;
    r[Common::REDUNDANCY] = json::Value::Type::ARRAY;

    r[Common::STATUS][Common::STATE] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Value::Type::NIL;

    json::Value rs{};
    rs[Common::ODATA_TYPE] = "#Intel.Oem.Thermal";
    rs[ThermalZone::DESIRED_SPEED_PWM] = json::Value::Type::NIL;
    rs[ThermalZone::VOLUMETRIC_AIRFLOW_CFM] = json::Value::Type::NIL;
    r[Common::OEM][Common::RACKSCALE] = std::move(rs);

    return r;
}


json::Value make_fan_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#Thermal.v1_1_0.Fan";
    r[Common::MEMBER_ID] = json::Value::Type::NIL;
    r[Common::NAME] = json::Value::Type::NIL;

    r[ThermalZone::READING] = json::Value::Type::NIL;
    r[ThermalZone::READING_UNITS] = json::Value::Type::NIL;

    r[ChassisSensor::UPPER_THRESHOLD_NON_CRITICAL] = json::Value::Type::NIL;
    r[ChassisSensor::UPPER_THRESHOLD_CRITICAL] = json::Value::Type::NIL;
    r[ChassisSensor::UPPER_THRESHOLD_FATAL] = json::Value::Type::NIL;

    r[ChassisSensor::LOWER_THRESHOLD_NON_CRITICAL] = json::Value::Type::NIL;
    r[ChassisSensor::LOWER_THRESHOLD_CRITICAL] = json::Value::Type::NIL;
    r[ChassisSensor::LOWER_THRESHOLD_FATAL] = json::Value::Type::NIL;

    r[ChassisSensor::MIN_READING_RANGE] = json::Value::Type::NIL;
    r[ChassisSensor::MAX_READING_RANGE] = json::Value::Type::NIL;
    r[ChassisSensor::PHYSICAL_CONTEXT] = json::Value::Type::NIL;

    r[Common::STATUS][Common::STATE] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Value::Type::NIL;

    r[Common::REDUNDANCY] = json::Value::Type::ARRAY;
    r[Common::RELATED_ITEM] = json::Value::Type::ARRAY;
    return r;
}


json::Value make_temperature_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::MEMBER_ID] = json::Value::Type::NIL;
    r[Common::NAME] = json::Value::Type::NIL;

    r[ChassisSensor::SENSOR_NUMBER] = json::Value::Type::NIL;
    r[ChassisSensor::READING_CELSIUS] = json::Value::Type::NIL;

    r[ChassisSensor::UPPER_THRESHOLD_NON_CRITICAL] = json::Value::Type::NIL;
    r[ChassisSensor::UPPER_THRESHOLD_CRITICAL] = json::Value::Type::NIL;
    r[ChassisSensor::UPPER_THRESHOLD_FATAL] = json::Value::Type::NIL;

    r[ChassisSensor::LOWER_THRESHOLD_NON_CRITICAL] = json::Value::Type::NIL;
    r[ChassisSensor::LOWER_THRESHOLD_CRITICAL] = json::Value::Type::NIL;
    r[ChassisSensor::LOWER_THRESHOLD_FATAL] = json::Value::Type::NIL;

    r[ChassisSensor::MIN_READING_RANGE_TEMP] = json::Value::Type::NIL;
    r[ChassisSensor::MAX_READING_RANGE_TEMP] = json::Value::Type::NIL;
    r[ChassisSensor::PHYSICAL_CONTEXT] = json::Value::Type::NIL;

    r[Common::STATUS][Common::STATE] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Value::Type::NIL;

    r[Common::RELATED_ITEM] = json::Value::Type::ARRAY;

    return r;
}


void fill_related_items(const agent_framework::model::ThermalZone& thermal_zone, json::Value& r) {
    // Find all chassis related to the current thermal zone
    auto cooled_chassis = agent_framework::module::get_manager<agent_framework::model::Chassis>()
        .get_keys([thermal_zone](const agent_framework::model::Chassis& ch) {
            return ch.get_thermal_zone() == thermal_zone.get_uuid();
        });
    for (const auto& chassis_uuid : cooled_chassis) {
        json::Value related_item{};
        related_item[Common::ODATA_ID] = endpoint::utils::get_component_url(
            agent_framework::model::enums::Component::Chassis, chassis_uuid);
        r[Common::RELATED_ITEM].push_back(related_item);
    }
}


void fill_fans(const agent_framework::model::ThermalZone& thermal_zone,
               json::Value& json_response, const std::string& thermal_url) {
    auto fans = agent_framework::module::get_manager<agent_framework::model::Fan>()
        .get_keys(thermal_zone.get_uuid());

    for (std::size_t index = 0; index < fans.size(); index++) {
        auto fan = agent_framework::module::get_manager<agent_framework::model::Fan>()
            .get_entry(fans[index]);
        auto metrics = agent_framework::module::get_manager<agent_framework::model::Metric>().get_entries([&fan](
            const agent_framework::model::Metric& metric) -> bool {
            return metric.get_component_uuid() == fan.get_uuid();
        });
        if (metrics.size() != 1) {
            log_warning("rest", "Fan " << fan.get_uuid() << " has " << metrics.size() << " assigned metrics.");
            continue;
        }
        const auto& metric = metrics[0];
        const auto definition = agent_framework::module::get_manager<agent_framework::model::MetricDefinition>()
            .get_entry(metric.get_metric_definition_uuid());

        auto f = ::make_fan_prototype();

        f[Common::ODATA_ID] =
            endpoint::PathBuilder(thermal_url + Common::HASH)
                .append(ThermalZone::FANS)
                .append(index)
                .build();
        f[Common::MEMBER_ID] = std::to_string(metric.get_id());
        f[Common::NAME] = "BaseBoard System Fan " + std::to_string(index);

        // fill Reading field:
        endpoint::utils::populate_metrics(f, metrics);

        f[ThermalZone::READING_UNITS] = definition.get_units();
        f[ChassisSensor::MIN_READING_RANGE] = definition.get_min_reading_range();
        f[ChassisSensor::MAX_READING_RANGE] = definition.get_max_reading_range();
        f[ChassisSensor::PHYSICAL_CONTEXT] = definition.get_physical_context();

        psme::rest::endpoint::status_to_json(fan, f);
        ::fill_related_items(thermal_zone, f);

        json_response[ThermalZone::FANS].push_back(f);
    }
}


void fill_temperatures(const agent_framework::model::ThermalZone& thermal_zone,
                       const std::vector<agent_framework::model::Metric>& temperature_readings,
                       json::Value& json_response, const std::string& thermal_url) {
    for (std::size_t index = 0; index < temperature_readings.size(); index++) {
        const auto& temperature_reading = temperature_readings[index];
        const auto definition = agent_framework::module::get_manager<agent_framework::model::MetricDefinition>()
            .get_entry(temperature_reading.get_metric_definition_uuid());

        auto t = ::make_temperature_prototype();

        t[Common::ODATA_ID] =
            endpoint::PathBuilder(thermal_url + Common::HASH)
                .append(ThermalZone::TEMPERATURES)
                .append(index)
                .build();

        t[Common::MEMBER_ID] = std::to_string(temperature_reading.get_id());


        t[ChassisSensor::SENSOR_NUMBER] = index;

        // fill ReadingCelsius
        endpoint::utils::populate_metrics(t, {temperature_reading});

        t[Common::NAME] = definition.get_name();
        t[ChassisSensor::MIN_READING_RANGE_TEMP] = definition.get_min_reading_range();
        t[ChassisSensor::MAX_READING_RANGE_TEMP] = definition.get_max_reading_range();
        t[ChassisSensor::PHYSICAL_CONTEXT] = definition.get_physical_context();

        psme::rest::endpoint::status_to_json(temperature_reading, t);
        ::fill_related_items(thermal_zone, t);

        json_response[ThermalZone::TEMPERATURES].push_back(t);
    }
}

static const std::map<std::string, std::string> gami_to_rest_attributes = {
    {agent_framework::model::literals::ThermalZone::DESIRED_SPEED_PWM,
        endpoint::PathBuilder(constants::Common::OEM)
            .append(constants::Common::RACKSCALE)
            .append(constants::ThermalZone::DESIRED_SPEED_PWM)
            .build()
    }
};

void fill_patch_attributes(const json::Value& patch, agent_framework::model::attribute::Attributes& attributes) {
    if (patch.is_member(constants::Common::OEM)) {
        const auto& oem = patch[constants::Common::OEM];
        if (oem.is_member(constants::Common::RACKSCALE)) {
            const auto& rackscale = oem[constants::Common::RACKSCALE];
            if (rackscale.is_member(constants::ThermalZone::DESIRED_SPEED_PWM)) {
                auto pwm = rackscale[constants::ThermalZone::DESIRED_SPEED_PWM].as_uint();
                attributes.set_value(agent_framework::model::literals::ThermalZone::DESIRED_SPEED_PWM, pwm);
            }
        }
    }
}

void check_response_errors(const agent_framework::model::responses::SetComponentAttributes& response) {
    const auto& result_statuses = response.get_statuses();
    if (!result_statuses.empty()) {
        const auto& error = error::ErrorFactory::create_error_from_set_component_attributes_results(
            result_statuses, ::gami_to_rest_attributes);
        throw error::ServerException(error);
    }
}

void update_model_after_patch(psme::core::agent::JsonAgentSPtr agent,
                              const agent_framework::model::ThermalZone& thermal_zone) {
    psme::rest::model::handler::HandlerManager::get_instance()->get_handler(
        agent_framework::model::enums::Component::ThermalZone)->
        load(agent,
             thermal_zone.get_parent_uuid(),
             agent_framework::model::enums::Component::Chassis,
             thermal_zone.get_uuid(),
             true);
}

}


endpoint::Thermal::Thermal(const std::string& path) : EndpointBase(path) {}


endpoint::Thermal::~Thermal() {}


void endpoint::Thermal::get(const server::Request& request, server::Response& response) {
    auto chassis = model::Find<agent_framework::model::Chassis>(request.params[PathParam::CHASSIS_ID]).get();
    auto thermal_zones = agent_framework::module::get_manager<agent_framework::model::ThermalZone>()
        .get_keys(chassis.get_uuid());

    auto r = ::make_prototype();

    r[Common::ODATA_ID] = PathBuilder(request).build();
    if (thermal_zones.empty()) {
        throw error::ServerException(error::ErrorFactory::create_resource_missing_error(request.get_url()));
    }
    else {
        auto thermal_zone = agent_framework::module::get_manager<agent_framework::model::ThermalZone>()
            .get_entry(thermal_zones.front());

        // Thermal Zone metrics:
        // airflow metrics go into a property in the top-level json object
        auto airflow_metrics = endpoint::utils::get_metrics(thermal_zone, ThermalZone::VOLUMETRIC_AIRFLOW_CFM_PATH);
        endpoint::utils::populate_metrics(r, airflow_metrics);

        // Thermal metrics go into a separate array in the top-level json object
        auto temperature_metrics = endpoint::utils::get_metrics(thermal_zone, ChassisSensor::READING_CELSIUS_PATH);
        fill_temperatures(thermal_zone, temperature_metrics, r, request.get_url());

        psme::rest::endpoint::status_to_json(thermal_zone, r);
        r[Common::OEM][Common::RACKSCALE][ThermalZone::DESIRED_SPEED_PWM] = thermal_zone.get_desired_speed_pwm();

        fill_fans(thermal_zone, r, request.get_url());
    }

    set_response(response, r);
}

void endpoint::Thermal::patch(const server::Request& request, server::Response& response) {
    auto chassis_id = request.params[PathParam::CHASSIS_ID];
    auto chassis = model::Find<agent_framework::model::Chassis>(chassis_id).get();

    // Only RMM service has this action
    if (!utils::has_resource_capability(chassis, Capability::RMM)) {
        throw error::ServerException(
            error::ErrorFactory::create_method_not_allowed_error(
                "This action is allowed only for RMM service."));
    }

    auto thermal_zones = agent_framework::module::get_manager<agent_framework::model::ThermalZone>()
        .get_keys(chassis.get_uuid());

    if (!thermal_zones.empty()) {
        auto thermal_zone = agent_framework::module::get_manager<agent_framework::model::ThermalZone>()
            .get_entry(thermal_zones.front());

        const auto json = JsonValidator::validate_request_body<schema::ThermalPatchSchema>(request);
        agent_framework::model::attribute::Attributes attributes{};
        ::fill_patch_attributes(json, attributes);

        if (!attributes.empty()) {
            agent_framework::model::requests::SetComponentAttributes gami_request{thermal_zone.get_uuid(), attributes};
            const auto& gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(thermal_zone.get_agent_id());

            auto set_thermal_zone_attributes = [&, gami_agent] {
                const auto& gami_response =
                    gami_agent->execute<agent_framework::model::responses::SetComponentAttributes>(gami_request);

                ::check_response_errors(gami_response);
                ::update_model_after_patch(gami_agent, thermal_zone);
            };

            gami_agent->execute_in_transaction(set_thermal_zone_attributes);
        }
    }
    else {
        auto message = std::string("PATCH operation on chassis of type ") +
            chassis.get_type().to_string() + " is not allowed.";
        throw error::ServerException(error::ErrorFactory::create_invalid_payload_error(message));
    }

    get(request, response);
}
