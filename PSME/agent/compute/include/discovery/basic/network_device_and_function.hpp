/*!
 * @brief Declarations of functions creating Network Devices and Functions for Compute agent
 *
 * @header{License}
 * @copyright Copyright (c) 2017 Intel Corporation.
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
 * @header{Filesystem}
 * @file network_device_and_function.hpp
 */
#pragma once
#include "agent-framework/module/compute_components.hpp"

namespace ipmi {
class ManagementController;
}

namespace agent {
namespace compute {
namespace discovery {
namespace basic {

/*!
 * @brief Discovers network device and function using IPMI commands
 *
 * @param[in] mc Reference to IPMI connector
 * @param[in] system reference to system model to be populated with data
 * @return tuple that tells the caller discovery status and health status to be set on the resource
 */
std::tuple<bool, agent_framework::model::enums::Health> discover_network_device(ipmi::ManagementController& mc,
                             agent_framework::model::System& system);

}
}
}
}
