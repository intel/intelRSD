/*!
 * @brief Implementation of Fpga databases classes.
 *
 * @copyright Copyright (c) 2019 Intel Corporation
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
 * @file databases.cpp
 */

#include "tools/databases.hpp"
#include "agent-framework/module/constants/common.hpp"
#include "agent-framework/module/constants/pnc.hpp"
#include "agent-framework/module/constants/storage.hpp"
#include "agent-framework/module/constants/chassis.hpp"

using namespace agent::fpga_discovery::tools;
using namespace agent_framework::model;

ZoneDatabase::ZoneDatabase(const Uuid& zone) : FpgaDiscoveryDatabase(zone, literals::Zone::ZONE) {}

FabricDatabase::FabricDatabase(const Uuid& fabric) : FpgaDiscoveryDatabase(fabric, literals::Fabric::FABRIC) {}

EndpointDatabase::EndpointDatabase(const Uuid& endpoint) : FpgaDiscoveryDatabase(endpoint, literals::Endpoint::ENDPOINT) {}

VolumeDatabase::VolumeDatabase(const Uuid& volume) : FpgaDiscoveryDatabase(volume, literals::Volume::VOLUME) {}

ChassisDatabase::ChassisDatabase(const Uuid& chassis) : FpgaDiscoveryDatabase(chassis, literals::Chassis::CHASSIS) {}
