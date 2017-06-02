/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file memory.hpp
 *
 * @brief ipmi-way of discovering memory properties
 * */

#pragma once

#include "agent-framework/module/model/system.hpp"

namespace ipmi {
class ManagementController;
}

namespace agent {
namespace compute {
namespace discovery {
namespace basic {

/*!
 * @brief Discovers Memory modules using IPMI GetDimmInfo command
 *
 * @param[in] mc Reference to IPMI connector
 * @param[in] system reference to system model to be populated with data
 * @return tuple that tells the caller discovery status and health status to be set on the resource
 */
std::tuple<bool, agent_framework::model::enums::Health> discover_memory_modules(ipmi::ManagementController& mc,
                                                                              agent_framework::model::System& system);
}
}
}
}
