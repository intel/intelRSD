/*!
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
 * @brief
 *
 * @file smbios_21_entry_point.cpp
 */

#include "smbios/smbios_21_entry_point.hpp"
#include <numeric>

using namespace smbios::parser;

namespace {

constexpr const char* _SM_ = "_SM_";
constexpr const char* _DMI_ = "_DMI_";

}

Smbios21EntryPoint::Smbios21EntryPoint(const std::uint8_t* buf, const size_t buf_size)
    : m_entry_point(reinterpret_cast<const EntryPointStructure*>(buf)), m_region_length(uint32_t(buf_size)) {
    if (nullptr == m_entry_point) {
        throw Exception("Invalid SMBIOS entry point");
    }
    if (buf_size < sizeof(EntryPointStructure)) {
        throw Exception("SMBIOS entry point too small");
    }
    // verify checksum
    uint8_t sum = std::accumulate(buf, buf + sizeof(EntryPointStructure), uint8_t(0));
    if (0 != sum) {
        throw Exception("Incorrect checksum");
    }
    // verify anchor strings
    if (get_anchor_string() != _SM_) {
        throw Exception("Invalid anchor string");
    }
    // verify intermediate anchor strings
    if (get_intermediate_anchor_string() != _DMI_) {
        throw Exception("Invalid intermediate anchor string");
    }
    // verify intermediate checksum
    sum = std::accumulate(buf + 0x10 /* start of intermediate region */,
                          buf + 0x10 + 0x0F /* end of intermediate region */, uint8_t(0));
    if (0 != sum) {
        throw Exception("Incorrect intermediate checksum");
    }
}

std::string Smbios21EntryPoint::get_anchor_string() const {
    return {reinterpret_cast<char const*>(m_entry_point->anchor), sizeof(m_entry_point->anchor)};
}

std::uint8_t Smbios21EntryPoint::get_major_version() const {
    return m_entry_point->major;
}

std::uint8_t Smbios21EntryPoint::get_minor_version() const {
    return m_entry_point->minor;
}

std::uint8_t Smbios21EntryPoint::get_revision() const {
    return m_entry_point->revision;
}

std::uint64_t Smbios21EntryPoint::get_struct_table_address() const {
    return m_entry_point->struct_table_address;
}

std::uint64_t Smbios21EntryPoint::get_struct_table_end_address() const {
    return Smbios21EntryPoint::get_length();
}

std::uint32_t Smbios21EntryPoint::get_length() const {
    return m_region_length;
}
