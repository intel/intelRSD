/*!
 * @brief Definition of PerformanceConfiguration attribute class.
 *
 * @copyright Copyright (c) 2019 Intel Corporation.
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
 * @file performance_configuration.cpp
 */

#include "agent-framework/module/model/attributes/performance_configuration.hpp"
#include "agent-framework/module/constants/compute.hpp"

using namespace agent_framework::model::attribute;
using namespace agent_framework::model;

PerformanceConfiguration::~PerformanceConfiguration() {}

json::Json PerformanceConfiguration::to_json() const {
    json::Json json = json::Json();
    json[literals::PerformanceConfiguration::CONFIGURATION_ID] = m_configuration_id;
    json[literals::PerformanceConfiguration::TYPE] = m_type;
    json[literals::PerformanceConfiguration::HIGH_PRIORITY_CORE_COUNT] = m_high_priority_core_count;
    json[literals::PerformanceConfiguration::LOW_PRIORITY_CORE_COUNT] = m_low_priority_core_count;
    json[literals::PerformanceConfiguration::HIGH_PRIORITY_BASE_FREQUENCY] = m_high_priority_base_frequency;
    json[literals::PerformanceConfiguration::LOW_PRIORITY_BASE_FREQUENCY] = m_low_priority_base_frequency;
    json[literals::PerformanceConfiguration::ACTIVE_CORES] = m_active_cores;
    json[literals::PerformanceConfiguration::BASE_CORE_FREQUENCY] = m_base_core_frequency;
    json[literals::PerformanceConfiguration::TDP] = m_tdp;
    json[literals::PerformanceConfiguration::MAX_JUNCTION_TEMP_CELSIUS] = m_max_junction_temp_celsius;
    return json;
}

PerformanceConfiguration PerformanceConfiguration::from_json(const json::Json& json) {
    PerformanceConfiguration configuration{};
    configuration.set_configuration_id(json[literals::PerformanceConfiguration::CONFIGURATION_ID]);
    configuration.set_type(json[literals::PerformanceConfiguration::TYPE]);
    configuration.set_high_priority_core_count(json[literals::PerformanceConfiguration::HIGH_PRIORITY_CORE_COUNT]);
    configuration.set_low_priority_core_count(json[literals::PerformanceConfiguration::LOW_PRIORITY_CORE_COUNT]);
    configuration.set_high_priority_base_frequency(json[literals::PerformanceConfiguration::HIGH_PRIORITY_BASE_FREQUENCY]);
    configuration.set_low_priority_base_frequency(json[literals::PerformanceConfiguration::LOW_PRIORITY_BASE_FREQUENCY]);
    configuration.set_active_cores(json[literals::PerformanceConfiguration::ACTIVE_CORES]);
    configuration.set_base_core_frequency(json[literals::PerformanceConfiguration::BASE_CORE_FREQUENCY]);
    configuration.set_tdp(json[literals::PerformanceConfiguration::TDP]);
    configuration.set_max_junction_temp_celsius(json[literals::PerformanceConfiguration::MAX_JUNCTION_TEMP_CELSIUS]);
    return configuration;
}