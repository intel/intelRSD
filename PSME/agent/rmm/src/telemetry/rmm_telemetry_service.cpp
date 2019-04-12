/*!
 * @brief RmmTelemetryService definition
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file rmm_telemetry_service.cpp
 */

#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/model/power_zone.hpp"
#include "agent-framework/module/model/thermal_zone.hpp"
#include "agent-framework/module/model/fan.hpp"
#include "discovery/helpers/metrics.hpp"
#include "telemetry/resource_key.hpp"
#include "telemetry/rmm_telemetry_service.hpp"
#include "telemetry/metric_definition_builder.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/eventing/events_queue.hpp"
#include "ipmi/command/sdv/rmm/get_aggregated_thermal_sensor.hpp"
#include "ipmi/command/sdv/rmm/get_drawer_power.hpp"


using ::telemetry::ResourceInstance;
using agent_framework::model::enums::Component;
using namespace agent::rmm::discovery::helpers;
namespace request = ipmi::command::sdv::request;
namespace response = ipmi::command::sdv::response;




namespace agent {
namespace rmm {

namespace {
using ::telemetry::MetricDefinitionBuilder;

constexpr int EXPECTED_FAN_SLOTS = 6;

model::MetricDefinition AIRFLOW =
    MetricDefinitionBuilder("/Oem/Intel_RackScale/VolumetricAirflowCfm")
        .set_name("Zone Airflow")
        .set_implementation(enums::MetricImplementation::PhysicalSensor)
        .set_metric_calculable(enums::MetricCalculable::NonSummable)
        .set_units("Cfm")
        .set_data_type(enums::MetricDataType::Int32)
        .set_is_linear(true)
        .set_metric_type(enums::MetricType::Numeric)
        .set_physical_context(enums::PhysicalContext::Exhaust)
        .set_sensor_type(enums::SensorType::Fan)
        .build();

model::MetricDefinition INLET_TEMPERATURE =
    MetricDefinitionBuilder("/ReadingCelsius")
        .set_name("Drawer inlet Temp 0")
        .set_implementation(enums::MetricImplementation::PhysicalSensor)
        .set_metric_calculable(enums::MetricCalculable::NonSummable)
        .set_units("Celsius")
        .set_data_type(enums::MetricDataType::Int32)
        .set_is_linear(true)
        .set_metric_type(enums::MetricType::Numeric)
        .set_physical_context(enums::PhysicalContext::Exhaust)
        .set_sensor_type(enums::SensorType::Temperature)
        .build();

model::MetricDefinition FAN_READING =
    MetricDefinitionBuilder("/Reading")
        .set_name("Fan Reading")
        .set_implementation(enums::MetricImplementation::PhysicalSensor)
        .set_metric_calculable(enums::MetricCalculable::NonSummable)
        .set_units("RPM")
        .set_data_type(enums::MetricDataType::Int32)
        .set_is_linear(true)
        .set_metric_type(enums::MetricType::Numeric)
        .set_physical_context(enums::PhysicalContext::Exhaust)
        .set_sensor_type(enums::SensorType::Temperature)
        .build();

model::MetricDefinition CONSUMED_POWER =
    MetricDefinitionBuilder("/PowerConsumedWatts")
        .set_name("Zone Consumed Power")
        .set_implementation(enums::MetricImplementation::PhysicalSensor)
        .set_metric_calculable(enums::MetricCalculable::NonSummable)
        .set_units("W")
        .set_data_type(enums::MetricDataType::Int32)
        .set_is_linear(true)
        .set_metric_type(enums::MetricType::Numeric)
        .set_physical_context(enums::PhysicalContext::PowerSupply)
        .set_sensor_type(enums::SensorType::PowerSupplyOrConverter)
        .build();

model::MetricDefinition VOLTAGE =
    MetricDefinitionBuilder("/ReadingVolts")
        .set_name("VRM0 Voltage")
        .set_implementation(enums::MetricImplementation::PhysicalSensor)
        .set_metric_calculable(enums::MetricCalculable::NonSummable)
        .set_units("V")
        .set_data_type(enums::MetricDataType::Int32)
        .set_is_linear(true)
        .set_metric_type(enums::MetricType::Numeric)
        .set_physical_context(enums::PhysicalContext::VoltageRegulator)
        .set_sensor_type(enums::SensorType::Voltage)
        .build();


bool update_fan_reading(agent_framework::model::Metric& metric, const DiscoveryContext&,
                        const DiscoveryParams<RmmType::Fan>& dp) {
    log_debug("telemetry", "Fan reading update");
    if (metric.get_value().is_null() || dp.tachs != metric.get_value().get<uint16_t>()) {
        log_debug("telemetry", "Fan (" << unsigned(dp.slot_id) << ") speed changed: "
                                           << metric.get_value() << " -> " << dp.tachs);
        metric.set_value(dp.tachs);
        return true;
    }
    return false;
}

bool update_fan(model::Fan& fan, const DiscoveryContext&, const DiscoveryParams<RmmType::Fan>& dp) {
    bool changed = false;
    /* if no speed is reported, report Critical health */
    if ((dp.tachs == 0) && (fan.get_status().get_health() != enums::Health::Critical)) {
        log_debug("telemetry", "Fan (" << unsigned(fan.get_slot_id()) << ") stopped: set health critical");
        auto status = fan.get_status();
        status.set_health(enums::Health::Critical);
        fan.set_status(status);
        changed = true;
    }
    else if ((dp.tachs != 0) && (fan.get_status().get_health() == enums::Health::Critical)) {
        log_debug("telemetry", "Fan (" << unsigned(fan.get_slot_id()) << ") is being started: set health OK");
        auto status = fan.get_status();
        status.set_health(enums::Health::OK);
        fan.set_status(status);
        changed = true;
    }

    return changed;
}

bool update_inlet_temperature_reading(agent_framework::model::Metric& metric, const DiscoveryContext& dc,
                                      const DiscoveryParams<RmmType::ThermalZone>& dp) {
    log_debug("telemetry", "Inlet Temperature reading update");

    OptionalField<uint32_t> new_temperature{};
    unsigned entries_count = 0;
    log_debug("telemetry", "Inlet temperature update, checking " << unsigned(dp.drawer_slots.size()) << " drawers.");
    for (const auto& slot : dp.drawer_slots) {
        log_debug("telemetry", "Checking drawer on slot " << unsigned(slot));
        try {
            response::GetAggregatedThermalSensor thermal{};
            dc.ipmi->send(request::GetAggregatedThermalSensor{},
                          agent::rmm::loader::IpmiConfig::get_psme_chassis_bridge(slot), thermal);
            for (uint8_t i = 0; i < thermal.get_slot_count(); ++i) {
                if (thermal.is_valid_thermal_reading_value_on_slot(i)) {
                    new_temperature = new_temperature + uint32_t(thermal.get_thermal_reading_on_slot(i));
                    ++entries_count;
                }
            }
        }
        catch (const std::exception& e) {
            log_warning("telemetry", "Unable to get sensor reading for chassis on slot " << unsigned(slot)
                                  << ": " << e.what());
        }
    }
    log_debug("telemetry", "Inlet temperature: received " << entries_count << " responses from drawers.");
    if (entries_count > 0) {
        new_temperature = uint32_t(new_temperature) / entries_count;
    }
    enums::State state = entries_count > 0 ? enums::State::Enabled : enums::State::UnavailableOffline;

    bool value_changed = (metric.get_value().is_null() == new_temperature.has_value())
                         || (new_temperature.has_value() && metric.get_value().get<uint32_t>() != new_temperature);

    if (value_changed || metric.get_status().get_state() != state) {
        log_debug("telemetry", "Inlet temperature reading changed: "
            << metric.get_value() << " -> " << new_temperature);
        log_debug("telemetry", "Inlet temperature state changed: "
            << metric.get_status().get_state().to_string() << " -> " << state.to_string());
        metric.set_value(new_temperature);
        auto status = metric.get_status();
        status.set_state(state);
        metric.set_status(status);
        return true;
    }
    return false;
}

bool update_tzone(agent_framework::model::ThermalZone&, const DiscoveryContext&,
                  const DiscoveryParams<RmmType::ThermalZone>&) {
    return false;
}

bool update_consumed_power_reading(agent_framework::model::Metric& metric, const DiscoveryContext& dc,
                                   const DiscoveryParams<RmmType::PowerZone>& dp) {
    OptionalField<int> power_consumed{};
    unsigned entries_count = 0;
    log_debug("telemetry", "Power zone consumed power reading update, checking "
        << unsigned(dp.drawer_slots.size()) << " drawers.");

    for (const auto& slot : dp.drawer_slots) {
        log_debug("telemetry", "Checking drawer on slot " << unsigned(slot));
        try {
            response::GetDrawerPower power{};
            dc.ipmi->send(request::GetDrawerPower{},
                agent::rmm::loader::IpmiConfig::get_psme_chassis_bridge(slot), power);
            for (uint8_t i = 0; i < power.get_slot_count(); ++i) {
                if (power.is_valid_value_on_slot(i)) {
                    power_consumed = power_consumed + power.get_power_on_slot(i);
                    ++entries_count;
                }
            }
        }
        catch (const std::exception& e) {
            log_warning("telemetry", "Unable to get power reading for chassis on slot " << unsigned(slot)
                                  << ": " << e.what());
        }
    }
    log_debug("telemetry", "Consumed power: received " << entries_count << " valid responses from drawers.");

    if (metric.get_value().is_null() == power_consumed.has_value()
         || (power_consumed.has_value() && metric.get_value().get<int>() != power_consumed)) {
        log_debug("telemetry", "Power zone consumed watts changed: "
            << metric.get_value() << " -> " << power_consumed);
        metric.set_value(power_consumed);
        return true;
    }
    return false;
}


bool update_pzone(agent_framework::model::PowerZone&, const DiscoveryContext&,
                  const DiscoveryParams<RmmType::PowerZone>&) {
    return false;
}

}

RmmTelemetryService::~RmmTelemetryService() { }

RmmTelemetryService::RmmTelemetryService() : RmmTelemetryServiceInterface(), m_resource_sensors{} {
    // m_resource_sensors vector is now filled in the constructor's body.
    // When it was filled in the initializer list, SonarQube hanged forever during code analysis.

    m_resource_sensors.emplace_back(
        std::make_shared<Sensor<RmmType::ThermalZone>>(
            ResourceInstance{Component::ThermalZone}, INLET_TEMPERATURE, update_inlet_temperature_reading, update_tzone)
    );
    m_resource_sensors.emplace_back(
        std::make_shared<Sensor<RmmType::PowerZone>>(
            ResourceInstance{Component::PowerZone}, CONSUMED_POWER, update_consumed_power_reading, update_pzone)
    );
    for (int fan_index = 0; fan_index < EXPECTED_FAN_SLOTS; ++fan_index) {
        m_resource_sensors.emplace_back(
            std::make_shared<Sensor<RmmType::Fan>>(
                ResourceInstance{ Component::Fan, fan_index }, FAN_READING, update_fan_reading, update_fan)
        );
    }

// Possible extensions :
// std::make_shared<Sensor<RmmType::ThermalZone>>(ResourceInstance{Component::ThermalZone}, AIRFLOW}, ...
// std::make_shared<Sensor<RmmType::PowerZone>>(ResourceInstance{Component::PowerZone}, VOLTAGE}, ...
}



}
}
