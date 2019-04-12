/*!
 * @brief Cpu Dimm temperature reader via Node Manager
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
 * @file cpu_dimm_temperature_telemetry_reader.hpp
 */

#pragma once

#include "telemetry/telemetry_reader.hpp"

namespace telemetry {

class CpuDimmTemperatureTelemetryReader : public TelemetryReader {
public:

    /*!
     * @brief Default constructor
     * @param _resource_key resource "type" to be handled
     * @param _metric_definition metric definition instance to be used
     * @param _reader_id reader Id
     */
    CpuDimmTemperatureTelemetryReader(ResourceInstance _resource_key, MetricDefinition& _metric_definition, ReaderId _reader_id)
        : TelemetryReader(_resource_key, _metric_definition, _reader_id) {}

    /*! Destructor */
    ~CpuDimmTemperatureTelemetryReader() override;

protected:

    static constexpr const int NOT_AVAILABLE = -1;

    template<int NUM_CPUS, int NUM_DIMMS, int NUM_DIMMS_PER_CPU>
    std::pair<int, int> get_cpu_and_dimm() const {
        const auto& component = get_resource_key().get_component();
        const auto instance_number = get_resource_key().get_instance_number();
        if (ResourceInstance::Component::Processor == component
            && instance_number >= 0 && instance_number < NUM_CPUS) {
            return std::make_pair(instance_number, NOT_AVAILABLE);
        }
        else if (ResourceInstance::Component::Memory == component
                 && instance_number >= 0 && instance_number < NUM_DIMMS) {
            return std::make_pair(instance_number / NUM_DIMMS_PER_CPU,
                                  instance_number % NUM_DIMMS_PER_CPU);
        }
        return std::make_pair(NOT_AVAILABLE, NOT_AVAILABLE);
    }

    template<int NUM_CPUS, int NUM_DIMMS, int NUM_DIMMS_PER_CPU>
    bool is_valid_cpu_dimm() const {
        const auto cpu_dimm = get_cpu_and_dimm<NUM_CPUS, NUM_DIMMS, NUM_DIMMS_PER_CPU>();
        const auto& component = get_resource_key().get_component();
        if (ResourceInstance::Component::Processor == component) {
            return (cpu_dimm.first != NOT_AVAILABLE) && (cpu_dimm.second == NOT_AVAILABLE);
        }
        else if (ResourceInstance::Component::Memory == component) {
            return (cpu_dimm.first != NOT_AVAILABLE) && (cpu_dimm.second != NOT_AVAILABLE);
        }
        return false;
    }
};

}
