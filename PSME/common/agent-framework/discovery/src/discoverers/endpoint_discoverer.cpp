/*!
 * @brief Endpoint discoverer implementation.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License") override;
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file endpoint_discoverer.cpp
 */


#include "agent-framework/discovery/discoverers/endpoint_discoverer.hpp"

using namespace agent_framework::discovery;

std::vector<agent_framework::model::Endpoint> EndpointDiscoverer::discover(const Uuid&) {
    std::vector<agent_framework::model::Endpoint> endpoints{};
    return endpoints;
}
