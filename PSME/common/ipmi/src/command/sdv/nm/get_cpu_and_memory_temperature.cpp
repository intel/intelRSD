/*!
 * @brief GetCpuAndMemoryTemperature IPMI command request and response implementation.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file sdv/nm/get_cpu_and_memory_temperature.cpp
 */

#include "ipmi/command/sdv/nm/get_cpu_and_memory_temperature.hpp"
#include "ipmi/command/sdv/nm/errors.hpp"
#include "ipmi/command/sdv/enums.hpp"
#include "ipmi/utils/byte_buffer.hpp"

using namespace ipmi;
using namespace ipmi::command::sdv;

namespace {
constexpr const std::uint8_t OFFSET_READINGS = 4;
}

request::GetCpuAndMemoryTemperature::GetCpuAndMemoryTemperature()
    : Request(NetFn::INTEL_NM, Cmd::GET_CPU_AND_MEMORY_TEMPERATURE) {
    m_memory_channel_mask_for_cpu.fill(0);
}


request::GetCpuAndMemoryTemperature::~GetCpuAndMemoryTemperature() {}

void request::GetCpuAndMemoryTemperature::pack(IpmiInterface::ByteBuffer& data) const {
    data.insert(data.end(), INTEL_IANA_LSB_FIRST.begin(), INTEL_IANA_LSB_FIRST.end());
    data.push_back(m_cpus);
    for (auto i = 0u; i < NUM_CPUS; ++i) {
        if (is_extended_frame_format()) {
            ipmi::ByteBuffer::put_uint32(data, static_cast<uint>(data.size()), m_memory_channel_mask_for_cpu[i]);
        }
        else {
            ipmi::ByteBuffer::put_uint16(data, static_cast<uint>(data.size()),
                       static_cast<std::uint16_t>(m_memory_channel_mask_for_cpu[i]));
        }
    }
}

response::GetCpuAndMemoryTemperature::GetCpuAndMemoryTemperature()
    : Response(NetFn::INTEL_NM + 1, Cmd::GET_CPU_AND_MEMORY_TEMPERATURE, MIN_RESPONSE_SIZE) {}


response::GetCpuAndMemoryTemperature::~GetCpuAndMemoryTemperature() {}

void response::GetCpuAndMemoryTemperature::unpack(const IpmiInterface::ByteBuffer& data) {
    m_raw_readings = {data.begin() + OFFSET_READINGS, data.end()};
}

void response::GetCpuAndMemoryTemperature::throw_error_on_completion_code(CompletionCode completion_code) const {
    switch (completion_code) {
    case InvalidCpuNumber::ERROR_CODE:
        throw InvalidCpuNumber(*this);
    case InvalidPeciInterfaceState::ERROR_CODE:
        throw InvalidPeciInterfaceState(*this);
    case InvalidPlatformState::ERROR_CODE:
        throw InvalidPlatformState(*this);
    case InvalidResponseLength::ERROR_CODE:
        throw InvalidResponseLength(*this);
    case UnspecifiedError::ERROR_CODE:
        throw UnspecifiedError(*this);
    default:
        break;
    };
}
