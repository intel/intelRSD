/*!
 * @brief GetTelemetryReadings IPMI command request and response implementation.
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
 * @file sdv/nm/get_telemetry_readings.cpp
 */

#include "ipmi/command/sdv/nm/get_telemetry_readings.hpp"
#include "ipmi/command/sdv/nm/errors.hpp"
#include "ipmi/command/sdv/enums.hpp"

using namespace ipmi;
using namespace ipmi::command::sdv;

request::GetTelemetryReadings::GetTelemetryReadings()
    : Request(NetFn::INTEL_NM, Cmd::GET_TELEMETRY_READINGS) {}


request::GetTelemetryReadings::~GetTelemetryReadings() {}

void request::GetTelemetryReadings::pack(IpmiInterface::ByteBuffer& data) const {
    data.insert(data.end(), INTEL_IANA_LSB_FIRST.begin(), INTEL_IANA_LSB_FIRST.end());
    // package GUID LSB first
    data.emplace_back(static_cast<std::uint8_t>(m_package_guid));
    data.emplace_back(static_cast<std::uint8_t>(m_package_guid >> 8));
    data.emplace_back(static_cast<std::uint8_t>(m_package_guid >> 16));
    data.emplace_back(static_cast<std::uint8_t>(m_package_guid >> 24));
    // metric offset
    data.push_back(m_start_reading_index);
}

response::GetTelemetryReadings::GetTelemetryReadings()
    : Response(NetFn::INTEL_NM + 1, Cmd::GET_TELEMETRY_READINGS, MIN_RESPONSE_SIZE) {}


response::GetTelemetryReadings::~GetTelemetryReadings() {}

namespace {
constexpr const std::uint8_t OFFSET_NEXT_READING_INDEX = 4;
constexpr const std::uint8_t OFFSET_READINGS = 5;
constexpr const int SINGLE_READING_SIZE = 4;
}

void response::GetTelemetryReadings::unpack(const IpmiInterface::ByteBuffer& data) {
    m_next_reading_index = data[OFFSET_NEXT_READING_INDEX];
    m_raw_readings.clear();
    const auto num_readings = (data.size() - OFFSET_READINGS) / SINGLE_READING_SIZE;

    for (auto i = 0u; i < num_readings; ++i) {
        m_raw_readings.emplace_back(std::uint32_t(data[OFFSET_READINGS + i * SINGLE_READING_SIZE]
                | (data[OFFSET_READINGS + i * SINGLE_READING_SIZE + 1] << 8)
                | (data[OFFSET_READINGS + i * SINGLE_READING_SIZE + 2] << 16)
                | (data[OFFSET_READINGS + i * SINGLE_READING_SIZE + 3] << 24)));
    }
}

void response::GetTelemetryReadings::throw_error_on_completion_code(CompletionCode completion_code) const {
    switch (completion_code) {
    case InvalidPackageGuid::ERROR_CODE:
        throw InvalidPackageGuid(*this);
    case InvalidReadingIndex::ERROR_CODE:
        throw InvalidReadingIndex(*this);
    default:
        break;
    };
}
