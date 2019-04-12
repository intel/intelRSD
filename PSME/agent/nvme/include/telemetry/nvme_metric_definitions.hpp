/*!
 * @brief Nvme telemetry's internal metric definitions.
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
 * @file nvme_metric_definitions.hpp
 */

#pragma once



#include "nvme_constants.hpp"
#include "telemetry/metric_definition_builder.hpp"
#include "nvme_telemetry_service.hpp"



namespace agent {
namespace nvme {
namespace telemetry {

agent_framework::model::MetricDefinition TEMPERATURE_KELVIN =
    ::telemetry::MetricDefinitionBuilder(Constants::TEMPERATURE_KELVIN_JSON_PTR)
        .set_name("driveTemperatureKelvin")
        .set_is_linear(true)
        .set_metric_type(agent_framework::model::enums::MetricType::Numeric)
        .set_data_type(agent_framework::model::enums::MetricDataType::Decimal)
        .set_sensor_type(agent_framework::model::enums::SensorType::Temperature)
        .set_units("Kelvin")
        .set_physical_context(agent_framework::model::enums::PhysicalContext::StorageDevice)
        .build();

agent_framework::model::MetricDefinition UNIT_SIZE_BYTES =
    ::telemetry::MetricDefinitionBuilder(Constants::UNIT_SIZE_BYTES_JSON_PTR)
        .set_name("driveUnitSize")
        .set_is_linear(true)
        .set_units("Bytes")
        .set_data_type(agent_framework::model::enums::MetricDataType::Int64)
        .set_metric_type(agent_framework::model::enums::MetricType::Numeric)
        .set_physical_context(agent_framework::model::enums::PhysicalContext::StorageDevice)
        .build();

agent_framework::model::MetricDefinition UNITS_READ =
    ::telemetry::MetricDefinitionBuilder(Constants::UNITS_READ_JSON_PTR)
        .set_name("driveUnitsRead")
        .set_is_linear(true)
        .set_data_type(agent_framework::model::enums::MetricDataType::Decimal)
        .set_metric_type(agent_framework::model::enums::MetricType::Numeric)
        .set_physical_context(agent_framework::model::enums::PhysicalContext::StorageDevice)
        .build();

agent_framework::model::MetricDefinition UNITS_WRITTEN =
    ::telemetry::MetricDefinitionBuilder(Constants::UNITS_WRITTEN_JSON_PTR)
        .set_name("driveUnitsWritten")
        .set_is_linear(true)
        .set_data_type(agent_framework::model::enums::MetricDataType::Decimal)
        .set_metric_type(agent_framework::model::enums::MetricType::Numeric)
        .set_physical_context(agent_framework::model::enums::PhysicalContext::StorageDevice)
        .build();

agent_framework::model::MetricDefinition HOST_READ_COMMANDS =
    ::telemetry::MetricDefinitionBuilder(Constants::HOST_READ_COMMANDS_JSON_PTR)
        .set_name("driveHostReadCommands")
        .set_is_linear(true)
        .set_data_type(agent_framework::model::enums::MetricDataType::Decimal)
        .set_metric_type(agent_framework::model::enums::MetricType::Numeric)
        .set_physical_context(agent_framework::model::enums::PhysicalContext::StorageDevice)
        .build();

agent_framework::model::MetricDefinition HOST_WRITE_COMMANDS =
    ::telemetry::MetricDefinitionBuilder(Constants::HOST_WRITE_COMMANDS_JSON_PTR)
        .set_name("driveHostWriteCommands")
        .set_is_linear(true)
        .set_data_type(agent_framework::model::enums::MetricDataType::Decimal)
        .set_metric_type(agent_framework::model::enums::MetricType::Numeric)
        .set_physical_context(agent_framework::model::enums::PhysicalContext::StorageDevice)
        .build();

agent_framework::model::MetricDefinition POWER_CYCLES =
    ::telemetry::MetricDefinitionBuilder(Constants::POWER_CYCLES_JSON_PTR)
        .set_name("drivePowerCycles")
        .set_is_linear(true)
        .set_data_type(agent_framework::model::enums::MetricDataType::Decimal)
        .set_metric_type(agent_framework::model::enums::MetricType::Numeric)
        .set_physical_context(agent_framework::model::enums::PhysicalContext::StorageDevice)
        .build();

agent_framework::model::MetricDefinition POWER_ON_HOURS =
    ::telemetry::MetricDefinitionBuilder(Constants::POWER_ON_HOURS_JSON_PTR)
        .set_name("drivePowerOnHours")
        .set_is_linear(true)
        .set_data_type(agent_framework::model::enums::MetricDataType::Decimal)
        .set_metric_type(agent_framework::model::enums::MetricType::Numeric)
        .set_physical_context(agent_framework::model::enums::PhysicalContext::StorageDevice)
        .build();

agent_framework::model::MetricDefinition CONTROLLER_BUSY_TIME_MINUTES =
    ::telemetry::MetricDefinitionBuilder(Constants::CONTROLLER_BUSY_TIME_MINUTES_JSON_PTR)
        .set_name("driveControllerBusyTimeMinutes")
        .set_is_linear(true)
        .set_data_type(agent_framework::model::enums::MetricDataType::Decimal)
        .set_metric_type(agent_framework::model::enums::MetricType::Numeric)
        .set_physical_context(agent_framework::model::enums::PhysicalContext::StorageDevice)
        .build();

agent_framework::model::MetricDefinition AVAILABLE_SPARE_PERCENTAGE =
    ::telemetry::MetricDefinitionBuilder(Constants::AVAILABLE_SPARE_PERCENTAGE_JSON_PTR)
        .set_name("driveAvailableSparePercentage")
        .set_is_linear(true)
        .set_data_type(agent_framework::model::enums::MetricDataType::Decimal)
        .set_metric_type(agent_framework::model::enums::MetricType::Numeric)
        .set_physical_context(agent_framework::model::enums::PhysicalContext::StorageDevice)
        .build();

agent_framework::model::MetricDefinition PREDICTED_MEDIA_LIFE_USED_PERCENT =
    ::telemetry::MetricDefinitionBuilder(Constants::PREDICTED_MEDIA_LIFE_USED_PERCENT_JSON_PTR)
        .set_name("drivePredictedMediaLifeUsedPercent")
        .set_is_linear(true)
        .set_data_type(agent_framework::model::enums::MetricDataType::Decimal)
        .set_metric_type(agent_framework::model::enums::MetricType::Numeric)
        .set_physical_context(agent_framework::model::enums::PhysicalContext::StorageDevice)
        .build();

agent_framework::model::MetricDefinition UNSAFE_SHUTDOWNS =
    ::telemetry::MetricDefinitionBuilder(Constants::UNSAFE_SHUTDOWNS_JSON_PTR)
        .set_name("driveUnsafeShutdowns")
        .set_is_linear(true)
        .set_data_type(agent_framework::model::enums::MetricDataType::Decimal)
        .set_metric_type(agent_framework::model::enums::MetricType::Numeric)
        .set_physical_context(agent_framework::model::enums::PhysicalContext::StorageDevice)
        .build();

agent_framework::model::MetricDefinition MEDIA_ERRORS =
    ::telemetry::MetricDefinitionBuilder(Constants::MEDIA_ERRORS_JSON_PTR)
        .set_name("driveMediaErrors")
        .set_is_linear(true)
        .set_data_type(agent_framework::model::enums::MetricDataType::Decimal)
        .set_metric_type(agent_framework::model::enums::MetricType::Numeric)
        .set_physical_context(agent_framework::model::enums::PhysicalContext::StorageDevice)
        .build();

agent_framework::model::MetricDefinition LATENCY_BUCKETS =
    ::telemetry::MetricDefinitionBuilder(Constants::LATENCY_BUCKETS_JSON_PTR)
        .set_name("driveLatencyTrackingHistogramBuckets")
        .set_is_linear(true)
        .set_data_type(agent_framework::model::enums::MetricDataType::Int32)
        .set_metric_type(agent_framework::model::enums::MetricType::Numeric)
        .set_physical_context(agent_framework::model::enums::PhysicalContext::StorageDevice)
        .set_precision(1)
        .set_min_reading_range(0)
        .set_sensing_interval("PT" + std::to_string(Constants::TELEMETRY_DELAY.count()) + "S")
        .set_metric_calculable(agent_framework::model::enums::MetricCalculable::Summable)
        .set_implementation(agent_framework::model::enums::MetricImplementation::PhysicalSensor)
        .build();

}  // namespace telemetry
}  // namespace nvme
}  // namespace agent
