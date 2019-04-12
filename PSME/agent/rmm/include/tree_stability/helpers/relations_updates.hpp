/*!
 * @brief Function prototypes for updating relations in the model to be used during tree stabilization.
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
 * @file relations_updates.hpp
 */

#pragma once



#include <string>



namespace agent {
namespace rmm {
namespace helpers {

/*!
 * @brief Update all relations in the model which involve power zone.
 *
 * @param old_uuid Power zone UUID before tree stabilization action.
 * @param new_uuid Power zone UUID after tree stabilization action.
 */
void update_power_zone_in_relations(const std::string& old_uuid, const std::string& new_uuid);


/*!
 * @brief Update all relations in the model which involve thermal zone.
 *
 * @param old_uuid Thermal zone UUID before tree stabilization action.
 * @param new_uuid Thermal zone UUID after tree stabilization action.
 */
void update_thermal_zone_in_relations(const std::string& old_uuid, const std::string& new_uuid);


/*!
 * @brief Update all relations in the model which involve chassis.
 *
 * @param old_uuid Chassis UUID before tree stabilization action.
 * @param new_uuid Chassis UUID after tree stabilization action.
 */
void update_chassis_in_relations(const std::string& old_uuid, const std::string& new_uuid);

}
}
}
