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
 * @file memory_topology.cpp
 */

#include "pmem/memory_topology.hpp"
#include "pmem/platform_specific/purley_refresh_memory_topology.hpp"
#include "pmem/platform_specific/purley_refresh_memory_mapping.hpp"

using namespace pmem;
using namespace agent_framework::model;

// Initialize Memory Location Map and Topology parameters with platform specific data (Purley-R)
using MemoryLocationMapping = std::unordered_map<std::string, agent_framework::model::attribute::MemoryLocation>;
MemoryLocationMapping MemoryTopology::m_memory_location_map {purley_refresh_topology::memory_location_map};

MemoryTopology::MemoryTopologyParameters MemoryTopology::m_memory_topology {purley_refresh_topology::NUM_CPUS,
                                                                            purley_refresh_topology::NUM_IMCS_PER_CPU,
                                                                            purley_refresh_topology::NUM_CHANNELS_PER_IMC,
                                                                            purley_refresh_topology::NUM_SLOTS_PER_CHANNEL,
                                                                            purley_refresh_topology::NUM_ACTIVE_SLOTS_PER_CHANNEL};

OptionalField<attribute::MemoryLocation> MemoryTopology::get_memory_location_for_locator(const OptionalField<std::string>& device_locator) {
    if (device_locator.has_value()) {
        const auto& memory_location_map = get_memory_location_mapping();
        auto it = memory_location_map.find(device_locator);
        if (it != memory_location_map.cend()) {
            return it->second;
        }
    }
    return OptionalField<attribute::MemoryLocation>{};
}

int MemoryTopology::get_memory_instance_for_dcpmem_location(const ipmi::command::sdv::DcpmemDimmLocation& location) {
    std::uint8_t cpu_id = location.cpu_number;
    std::uint8_t smbus_id = location.smbus_id;
    std::uint8_t dimm_idx = location.dimm_index;

    const auto num_dimms_per_imc = m_memory_topology.get_num_dimms_per_imc();
    const auto num_dimms_per_cpu = m_memory_topology.get_num_dimms_per_cpu();

    return ((cpu_id * num_dimms_per_cpu + smbus_id * num_dimms_per_imc + dimm_idx) >>
            (m_memory_topology.num_active_slots_per_channel % m_memory_topology.num_slots_per_channel));
}

ipmi::command::sdv::DcpmemDimmLocation MemoryTopology::get_dcpmem_location_for_memory_instance(const int memory_instance) {
    const auto num_active_slots_per_cpu = m_memory_topology.get_num_active_slots_per_cpu();
    const auto num_dimms_on_platform = m_memory_topology.get_num_dimms_on_platform();

    if (memory_instance >= 0 && memory_instance < num_dimms_on_platform) {
        std::uint8_t cpu_id = std::uint8_t(memory_instance / num_active_slots_per_cpu);
        std::uint8_t smbus_id = std::uint8_t((memory_instance % num_active_slots_per_cpu) / m_memory_topology.num_channels_per_imc);
        std::uint8_t dimm_idx = std::uint8_t((memory_instance % m_memory_topology.num_channels_per_imc) * m_memory_topology.num_slots_per_channel);

        return ipmi::command::sdv::DcpmemDimmLocation{cpu_id, smbus_id, dimm_idx};
    }
    return ipmi::command::sdv::DcpmemDimmLocation{0, 0, 0};
}

std::vector<ipmi::command::sdv::DcpmemDimmLocation> MemoryTopology::get_dcpmem_locations_for_memory_mapping(const MemoryLocationMapping& location_map) {
    std::vector<ipmi::command::sdv::DcpmemDimmLocation> dcpmem_locations{};

    std::transform(location_map.begin(), location_map.end(), std::back_inserter(dcpmem_locations),
                   [](const MemoryLocationMapping::value_type& pair) {
                       agent_framework::model::attribute::MemoryLocation location =  pair.second;
                       auto cpu_id = std::uint8_t(location.get_socket());
                       auto smbus_id = std::uint8_t(location.get_controller());
                       auto dimm_idx = std::uint8_t(m_memory_topology.num_slots_per_channel * location.get_channel() + location.get_slot());

                       return ipmi::command::sdv::DcpmemDimmLocation{cpu_id, smbus_id, dimm_idx};
                   });

    return dcpmem_locations;
}