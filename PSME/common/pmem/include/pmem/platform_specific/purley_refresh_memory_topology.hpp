/*!
 * @brief Purley-Refresh Memory Topology related platform specific definitions.
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
 * @file purley_refresh_memory_topology.hpp
 */

#pragma once

#include "agent-framework/module/model/attributes/memory_location.hpp"

namespace pmem {
namespace purley_refresh_topology {

// Memory topology on Purley-R
static constexpr const std::uint8_t NUM_CPUS = 2;                 // Number of CPU sockets
static constexpr const std::uint8_t NUM_IMCS_PER_CPU = 2;         // Number of iMC on each CPU
static constexpr const std::uint8_t NUM_CHANNELS_PER_IMC = 3;     // Number of channels supported on each iMC
static constexpr const std::uint8_t NUM_SLOTS_PER_CHANNEL = 2;    // Number of DIMM slots per channel supported
static constexpr const std::uint8_t DCPMEM_BLOCK_SIZE_BYTES = 64; // Size of a DCPMEM block in bytes

static constexpr const std::uint8_t NUM_DIMMS_PER_IMC = NUM_CHANNELS_PER_IMC * NUM_SLOTS_PER_CHANNEL; // 6 x DIMM/iMC
static constexpr const std::uint8_t NUM_DIMMS_PER_CPU = NUM_IMCS_PER_CPU * NUM_DIMMS_PER_IMC;         // 12 x DIMM/CPU

// Platform specific - QCT TPP: one DIMM slot per channel is supported (physically present)
static constexpr const std::uint8_t NUM_ACTIVE_SLOTS_PER_CHANNEL = 1;
// Platform Specific - QCT TPP: 6 x DIMM/CPU, 12 x DIMM/PLATFORM
static constexpr const std::uint8_t NUM_ACTIVE_SLOTS_PER_CPU = NUM_IMCS_PER_CPU * NUM_CHANNELS_PER_IMC * NUM_ACTIVE_SLOTS_PER_CHANNEL;
static constexpr const std::uint8_t NUM_DIMMS_ON_PLATFORM =
    (NUM_CPUS * NUM_DIMMS_PER_CPU) >> (NUM_ACTIVE_SLOTS_PER_CHANNEL % NUM_SLOTS_PER_CHANNEL);

}
}