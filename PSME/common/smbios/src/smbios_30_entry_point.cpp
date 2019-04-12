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
 * @file smbios_30_entry_point.cpp
 */

#include "smbios/smbios_30_entry_point.hpp"
#include <numeric>

using namespace smbios::parser;

namespace {

constexpr const char* _SM3_ = "_SM3_";

}

Smbios30EntryPoint::Smbios30EntryPoint(const std::uint8_t* buf, const size_t buf_size)
    : m_entry_point(reinterpret_cast<const EntryPointStructure*> (buf)), m_region_length(uint32_t(buf_size)) {
    if (nullptr == m_entry_point) {
        throw Exception("Invalid SMBIOS entry point");
    }
    if (buf_size < sizeof (EntryPointStructure)) {
        throw Exception("SMBIOS entry point too small");
    }
    // verify checksum
    uint8_t sum = std::accumulate(buf, buf + sizeof(EntryPointStructure), uint8_t(0));
    if (0 != sum) {
        throw Exception("Incorrect checksum");
    }
    // verify anchor strings
    if (get_anchor_string() != _SM3_) {
        throw Exception("Invalid anchor string");
    }
}

std::string Smbios30EntryPoint::get_anchor_string() const  {
    return {reinterpret_cast<char const*>(m_entry_point->anchor), sizeof(m_entry_point->anchor)};
}

std::uint8_t Smbios30EntryPoint::get_major_version() const {
    return m_entry_point->major;
}

std::uint8_t Smbios30EntryPoint::get_minor_version() const {
    return m_entry_point->minor;
}

std::uint8_t Smbios30EntryPoint::get_revision() const {
    return m_entry_point->revision;
}

std::uint64_t Smbios30EntryPoint::get_struct_table_address() const {
    return m_entry_point->struct_table_address;
}

std::uint64_t Smbios30EntryPoint::get_struct_table_end_address() const {
    return Smbios30EntryPoint::get_length();
}

std::uint32_t Smbios30EntryPoint::get_length() const {
    return m_region_length;
}