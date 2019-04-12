/*!
 * @brief Implementation of the iSCSI MDR entry point v1.0.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file iscsi_mdr_10_entry_point.cpp
 */

#include "iscsi/iscsi_mdr_10_entry_point.hpp"
#include "iscsi/structs/iscsi_mdr_version.hpp"

#include <type_traits>

namespace iscsi {
namespace parser {

IscsiMdr10EntryPoint::IscsiMdr10EntryPoint(const std::uint8_t* buf, const size_t buf_size) :
    m_version(reinterpret_cast<decltype(m_version)>(buf)), m_region_length(uint32_t(buf_size)) {
    if (buf_size < sizeof(*m_version)) {
        throw Exception("ISCSI MDR blob size is too small");
    }

    if (get_major_version() != SUPPORTED_MAJOR && get_minor_version() != SUPPORTED_MINOR) {
        throw Exception("iSCSI MDR version mismatch");
    }
}

std::uint8_t IscsiMdr10EntryPoint::get_major_version() const {
    return m_version->data.major_version;
}

std::uint8_t IscsiMdr10EntryPoint::get_minor_version() const {
    return m_version->data.minor_version;
}

std::uint32_t IscsiMdr10EntryPoint::get_length() const {
    return m_region_length;
}

std::uint64_t IscsiMdr10EntryPoint::get_struct_table_address() const {
    return 0;
}

std::uint64_t IscsiMdr10EntryPoint::get_struct_table_end_address() const {
    return IscsiMdr10EntryPoint::get_length();
}

std::string IscsiMdr10EntryPoint::get_anchor_string() const {
    throw Exception("method not implemented");
}

std::uint8_t IscsiMdr10EntryPoint::get_revision() const {
    throw Exception("method not implemented");
}

}  // namespace parser
}  // namespace iscsi
