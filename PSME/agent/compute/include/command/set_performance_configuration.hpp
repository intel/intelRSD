/*!
 * @brief Declaration of function for handling set Performance Configuration operation
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
 * @file set_performance_configuration.hpp
 */

#pragma once

#include "agent-framework/command/compute_commands.hpp"


namespace agent {
namespace compute {

struct PerformanceConfigurationAttributes {
    std::uint8_t node_index{};
    std::uint8_t cpu_index{};
    std::uint8_t configuration_index{};
};

/*!
 * @brief Implementation of SetComponentAttributes for set Performance Configuration operation
 * @param[in] system_uuid the uuid of the System
 * @param[in] performance_configuration_attributes the performance configuration to be set
 * @return Created task UUID
 */
Uuid process_performance_configuration(const Uuid& system_uuid, const PerformanceConfigurationAttributes& performance_configuration_attributes);

}
}
