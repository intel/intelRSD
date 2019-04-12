/*!
 * @brief Intel Optane DC Persistent Memory firmware interface accessor implementation
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
 * @file dcpmem_data_accessor.cpp
 */

#include "ipmi/utils/sdv/dcpmem_data_accessor.hpp"
#include "ipmi/command/sdv/nm/trigger_dcpmem_proxy_access.hpp"
#include "ipmi/command/sdv/nm/get_dcpmem_readings.hpp"
#include "ipmi/command/sdv/nm/errors.hpp"

#include <chrono>
#include <thread>

using namespace ipmi;
using namespace ipmi::sdv;
using namespace ipmi::command::sdv;

static constexpr const unsigned BASE_OFFSET = 0x0;    // Base offset for response read
static constexpr const unsigned COMMANDS_DELAY = 200; // Delay between IPMI commands send (in ms)

void DcpmemDataAccessor::trigger_access(std::uint8_t cpu_number, std::uint8_t smbus_id, std::uint8_t dimm_index,
                                        std::uint8_t opcode, std::uint8_t subopcode, std::uint8_t read_offset) const {

    request::TriggerDcpmemProxyAccess trigger_access_req{};
    response::TriggerDcpmemProxyAccess trigger_access_resp{};
    trigger_access_req.set_cpu_number(cpu_number);
    trigger_access_req.set_smbus_identifier(smbus_id);
    trigger_access_req.set_dimm_index(dimm_index);
    trigger_access_req.set_opcode(opcode);
    trigger_access_req.set_subopcode(subopcode);
    trigger_access_req.set_read_offset(read_offset);
    trigger_access_req.set_read_size(CHUNK_SIZE); // Read 64B chunks

    // Write function is not used - offset and size set to 0
    trigger_access_req.set_write_offset(0x0);
    trigger_access_req.set_write_size(0x0);

    m_ipmi.send(trigger_access_req, m_bridge, trigger_access_resp);
}

IpmiInterface::ByteBuffer DcpmemDataAccessor::get_readings() const {
    request::GetDcpmemReadings reading_req{};
    response::GetDcpmemReadings reading_resp{};

    m_ipmi.send(reading_req, m_bridge, reading_resp);
    return reading_resp.get_readings();
}

IpmiInterface::ByteBuffer DcpmemDataAccessor::get_dcpmem_data(DcpmemDimmLocation dimm, DcpmemCommand command) const {
    // TODO: Performance measurements.
    // Check if reading smaller chunks is faster than 64B (usually only few bytes are meaningful in a response)
    // - if yes, refactor function to fetch exact number of response bytes at a time.
    if (command.length % CHUNK_SIZE != 0) {
        throw std::runtime_error("DCPMEM Command IPMI Request is not aligned to 64B!");
    }

    // Initialize response header (DCPMEM_RESPONSE_HEADER) - type and length (aligned to 64B chunks)
    IpmiInterface::ByteBuffer output_data{std::uint8_t(command.command_type), command.length};
    auto chunk_idx = 0;
    auto chunks_number = int(command.length / CHUNK_SIZE);

    while (chunk_idx < chunks_number) {
        auto read_offset = BASE_OFFSET + chunk_idx * CHUNK_SIZE;
        chunk_idx++;

        trigger_access(dimm.cpu_number, dimm.smbus_id, dimm.dimm_index,
                       command.opcode, command.subopcode, std::uint8_t(read_offset));
        // Some delay required between commands execution - 'operation in progress' status is returned otherwise
        std::this_thread::sleep_for(std::chrono::milliseconds(COMMANDS_DELAY));
        auto temp_data = get_readings();

        output_data.insert(output_data.end(), temp_data.begin(), temp_data.end());
    }

    return output_data;
}