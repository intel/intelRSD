/*!
 * @brief Thermal zone builder class implementation
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
 * @file thermal_zone_builder.cpp
 */

#include "discovery/builders/thermal_zone_builder.hpp"



using namespace agent::compute::discovery;
using namespace agent_framework::model;


agent_framework::model::ThermalZone ThermalZoneBuilder::build_default(const std::string& parent_uuid) {
    ThermalZone zone{parent_uuid};

    zone.add_collection({enums::CollectionName::Fans,
                         enums::CollectionType::Fans
    });
    zone.set_status({enums::State::Enabled, {}});
    return zone;
}
