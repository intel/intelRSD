/*!
 * @brief Purley-Refresh Memory Topology / Mapping related platform specific definitions.
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
 * @file purley_refresh_memory_mapping.hpp
 */

#include <unordered_map>

namespace pmem {
namespace purley_refresh_topology {

/* QCT TPP (Purley-R) Memory Location Mapping:
 * SMBIOS Type 17 Memory Device :: Device Locator -> MemoryLocation mapping
 * MemoryLocation {Socket, Controller, Channel, Slot}
 * */
std::unordered_map<std::string, agent_framework::model::attribute::MemoryLocation> memory_location_map {
    {"CPU0_DIMM_A0", agent_framework::model::attribute::MemoryLocation{0, 0, 0, 0}},
    {"CPU0_DIMM_A1", agent_framework::model::attribute::MemoryLocation{0, 0, 1, 0}},
    {"CPU0_DIMM_A2", agent_framework::model::attribute::MemoryLocation{0, 0, 2, 0}},
    {"CPU0_DIMM_B0", agent_framework::model::attribute::MemoryLocation{0, 1, 0, 0}},
    {"CPU0_DIMM_B1", agent_framework::model::attribute::MemoryLocation{0, 1, 1, 0}},
    {"CPU0_DIMM_B2", agent_framework::model::attribute::MemoryLocation{0, 1, 2, 0}},
    {"CPU1_DIMM_C0", agent_framework::model::attribute::MemoryLocation{1, 0, 0, 0}},
    {"CPU1_DIMM_C1", agent_framework::model::attribute::MemoryLocation{1, 0, 1, 0}},
    {"CPU1_DIMM_C2", agent_framework::model::attribute::MemoryLocation{1, 0, 2, 0}},
    {"CPU1_DIMM_D0", agent_framework::model::attribute::MemoryLocation{1, 1, 0, 0}},
    {"CPU1_DIMM_D1", agent_framework::model::attribute::MemoryLocation{1, 1, 1, 0}},
    {"CPU1_DIMM_D2", agent_framework::model::attribute::MemoryLocation{1, 1, 2, 0}}
};

}
}