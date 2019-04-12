/*!
 * @brief Chassis builder class implementation.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file chassis_builder.cpp
 */

#include "discovery/builders/fpgaof_chassis_builder.hpp"



using namespace agent::fpgaof::discovery;
using namespace agent_framework;


model::Chassis FpgaofChassisBuilder::build_default(const Uuid& parent_uuid) {
    model::Chassis chassis{parent_uuid};

    chassis.set_status({model::enums::State::Enabled, model::enums::Health::OK});
    chassis.set_type(model::enums::ChassisType::Enclosure);

    return chassis;
}
