/*!
 * @brief Purley platform chassis builder class interface.
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
 * @file purley_chassis_builder.hpp
 */

#pragma once



#include "discovery/builders/chassis_builder.hpp"
#include "ipmi/command/sdv/get_slot_id.hpp"



namespace agent {
namespace compute {
namespace discovery {

class PurleyChassisBuilder : public ChassisBuilder {
public:
    /*!
     * @brief Update chassis object with GetSlotId IPMI command response.
     * @param chassis Chassis object to be filled with discovered data.
     * @param get_slot_id_response GetSlotId IPMI command response object.
     */
    static void update_slot_id(agent_framework::model::Chassis& chassis,
                               const ipmi::command::sdv::response::GetSlotId& get_slot_id_response);
};

}
}
}
