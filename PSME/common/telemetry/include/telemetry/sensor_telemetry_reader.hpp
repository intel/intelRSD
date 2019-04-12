/*!
 * @brief Telemetry reader for plain SDR sensor
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
 * @file sensor_telemetry_reader.hpp
 */

#pragma once

#include "telemetry/telemetry_reader.hpp"

#include <unordered_map>

namespace telemetry {

class SensorTelemetryReader final : public TelemetryReader {
public:
    /*!
     * @brief Get reader id for the sensor readers (statically assigned)
     * @return reader id
     */
    static ReaderId assigned_reader_id();

    using SensorNumber = std::uint8_t;
    using EntityId = std::uint8_t;
    using EntityInstance = std::uint8_t;

    SensorTelemetryReader(ResourceInstance _resource_key, MetricDefinition& _metric_definition,
                          SensorNumber _sensor_number, EntityId _entity_id, EntityInstance _entity_instance) :
        TelemetryReader(_resource_key, _metric_definition, assigned_reader_id()),
        sensor_number(_sensor_number), entity_id(_entity_id), entity_instance(_entity_instance) {}

    std::string get_info() const override;


    SensorNumber get_sensor_number() const { return sensor_number; }

    EntityId get_entity_id() const { return entity_id; }

    EntityInstance get_entity_instance() const { return entity_instance; }

protected:
    Context::Ptr create_context(ipmi::IpmiController& ctrl, PtrVector& readers) override;

    /*! Check if sensor definition matches the SDR configuration */
    bool is_valid(Context::Ptr context) const override;

    /*! Read all sensors to be read */
    bool read(Context::Ptr context, ipmi::IpmiController& ctrl) override;

private:
    const SensorNumber sensor_number;
    const EntityId entity_id;
    const EntityInstance entity_instance;
};

}
