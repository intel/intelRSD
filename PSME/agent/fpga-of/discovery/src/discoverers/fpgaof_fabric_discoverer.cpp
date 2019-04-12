/*!
 * @brief FPGA-oF fabric discoverer implementation.
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
 * @file fpgaof_fabric_discoverer.cpp
 */

#include "agent-framework/discovery/builders/fabric_builder.hpp"
#include "discovery/discoverers/fpgaof_fabric_discoverer.hpp"

using namespace agent::fpgaof::discovery;

agent_framework::model::Fabric FpgaofFabricDiscoverer::discover(const Uuid& parent_uuid) {
    auto fabric = agent_framework::discovery::FabricDiscoverer::discover(parent_uuid);
    agent_framework::discovery::FabricBuilder::fpgaof_protocol(fabric);
    return fabric;
}

