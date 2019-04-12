/*!
 * @brief Declarations of database entities.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation
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
 * @file entities.hpp
 */

#pragma once



#include "agent-framework/database/database_entity.hpp"

#include "agent-framework/module/constants/chassis.hpp"
#include "agent-framework/module/constants/pnc.hpp"
#include "agent-framework/module/constants/common.hpp"
#include "agent-framework/module/constants/storage.hpp"
#include "agent-framework/module/constants/compute.hpp"



namespace agent_framework {
namespace database {

using ChassisEntity = DatabaseEntity<agent_framework::model::literals::Chassis::CHASSIS>;
using EndpointEntity = DatabaseEntity<agent_framework::model::literals::Endpoint::ENDPOINT>;
using FabricEntity = DatabaseEntity<agent_framework::model::literals::Fabric::FABRIC>;
using ZoneEntity = DatabaseEntity<agent_framework::model::literals::Zone::ZONE>;
using VolumeEntity = DatabaseEntity<agent_framework::model::literals::Volume::VOLUME>;
using SystemEntity = DatabaseEntity<agent_framework::model::literals::System::SYSTEM>;
using ProcessorEntity = DatabaseEntity<agent_framework::model::literals::Processor::PROCESSOR>;

}
}

