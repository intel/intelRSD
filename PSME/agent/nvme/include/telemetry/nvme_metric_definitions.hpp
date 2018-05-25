/*!
 * @brief Nvme telemetry's internal metric definitions.
 *
 * @copyright Copyright (c) 2017-2018 Intel Corporation
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
 * @header{Files}
 * @file nvme_metric_definitions.hpp
 */

#pragma once

#include "telemetry/metric_definition_builder.hpp"

namespace agent {
namespace nvme {
namespace telemetry {

agent_framework::model::MetricDefinition TEMPERATURE_KELVIN =
    ::telemetry::MetricDefinitionBuilder("/TemperatureKelvin")
            .set_name("driveTemperatureKelvin")
            .set_is_linear(true)
            .set_metric_type(agent_framework::model::enums::MetricType::Numeric)
            .set_data_type(agent_framework::model::enums::MetricDataType::Decimal)
            .set_sensor_type(agent_framework::model::enums::SensorType::Temperature)
            .set_units("Kelvin")
            .set_physical_context(agent_framework::model::enums::PhysicalContext::StorageDevice)
            .build();

agent_framework::model::MetricDefinition UNIT_SIZE_BYTES =
    ::telemetry::MetricDefinitionBuilder("/LifeTime/UnitSizeBytes")
            .set_name("driveUnitSize")
            .set_is_linear(true)
            .set_units("Bytes")
            .set_data_type(agent_framework::model::enums::MetricDataType::Int64)
            .set_metric_type(agent_framework::model::enums::MetricType::Numeric)
            .set_physical_context(agent_framework::model::enums::PhysicalContext::StorageDevice)
            .build();

agent_framework::model::MetricDefinition UNITS_READ =
    ::telemetry::MetricDefinitionBuilder("/LifeTime/UnitsRead")
            .set_name("driveUnitsRead")
            .set_is_linear(true)
            .set_data_type(agent_framework::model::enums::MetricDataType::Decimal)
            .set_metric_type(agent_framework::model::enums::MetricType::Numeric)
            .set_physical_context(agent_framework::model::enums::PhysicalContext::StorageDevice)
            .build();

agent_framework::model::MetricDefinition UNITS_WRITTEN =
    ::telemetry::MetricDefinitionBuilder("/LifeTime/UnitsWritten")
            .set_name("driveUnitsWritten")
            .set_is_linear(true)
            .set_data_type(agent_framework::model::enums::MetricDataType::Decimal)
            .set_metric_type(agent_framework::model::enums::MetricType::Numeric)
            .set_physical_context(agent_framework::model::enums::PhysicalContext::StorageDevice)
            .build();

agent_framework::model::MetricDefinition HOST_READ_COMMANDS =
    ::telemetry::MetricDefinitionBuilder("/LifeTime/HostReadCommands")
            .set_name("driveHostReadCommands")
            .set_is_linear(true)
            .set_data_type(agent_framework::model::enums::MetricDataType::Decimal)
            .set_metric_type(agent_framework::model::enums::MetricType::Numeric)
            .set_physical_context(agent_framework::model::enums::PhysicalContext::StorageDevice)
            .build();

agent_framework::model::MetricDefinition HOST_WRITE_COMMANDS =
    ::telemetry::MetricDefinitionBuilder("/LifeTime/HostWriteCommands")
            .set_name("driveHostWriteCommands")
            .set_is_linear(true)
            .set_data_type(agent_framework::model::enums::MetricDataType::Decimal)
            .set_metric_type(agent_framework::model::enums::MetricType::Numeric)
            .set_physical_context(agent_framework::model::enums::PhysicalContext::StorageDevice)
            .build();

agent_framework::model::MetricDefinition POWER_CYCLES =
    ::telemetry::MetricDefinitionBuilder("/LifeTime/PowerCycles")
            .set_name("drivePowerCycles")
            .set_is_linear(true)
            .set_data_type(agent_framework::model::enums::MetricDataType::Decimal)
            .set_metric_type(agent_framework::model::enums::MetricType::Numeric)
            .set_physical_context(agent_framework::model::enums::PhysicalContext::StorageDevice)
            .build();

agent_framework::model::MetricDefinition POWER_ON_HOURS =
    ::telemetry::MetricDefinitionBuilder("/LifeTime/PowerOnHours")
            .set_name("drivePowerOnHours")
            .set_is_linear(true)
            .set_data_type(agent_framework::model::enums::MetricDataType::Decimal)
            .set_metric_type(agent_framework::model::enums::MetricType::Numeric)
            .set_physical_context(agent_framework::model::enums::PhysicalContext::StorageDevice)
            .build();

agent_framework::model::MetricDefinition CONTROLLER_BUSY_TIME_MINUTES =
    ::telemetry::MetricDefinitionBuilder("/LifeTime/ControllerBusyTimeMinutes")
            .set_name("driveControllerBusyTimeMinutes")
            .set_is_linear(true)
            .set_data_type(agent_framework::model::enums::MetricDataType::Decimal)
            .set_metric_type(agent_framework::model::enums::MetricType::Numeric)
            .set_physical_context(agent_framework::model::enums::PhysicalContext::StorageDevice)
            .build();

agent_framework::model::MetricDefinition AVAILABLE_SPARE_PERCENTAGE =
    ::telemetry::MetricDefinitionBuilder("/HealthData/AvailableSparePercentage")
            .set_name("driveAvailableSparePercentage")
            .set_is_linear(true)
            .set_data_type(agent_framework::model::enums::MetricDataType::Decimal)
            .set_metric_type(agent_framework::model::enums::MetricType::Numeric)
            .set_physical_context(agent_framework::model::enums::PhysicalContext::StorageDevice)
            .build();

agent_framework::model::MetricDefinition PREDICTED_MEDIA_LIFE_USED_PERCENT =
    ::telemetry::MetricDefinitionBuilder("/HealthData/PredictedMediaLifeUsedPercent")
            .set_name("drivePredictedMediaLifeUsedPercent")
            .set_is_linear(true)
            .set_data_type(agent_framework::model::enums::MetricDataType::Decimal)
            .set_metric_type(agent_framework::model::enums::MetricType::Numeric)
            .set_physical_context(agent_framework::model::enums::PhysicalContext::StorageDevice)
            .build();

agent_framework::model::MetricDefinition UNSAFE_SHUTDOWNS =
    ::telemetry::MetricDefinitionBuilder("/HealthData/UnsafeShutdowns")
            .set_name("driveUnsafeShutdowns")
            .set_is_linear(true)
            .set_data_type(agent_framework::model::enums::MetricDataType::Decimal)
            .set_metric_type(agent_framework::model::enums::MetricType::Numeric)
            .set_physical_context(agent_framework::model::enums::PhysicalContext::StorageDevice)
            .build();

agent_framework::model::MetricDefinition MEDIA_ERRORS =
    ::telemetry::MetricDefinitionBuilder("/HealthData/MediaErrors")
            .set_name("driveMediaErrors")
            .set_is_linear(true)
            .set_data_type(agent_framework::model::enums::MetricDataType::Decimal)
            .set_metric_type(agent_framework::model::enums::MetricType::Numeric)
            .set_physical_context(agent_framework::model::enums::PhysicalContext::StorageDevice)
            .build();

}  // namespace telemetry
}  // namespace nvme
}  // namespace agent
