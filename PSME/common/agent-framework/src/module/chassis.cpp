/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 *
 * @section DESCRIPTION
*/

#include "agent-framework/module/chassis.hpp"

#include "json/json.hpp"

using namespace agent_framework::generic;

Chassis::~Chassis() {}

void Chassis::read_configuration(const json::Value& chassis_configuration) {
    try {
        log_debug(GET_LOGGER("module"), "Reading chassis configuration.");
        if (chassis_configuration["type"].is_string()) {
            m_type = chassis_configuration["type"].as_string();
        }
        if (chassis_configuration["size"].is_uint()) {
            m_size = chassis_configuration["size"].as_uint();
        }
        if (chassis_configuration["locationOffset"].is_uint()) {
            m_location_offset = chassis_configuration["locationOffset"].as_uint();
        }

        m_read_compute_zones(chassis_configuration);
    }
    catch (const json::Value::Exception& e) {
        log_warning(GET_LOGGER("module"), "Invalid/missing chassis configuration member: "
                  << e.what());
    }
    catch (...) {
        log_alert(GET_LOGGER("module"), "Unknown error in chassis section");
    }
}

void Chassis::m_read_compute_zones(const json::Value& chassis_configuration) {
    if (chassis_configuration["computeZones"].is_array()) {
        for (const auto& zone_configuration : chassis_configuration["computeZones"]) {
            ComputeZone::ComputeZoneSharedPtr compute_zone
                = std::make_shared<ComputeZone>();
            compute_zone->read_configuration(zone_configuration);
            m_compute_zones.push_back(compute_zone);
        }
    }
}
