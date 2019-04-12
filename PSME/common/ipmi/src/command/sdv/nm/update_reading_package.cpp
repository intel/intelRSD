/*!
 * @brief UpdateReadingPackage IPMI command request and response implementation.
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
 * @file sdv/nm/update_reading_package.cpp
 */

#include "ipmi/command/sdv/nm/update_reading_package.hpp"
#include "ipmi/command/sdv/nm/errors.hpp"
#include "ipmi/command/sdv/enums.hpp"

using namespace ipmi;
using namespace ipmi::command::sdv;

namespace {
constexpr const std::uint8_t UPDATE_MODE_MASK = 0xFC;
constexpr const std::uint8_t DELETE_COMMAND = 0x01;
constexpr const std::uint8_t PACKAGE_VOLATILE_MASK = 0xFB;
constexpr const std::uint8_t PERSISTENT_COMMAND = 1 << 3;
}

request::UpdateReadingPackage::UpdateReadingPackage()
    : Request(NetFn::INTEL_NM, Cmd::UPDATE_READING_PACKAGE) {}


request::UpdateReadingPackage::~UpdateReadingPackage() {}

void request::UpdateReadingPackage::pack(IpmiInterface::ByteBuffer& data) const {
    data.insert(data.end(), INTEL_IANA_LSB_FIRST.begin(), INTEL_IANA_LSB_FIRST.end());
    // package GUID LSB first
    data.emplace_back(static_cast<std::uint8_t>(m_package_guid));
    data.emplace_back(static_cast<std::uint8_t>(m_package_guid >> 8));
    data.emplace_back(static_cast<std::uint8_t>(m_package_guid >> 16));
    data.emplace_back(static_cast<std::uint8_t>(m_package_guid >> 24));
    data.emplace_back(m_reading_package_command);
    data.emplace_back(m_reporting_interval.count());
    // readings shall not be provided when deleting reading package
    if (!(m_reading_package_command & DELETE_COMMAND)) {
        for (const auto& reading: m_readings) {
            data.emplace_back(static_cast<uint8_t>(reading.m_id));
            data.emplace_back(static_cast<uint8_t>(reading.m_id >> 8));
            data.emplace_back(static_cast<uint8_t>(reading.m_id >> 16));
            data.emplace_back(static_cast<uint8_t>(reading.m_id >> 24));
            data.emplace_back(static_cast<uint8_t>(reading.m_measure_type));
        }
    }
}

void request::UpdateReadingPackage::set_package_guid(std::uint32_t package_guid) {
    m_package_guid = package_guid;
}

void request::UpdateReadingPackage::enable_append_mode() {
    m_reading_package_command = m_reading_package_command & UPDATE_MODE_MASK;
}

void request::UpdateReadingPackage::delete_reading_package() {
    m_reading_package_command = m_reading_package_command | DELETE_COMMAND;
}

void request::UpdateReadingPackage::set_package_volatile() {
    m_reading_package_command = m_reading_package_command & PACKAGE_VOLATILE_MASK;
}

void request::UpdateReadingPackage::set_package_persistent() {
    m_reading_package_command = m_reading_package_command | PERSISTENT_COMMAND;
}

void request::UpdateReadingPackage::set_reporting_interval(const deciseconds interval) {
    m_reporting_interval = interval;
}

void request::UpdateReadingPackage::set_metric_ids(const MetricIdVec& readings) {
    m_readings = readings;
}

response::UpdateReadingPackage::UpdateReadingPackage()
    : Response(NetFn::INTEL_NM + 1, Cmd::UPDATE_READING_PACKAGE, RESPONSE_SIZE) {}


response::UpdateReadingPackage::~UpdateReadingPackage() {}

namespace {
constexpr const std::uint8_t OFFSET_PACKAGE_GUID = 4;
}

void response::UpdateReadingPackage::unpack(const IpmiInterface::ByteBuffer& data) {
    m_reading_package_guid = std::uint32_t(data[OFFSET_PACKAGE_GUID]
                                            | (data[OFFSET_PACKAGE_GUID + 1] << 8)
                                            | (data[OFFSET_PACKAGE_GUID + 2] << 16)
                                            | (data[OFFSET_PACKAGE_GUID + 3] << 24));
}

void response::UpdateReadingPackage::throw_error_on_completion_code(CompletionCode completion_code) const {
    switch (completion_code) {
    case InvalidPackageGuid::ERROR_CODE:
        throw InvalidPackageGuid(*this);
    case InvalidMetricGuid::ERROR_CODE:
        throw InvalidMetricGuid(*this);
    case InvalidReportingInterval::ERROR_CODE:
        throw InvalidReportingInterval(*this);
    case InvalidGroupingOperation::ERROR_CODE:
        throw InvalidGroupingOperation(*this);
    case InvalidReadingPackageCommand::ERROR_CODE:
        throw InvalidReadingPackageCommand(*this);
    case InvalidPersistencyParameter::ERROR_CODE:
        throw InvalidPersistencyParameter(*this);
    default:
        break;
    };
}
