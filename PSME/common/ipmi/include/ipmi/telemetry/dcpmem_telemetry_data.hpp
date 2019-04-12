/*!
 * @brief DCPMEM Telemetry data - Telemetry Reader context.
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
 * @file ipmi/telemetry/dcpmem_telemetry_data.hpp
 */

#pragma once

#include "ipmi/utils/sdv/dcpmem_data_accessor.hpp"
#include "ipmi/command/sdv/nm/errors.hpp"
#include "pmem/dcpmem_parser.hpp"
#include "pmem/memory_topology.hpp"
#include "logger/logger_factory.hpp"

namespace ipmi {
namespace sdv {

class DcpmemTelemetryData {
public:
    void set_dcpmem_dimm_locations(std::vector<ipmi::command::sdv::DcpmemDimmLocation>& dcpmem_locations) {
        m_dcpmem_dimm_locations = dcpmem_locations;
    }

    pmem::structs::SMART_INFO get_smart_info(int memory_instance_number) {
        return m_dcpmem_smart_info_array[memory_instance_number];
    }

    pmem::structs::MEMORY_INFO get_memory_info(int memory_instance_number) {
        return m_dcpmem_memory_info_array[memory_instance_number];
    }

    void update_readings(ipmi::IpmiController& ipmi, const ipmi::BridgeInfo& bridge) {
        ipmi::sdv::DcpmemDataAccessor dcpmem_accessor{ipmi, bridge};
        IpmiInterface::ByteBuffer dcpmem_smart_info{};
        IpmiInterface::ByteBuffer dcpmem_memory_info{};

        for (const auto& location : m_dcpmem_dimm_locations) {
            std::uint8_t cpu_id = location.cpu_number;
            std::uint8_t smbus_id = location.smbus_id;
            std::uint8_t dimm_id = location.dimm_index;

            ipmi::command::sdv::DcpmemDimmLocation dcpmem_location{cpu_id, smbus_id, dimm_id};

            try {
                dcpmem_smart_info = dcpmem_accessor.get_dcpmem_data(dcpmem_location, ipmi::command::sdv::DCPMEM_GET_SMART_INFO);
                dcpmem_memory_info = dcpmem_accessor.get_dcpmem_data(dcpmem_location, ipmi::command::sdv::DCPMEM_GET_MEMORY_INFO);
            }
            catch (const ipmi::command::sdv::OperationInProgressError& e) {
                log_warning("dcpmem-telemetry", e.what());
                continue;
            }
            catch (const ipmi::command::sdv::IllegalSmbusAddressError&) {
                log_debug("dcpmem-telemetry", "SMBus Address Error: not a DCPMEM -"
                          << " [Cpu: " << std::to_string(cpu_id)
                          << " Smbus: " << std::to_string(smbus_id)
                          << " Dimm: " << std::to_string(dimm_id) << "]");
                continue;
            }

            // Parse telemetry retrieved from the DCPMEM FW and insert reading items to relevant arrays (indexed by ResourceInstance number)
            pmem::parser::DcpmemParser dcpmem_smart_info_parser(dcpmem_smart_info.data(), dcpmem_smart_info.size(), false);
            auto smart_info = (dcpmem_smart_info_parser.get_all<pmem::structs::DCPMEM_GET_SMART_INFO_DATA>()).front().data;

            pmem::parser::DcpmemParser dcpmem_memory_info_parser(dcpmem_memory_info.data(), dcpmem_memory_info.size(), false);
            auto memory_info = (dcpmem_memory_info_parser.get_all<pmem::structs::DCPMEM_GET_MEMORY_INFO_DATA>()).front().data;

            auto memory_instance_number = pmem::MemoryTopology::get_memory_instance_for_dcpmem_location(dcpmem_location);
            m_dcpmem_smart_info_array[memory_instance_number] = smart_info;
            m_dcpmem_memory_info_array[memory_instance_number] = memory_info;

            log_debug("dcpmem-telemetry", "Read telemetry: DCPMEM"
                      << " ResourceInstance[" << memory_instance_number << "] -"
                      << " [Cpu: " << std::to_string(cpu_id)
                      << " Smbus: " << std::to_string(smbus_id)
                      << " Dimm: " << std::to_string(dimm_id) << "]");
        }
    }

private:
    std::array<pmem::structs::SMART_INFO, pmem::MemoryTopology::get_number_of_supported_dimms()> m_dcpmem_smart_info_array{};
    std::array<pmem::structs::MEMORY_INFO, pmem::MemoryTopology::get_number_of_supported_dimms()> m_dcpmem_memory_info_array{};
    std::vector<ipmi::command::sdv::DcpmemDimmLocation> m_dcpmem_dimm_locations{};
};

}
}