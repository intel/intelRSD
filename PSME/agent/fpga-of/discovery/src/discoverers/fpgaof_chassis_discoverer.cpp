/*!
 * @brief Chassis discoverer implementation.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License") override;
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file chassis_discoverer.cpp
 */

#include "discovery/discoverers/fpgaof_chassis_discoverer.hpp"
#include "discovery/builders/fpgaof_chassis_builder.hpp"


using namespace agent::fpgaof::discovery;

agent_framework::model::Chassis FpgaofChassisDiscoverer::discover(const Uuid& parent_uuid) {
    auto chassis = FpgaofChassisBuilder::build_default(parent_uuid);
    return chassis;
}
