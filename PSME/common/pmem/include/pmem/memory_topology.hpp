/*!
 * @brief Memory Topology related utility functions.
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
 * @file memory_topology.hpp
 */

#pragma once

#include "pmem/platform_specific/purley_refresh_memory_topology.hpp"
#include "agent-framework/module/model/attributes/memory_location.hpp"
#include "ipmi/utils/sdv/dcpmem_commands.hpp"

#include <memory>
#include <unordered_map>

namespace pmem {

class MemoryTopology {

    struct MemoryTopologyParameters {
        std::uint8_t num_cpus;
        std::uint8_t num_imcs_per_cpu;
        std::uint8_t num_channels_per_imc;
        std::uint8_t num_slots_per_channel;
        std::uint8_t num_active_slots_per_channel;

        std::uint8_t get_num_dimms_per_imc() const {
            return (std::uint8_t)(num_channels_per_imc * num_slots_per_channel);
        }

        std::uint8_t get_num_dimms_per_cpu() const {
            return (std::uint8_t)(num_imcs_per_cpu * get_num_dimms_per_imc());
        }

        std::uint8_t get_num_dimms_on_platform() const {
            return (std::uint8_t)((num_cpus * get_num_dimms_per_cpu()) >> (num_active_slots_per_channel *  num_slots_per_channel));
        }

        std::uint8_t get_num_active_slots_per_cpu() const {
            return (std::uint8_t)(num_imcs_per_cpu * num_channels_per_imc * num_active_slots_per_channel);
        }
    };

public:
    using MemoryLocationMapping = std::unordered_map<std::string, agent_framework::model::attribute::MemoryLocation>;

    /*!
     * @brief Gets Memory Location (socket, controller, channel, slot) for a given Device Locator
     * @param device_locator SMBIOS (Type 17) Device Locator property
     * @return MemoryLocation object with memory topology
     */
    static OptionalField<agent_framework::model::attribute::MemoryLocation> get_memory_location_for_locator(const OptionalField<std::string>& device_locator);

    /*!
     * @brief Gets Memory Resource Instance for a given DCPMEM DIMM Location (CPU Number, SMBUS Identifier, DIMM Index)
     * @param location DCPMEM DIMM Location object used in communication with SPS Node Manager
     * @return Memory Resource Instance for a given location (0 based)
     */
    static int get_memory_instance_for_dcpmem_location(const ipmi::command::sdv::DcpmemDimmLocation& location);

    /*!
     * @brief Gets DCPMEM DIMM Location (CPU Number, SMBUS Identifier, DIMM Index) for a given Memory Resource Instance
     * @param memory_instance Memory Resource Instance for a given location (0 based)
     * @return DCPMEM DIMM Location object used in communication with SPS Node Manager
     */
    static ipmi::command::sdv::DcpmemDimmLocation get_dcpmem_location_for_memory_instance(const int memory_instance);

    /*!
     * @brief Gets all DCPMEM DIMM Locations present in a System for a given Memory Mapping (Platform Specific)
     * @param location_map Memory Location Mapping (Device Locator -> Memory Location)
     * @return Container of DCPMEM DIMM Location objects translated from Memory Location objects
     */
    static std::vector<ipmi::command::sdv::DcpmemDimmLocation> get_dcpmem_locations_for_memory_mapping(const MemoryLocationMapping& location_map);

    /*!
     * @brief Gets number of DIMMs that platform supports
     * @return Number of DIMMs supported
     */
    constexpr static std::uint8_t get_number_of_supported_dimms() {
        return pmem::purley_refresh_topology::NUM_DIMMS_ON_PLATFORM;
    }

    /*!
     * @brief Gets number of bytes per DCPMEM block
     * @return block size in bytes
     */
    constexpr static std::uint8_t get_dcpmem_block_size() {
        return pmem::purley_refresh_topology::DCPMEM_BLOCK_SIZE_BYTES;
    }

    /*!
     * @brief Gets Memory Location Mapping (Device Locator -> Memory Location (socket, controller, channel, slot))
     * @return MemoryLocationMapping object
     */
    static MemoryLocationMapping get_memory_location_mapping() {
        return m_memory_location_map;
    }

    /*!
     * @brief Gets Memory Topology constants (number of CPUs, iMCs, Channels, Slots etc.)
     * @return MemoryTopologyParameters object
     */
    static MemoryTopologyParameters get_topology_parameters() {
        return m_memory_topology;
    }

private:
    static MemoryLocationMapping m_memory_location_map;
    static MemoryTopologyParameters m_memory_topology;
};

}
