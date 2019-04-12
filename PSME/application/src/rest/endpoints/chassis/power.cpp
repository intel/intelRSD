/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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

#include "psme/rest/endpoints/chassis/power.hpp"
#include "psme/rest/constants/constants.hpp"
#include "psme/rest/utils/status_helpers.hpp"
#include "psme/rest/server/error/error_factory.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;

namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Power.Power";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#Power.v1_1_0.Power";
    r[Common::ID] = "Power";
    r[Common::NAME] = "Power";
    r[Common::DESCRIPTION] = "Power";

    r[PowerZone::POWER_CONTROL] = json::Json::value_t::array;
    r[PowerZone::VOLTAGES] = json::Json::value_t::array;
    r[PowerZone::POWER_SUPPLIES] = json::Json::value_t::array;
    r[Common::REDUNDANCY] = json::Json::value_t::array;

    r[Common::OEM][Common::RACKSCALE][Common::ODATA_TYPE] = "#Intel.Oem.Power";
    r[Common::OEM][Common::RACKSCALE][PowerZone::INPUT_AC_POWER_WATTS] = json::Json::value_t::null;

    json::Json action = json::Json();
    action[Common::TARGET] = json::Json::value_t::null;
    action[PowerZone::STATE_ALLOWABLE_VALUES] = json::Json::value_t::array;
    action[PowerZone::MEMBER_ID_ALLOWABLE_VALUES] = json::Json::value_t::array;

    r[Common::OEM][Common::RACKSCALE][Common::ACTIONS]
        [PowerZone::HASH_INTEL_OEM_REQUEST_POWER_SUPPLY_STATE_CHANGE] = std::move(action);

    return r;
}


json::Json make_power_control_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::MEMBER_ID] = json::Json::value_t::null;
    r[Common::NAME] = json::Json::value_t::null;

    r[PowerZone::POWER_CONSUMED_WATTS] = json::Json::value_t::null;
    r[PowerZone::POWER_REQUESTED_WATTS] = json::Json::value_t::null;
    r[PowerZone::POWER_AVAILABLE_WATTS] = json::Json::value_t::null;
    r[PowerZone::POWER_CAPACITY_WATTS] = json::Json::value_t::null;
    r[PowerZone::POWER_ALLOCATED_WATTS] = json::Json::value_t::null;

    r[PowerZone::POWER_METRICS][PowerZone::INTERVAL_IN_MIN] = json::Json::value_t::null;
    r[PowerZone::POWER_METRICS][PowerZone::MIN_CONSUMED_WATTS] = json::Json::value_t::null;
    r[PowerZone::POWER_METRICS][PowerZone::MAX_CONSUMED_WATTS] = json::Json::value_t::null;
    r[PowerZone::POWER_METRICS][PowerZone::AVERAGE_CONSUMED_WATTS] = json::Json::value_t::null;

    r[PowerZone::POWER_LIMIT][PowerZone::LIMIT_IN_WATTS] = json::Json::value_t::null;
    r[PowerZone::POWER_LIMIT][PowerZone::LIMIT_EXCEPTION] = json::Json::value_t::null;
    r[PowerZone::POWER_LIMIT][PowerZone::CORRECTION_IN_MS] = json::Json::value_t::null;

    r[Common::STATUS][Common::STATE] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Json::value_t::null;

    r[Common::RELATED_ITEM] = json::Json::value_t::array;
    r[Common::OEM] = json::Json::value_t::object;

    return r;
}


json::Json make_voltage_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::MEMBER_ID] = json::Json::value_t::null;
    r[Common::NAME] = json::Json::value_t::null;

    r[ChassisSensor::SENSOR_NUMBER] = json::Json::value_t::null;
    r[ChassisSensor::READING_VOLTS] = json::Json::value_t::null;

    r[ChassisSensor::UPPER_THRESHOLD_NON_CRITICAL] = json::Json::value_t::null;
    r[ChassisSensor::UPPER_THRESHOLD_CRITICAL] = json::Json::value_t::null;
    r[ChassisSensor::UPPER_THRESHOLD_FATAL] = json::Json::value_t::null;

    r[ChassisSensor::LOWER_THRESHOLD_NON_CRITICAL] = json::Json::value_t::null;
    r[ChassisSensor::LOWER_THRESHOLD_CRITICAL] = json::Json::value_t::null;
    r[ChassisSensor::LOWER_THRESHOLD_FATAL] = json::Json::value_t::null;

    r[ChassisSensor::MIN_READING_RANGE] = json::Json::value_t::null;
    r[ChassisSensor::MAX_READING_RANGE] = json::Json::value_t::null;
    r[ChassisSensor::PHYSICAL_CONTEXT] = json::Json::value_t::null;

    r[Common::STATUS][Common::STATE] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Json::value_t::null;

    r[Common::RELATED_ITEM] = json::Json::value_t::array;

    return r;
}


json::Json make_power_supply_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#Power.v1_1_0.PowerSupply";
    r[Common::MEMBER_ID] = json::Json::value_t::null;
    r[Common::NAME] = json::Json::value_t::null;

    r[Common::STATUS][Common::STATE] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Json::value_t::null;

    r[PowerZone::POWER_SUPPLY_TYPE] = json::Json::value_t::null;
    r[PowerZone::LINE_INPUT_VOLTAGE_TYPE] = json::Json::value_t::null;
    r[PowerZone::LINE_INPUT_VOLTAGE] = json::Json::value_t::null;
    r[PowerZone::POWER_CAPACITY_WATTS] = json::Json::value_t::null;
    r[PowerZone::LAST_POWER_OUTPUT_WATTS] = json::Json::value_t::null;

    r[Common::MODEL] = json::Json::value_t::null;
    r[Common::MANUFACTURER] = json::Json::value_t::null;
    r[Common::FIRMWARE_VERSION] = json::Json::value_t::null;
    r[Common::SERIAL_NUMBER] = json::Json::value_t::null;
    r[Common::PART_NUMBER] = json::Json::value_t::null;

    r[PowerZone::SPARE_PART_NUMBER] = json::Json::value_t::null;
    r[PowerZone::INPUT_RANGES] = json::Json::value_t::array;

    r[Common::REDUNDANCY] = json::Json::value_t::array;
    r[Common::RELATED_ITEM] = json::Json::value_t::array;

    r[Common::OEM] = json::Json::value_t::object;

    return r;
}


void fill_related_items(const agent_framework::model::PowerZone& power_zone, json::Json& r) {
    // Find all chassis powered by current power zone
    auto powered_chassis = agent_framework::module::get_manager<agent_framework::model::Chassis>()
        .get_keys([power_zone](const agent_framework::model::Chassis& ch) {
            return ch.get_power_zone() == power_zone.get_uuid();
        });

    for (const auto& chassis_uuid : powered_chassis) {
        json::Json related_item = json::Json();
        related_item[Common::ODATA_ID] = endpoint::utils::get_component_url(
            agent_framework::model::enums::Component::Chassis, chassis_uuid);

        r[Common::RELATED_ITEM].push_back(related_item);
    }
}


void fill_voltages(const agent_framework::model::PowerZone& power_zone,
                   const std::vector<agent_framework::model::Metric>& voltage_readings,
                   const std::string& power_url, json::Json& json_response) {
    for (std::size_t voltage_index = 0; voltage_index < voltage_readings.size(); ++voltage_index) {
        const auto& voltage = voltage_readings[voltage_index];
        const auto definition = agent_framework::module::get_manager<agent_framework::model::MetricDefinition>()
                .get_entry(voltage.get_metric_definition_uuid());
        auto v = ::make_voltage_prototype();
        v[Common::ODATA_ID] =
            endpoint::PathBuilder(power_url + Common::HASH)
                .append(PowerZone::VOLTAGES)
                .append(voltage_index)
                .build();

        v[Common::MEMBER_ID] = std::to_string(voltage.get_id());

        v[ChassisSensor::SENSOR_NUMBER] = voltage_index;

        // fill ReadingVolts field:
        endpoint::utils::populate_metrics(v, {voltage});

        v[Common::NAME] = definition.get_name();
        v[ChassisSensor::MIN_READING_RANGE] = definition.get_min_reading_range();
        v[ChassisSensor::MAX_READING_RANGE] = definition.get_max_reading_range();
        v[ChassisSensor::PHYSICAL_CONTEXT] = definition.get_physical_context();

        fill_related_items(power_zone, v);
        psme::rest::endpoint::status_to_json(voltage, v);
        json_response[PowerZone::VOLTAGES].push_back(v);
    }
}


void fill_power_controls(const agent_framework::model::PowerZone& power_zone,
                         const std::vector<agent_framework::model::Metric>& dc_power_consumed_metrics,
                         const std::string& power_url,
                         json::Json& json_response) {
    auto control = ::make_power_control_prototype();
    control[Common::ODATA_ID] =
        endpoint::PathBuilder(power_url + Common::HASH)
            .append(PowerZone::POWER_CONTROL)
            .append(0) // index in array
            .build();

    control[Common::MEMBER_ID] = std::to_string(power_zone.get_id());
    control[Common::NAME] = "System Power Control 0";

    // fill PowerConsumedWatts
    endpoint::utils::populate_metrics(control, dc_power_consumed_metrics);

    control[PowerZone::POWER_REQUESTED_WATTS] = power_zone.get_power_requested_watts();
    control[PowerZone::POWER_AVAILABLE_WATTS] = power_zone.get_power_available_watts();
    control[PowerZone::POWER_CAPACITY_WATTS] = power_zone.get_power_capacity_watts();
    control[PowerZone::POWER_ALLOCATED_WATTS] = power_zone.get_power_allocated_watts();

    fill_related_items(power_zone, control);
    psme::rest::endpoint::status_to_json(power_zone, control);

    json_response[PowerZone::POWER_CONTROL].push_back(control);
}


void fill_power_supplies(const std::vector<std::string>& power_zones, const std::string& power_url, json::Json& r) {
    for (std::size_t power_zone_index = 0; power_zone_index < power_zones.size(); ++power_zone_index) {
        auto power_zone = agent_framework::module::get_manager<agent_framework::model::PowerZone>()
            .get_entry(power_zones[power_zone_index]);
        auto power_supplies = agent_framework::module::get_manager<agent_framework::model::Psu>()
            .get_keys(power_zone.get_uuid());

        for (std::size_t psu_index = 0; psu_index < power_supplies.size(); ++psu_index) {
            auto psu = agent_framework::module::get_manager<agent_framework::model::Psu>()
                .get_entry(power_supplies[psu_index]);

            auto power_supply = ::make_power_supply_prototype();
            power_supply[Common::ODATA_ID] =
                endpoint::PathBuilder(power_url + Common::HASH)
                    .append(PowerZone::POWER_SUPPLIES)
                    .append(psu_index)
                    .build();

            power_supply[Common::MEMBER_ID] = std::to_string(psu.get_id());
            power_supply[Common::NAME] = "Power Supply " + std::to_string(psu_index + 1);

            power_supply[PowerZone::POWER_SUPPLY_TYPE] = psu.get_power_supply_type();
            power_supply[PowerZone::LINE_INPUT_VOLTAGE_TYPE] = psu.get_line_input_voltage_type();
            power_supply[PowerZone::LINE_INPUT_VOLTAGE] = psu.get_line_input_voltage_volts();
            power_supply[PowerZone::POWER_CAPACITY_WATTS] = psu.get_power_capacity_watts();
            power_supply[PowerZone::LAST_POWER_OUTPUT_WATTS] = psu.get_last_power_output_watts();

            power_supply[Common::MODEL] = psu.get_fru_info().get_model_number();
            power_supply[Common::MANUFACTURER] = psu.get_fru_info().get_manufacturer();
            power_supply[Common::FIRMWARE_VERSION] = psu.get_firmware_version();
            power_supply[Common::SERIAL_NUMBER] = psu.get_fru_info().get_serial_number();
            power_supply[Common::PART_NUMBER] = psu.get_fru_info().get_part_number();

            fill_related_items(power_zone, power_supply);
            psme::rest::endpoint::status_to_json(psu, power_supply);
            r[PowerZone::POWER_SUPPLIES].push_back(power_supply);
        }
    }
}

void fill_actions(const std::vector<std::string>& power_zones, const std::string& power_url, json::Json& r) {
    json::Json state_change_action = json::Json();
    state_change_action[Common::TARGET] =
        endpoint::PathBuilder(power_url)
            .append(Common::OEM)
            .append(Common::RACKSCALE)
            .append(Common::ACTIONS)
            .append(PowerZone::INTEL_OEM_REQUEST_POWER_SUPPLY_STATE_CHANGE)
            .build();

    state_change_action[PowerZone::STATE_ALLOWABLE_VALUES] = json::Json::value_t::array;
    for (const auto& state : endpoint::Power::get_state_allowable_values()) {
        state_change_action[PowerZone::STATE_ALLOWABLE_VALUES].push_back(state);
    }

    state_change_action[PowerZone::MEMBER_ID_ALLOWABLE_VALUES] = json::Json::value_t::array;
    for (const auto& power_zone_uuid : power_zones) {
        auto power_zone = agent_framework::module::get_manager<agent_framework::model::PowerZone>()
            .get_entry(power_zone_uuid);
        auto power_supplies = agent_framework::module::get_manager<agent_framework::model::Psu>()
            .get_entries(power_zone.get_uuid());
        for (const auto& psu : power_supplies) {
            state_change_action[PowerZone::MEMBER_ID_ALLOWABLE_VALUES].push_back(std::to_string(psu.get_id()));
        }
    }

    r[Common::OEM][Common::RACKSCALE][Common::ACTIONS]
        [PowerZone::HASH_INTEL_OEM_REQUEST_POWER_SUPPLY_STATE_CHANGE] = std::move(state_change_action);
}

}


endpoint::Power::Power(const std::string& path) : EndpointBase(path) {}


endpoint::Power::~Power() {}


const std::vector<std::string>& endpoint::Power::get_state_allowable_values() {
    using namespace agent_framework::model;
    static std::vector<std::string> allowed_states{
        enums::State(enums::State::Enabled).to_string(),
        enums::State(enums::State::Disabled).to_string()
    };
    return allowed_states;
}



void endpoint::Power::get(const server::Request& request, server::Response& response) {
    auto chassis = model::find<agent_framework::model::Chassis>(request.params).get();

    const auto& url = request.get_url();
    auto r = make_prototype();
    r[Common::ODATA_ID] = PathBuilder(request).build();

    auto power_zones = agent_framework::module::get_manager<agent_framework::model::PowerZone>()
        .get_keys(chassis.get_uuid());
    if (power_zones.empty()) {
        throw error::ServerException(error::ErrorFactory::create_resource_missing_error(request.get_url()));
    }
    else {
        auto power_zone = agent_framework::module::get_manager<agent_framework::model::PowerZone>()
            .get_entry(power_zones.front());

        // Power Zone metrics:
        // Power Consumed metrics go into PowerControl array member
        auto dc_power_consumed_metrics = endpoint::utils::get_metrics(power_zone, PowerZone::POWER_CONSUMED_WATTS_PATH);
        ::fill_power_controls(power_zone, dc_power_consumed_metrics, url, r);

        // Voltage metrics go into a separate array in the top-level json object
        auto voltage_metrics = endpoint::utils::get_metrics(power_zone, ChassisSensor::READING_VOLTS_PATH);
        fill_voltages(power_zone, voltage_metrics, url, r);

        // Input AC Power metrics go into a property in the top-level json object
        auto ac_power_consumed_metrics = endpoint::utils::get_metrics(power_zone, PowerZone::INPUT_AC_POWER_WATTS_PATH);
        endpoint::utils::populate_metrics(r, ac_power_consumed_metrics);

        ::fill_power_supplies(power_zones, url, r);
    }

    ::fill_actions(power_zones, url, r);

    set_response(response, r);
}
