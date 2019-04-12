/*!
 * @brief Intel Optane DC Persistent Memory command entry point
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
 * @file dcpmem_command_entry_point.cpp
 */

#include "pmem/dcpmem_command_entry_point.hpp"

using namespace pmem::parser;

DcpmemCommandEntryPoint::DcpmemCommandEntryPoint(const std::uint8_t* buf, const size_t buf_size)
    : m_entry_point(reinterpret_cast<const EntryPointStructure*> (buf)) {
    if (nullptr == m_entry_point) {
        throw Exception("Invalid DCPMEM command entry point");
    }
    if (buf_size < sizeof (EntryPointStructure)) {
        throw Exception("DCPMEM command entry point too small");
    }
}

std::uint32_t DcpmemCommandEntryPoint::get_length() const {
    return m_entry_point->length;
}

std::uint64_t DcpmemCommandEntryPoint::get_struct_table_address() const {
    return 0;
}

std::uint64_t DcpmemCommandEntryPoint::get_struct_table_end_address() const {
    return DcpmemCommandEntryPoint::get_length();
}

std::uint8_t DcpmemCommandEntryPoint::get_revision() const {
    throw Exception("Method not implemented: get_revision()");
}

std::uint8_t DcpmemCommandEntryPoint::get_major_version() const {
    throw Exception("Method not implemented: get_major_version()");
}

std::uint8_t DcpmemCommandEntryPoint::get_minor_version() const {
    throw Exception("Method not implemented: get_minor_version()");
}

std::string DcpmemCommandEntryPoint::get_anchor_string() const {
    throw Exception("Method not implemented: get_anchor_string()");
}