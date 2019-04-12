/*!
 * @brief Implementation of discovery initiator.
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
 * @file discovery_initiator.cpp
 */

#include "discovery/discovery_initiator.hpp"

using namespace agent::nvme::discovery;
using namespace agent_framework::model;

DiscoveryInitiator::DiscoveryInitiator() {}
DiscoveryInitiator::~DiscoveryInitiator() {}

void DiscoveryInitiator::discover(const Uuid& manager_uuid, const Uuid& fabric_uuid) {
    (void) manager_uuid;
    (void) fabric_uuid;
    // TODO do initiator only discovery
}
