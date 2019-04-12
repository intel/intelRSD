/*!
 * @brief Intel Optane DC Persistent Memory firmware commands definitions
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file dcpmem_commands.hpp
 */

#pragma once

#include <cstdint>

namespace ipmi {
namespace command {
namespace sdv {

static constexpr const unsigned CHUNK_SIZE = 64; // Read chunk size (in bytes)

struct DcpmemDimmLocation {
    constexpr DcpmemDimmLocation(std::uint8_t cpu, std::uint8_t smbus, std::uint8_t dimm)
        : cpu_number(cpu), smbus_id(smbus), dimm_index(dimm) {}

    constexpr bool operator==(const DcpmemDimmLocation& rhs) const {
        return ((cpu_number == rhs.cpu_number) && (smbus_id == rhs.smbus_id) && (dimm_index == rhs.dimm_index));
    }

    std::uint8_t cpu_number{};
    std::uint8_t smbus_id{};
    std::uint8_t dimm_index{};
};

enum class DcpmemCommandType : uint8_t {
    UNKNOWN = 0,
    IDENTIFY_DIMM,
    GET_SECURITY_STATE,
    GET_POWER_MANAGEMENT_POLICY,
    GET_DIMM_PARTITION_INFO,
    GET_SMART_INFO,
    GET_MEMORY_INFO
};

struct DcpmemCommand {
    constexpr DcpmemCommand(DcpmemCommandType type, std::uint8_t oc, std::uint8_t suboc, std::uint8_t len = CHUNK_SIZE)
        : command_type(type), opcode(oc), subopcode(suboc), length(len) {}

    DcpmemCommandType command_type{DcpmemCommandType::UNKNOWN};
    std::uint8_t opcode{};           // DCPMEM FW command Opcode
    std::uint8_t subopcode{};        // DCPMEM FW command Sub-Opcode
    std::uint8_t length{CHUNK_SIZE}; // Length of command response (aligned to CHUNK_SIZE = 64B)
};

/*!
 * Supported DCPMEM FW commands
 */
constexpr DcpmemCommand DCPMEM_IDENTIFY_DIMM               = DcpmemCommand(DcpmemCommandType::IDENTIFY_DIMM, 0x01, 0x00, 128);
constexpr DcpmemCommand DCPMEM_GET_SECURITY_STATE          = DcpmemCommand(DcpmemCommandType::GET_SECURITY_STATE, 0x02, 0x00);
constexpr DcpmemCommand DCPMEM_GET_POWER_MANAGEMENT_POLICY = DcpmemCommand(DcpmemCommandType::GET_POWER_MANAGEMENT_POLICY, 0x04, 0x02);
constexpr DcpmemCommand DCPMEM_GET_DIMM_PARTITION_INFO     = DcpmemCommand(DcpmemCommandType::GET_DIMM_PARTITION_INFO, 0x06, 0x02);
constexpr DcpmemCommand DCPMEM_GET_SMART_INFO              = DcpmemCommand(DcpmemCommandType::GET_SMART_INFO, 0x08, 0x00, 128);
constexpr DcpmemCommand DCPMEM_GET_MEMORY_INFO             = DcpmemCommand(DcpmemCommandType::GET_MEMORY_INFO, 0x08, 0x03);
}
}
}