/*!
 * @brief ACPI Table entry point
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
 * @file acpi_table_entry_point.cpp
 */

#include "acpi/acpi_table_entry_point.hpp"
#include <numeric>

using namespace acpi::parser;

AcpiTableEntryPoint::AcpiTableEntryPoint(const std::uint8_t* buf, const size_t buf_size)
    : m_entry_point(reinterpret_cast<const EntryPointStructure*> (buf)) {
    if (nullptr == m_entry_point) {
        throw Exception("Invalid ACPI entry point");
    }
    if (buf_size < sizeof (EntryPointStructure)) {
        throw Exception("ACPI entry point too small");
    }

    const std::uint8_t* table_start = buf;
    const std::uint8_t* const buffer_end = buf + buf_size;
    std::uint32_t data_offset_in_mdr = sizeof(acpi::structs::ACPI_TABLE_HEADER); // Data offset = after ACPI header
    std::uint32_t data_end_offset_in_mdr = 0;

    while (table_start < buffer_end) {
        m_entry_point = reinterpret_cast<const EntryPointStructure*> (table_start);
        std::uint32_t current_table_length = get_length();

        if (buf_size < current_table_length) {
            throw Exception("Invalid length of ACPI table");
        }

        // Verify checksum
        uint8_t sum = std::accumulate(table_start, table_start + current_table_length, uint8_t(0));
        if (0 != sum) {
            throw Exception("Incorrect ACPI table checksum");
        }

        // Verify signature
        std::string acpi_signature = get_signature();
        if (acpi_signature != acpi::structs::NFIT && acpi_signature != acpi::structs::PCAT &&
            acpi_signature != acpi::structs::SRAT && acpi_signature != acpi::structs::HMAT &&
            acpi_signature != acpi::structs::PMTT) {

            throw Exception("Invalid ACPI table signature (supported: NFIT, PCAT, SRAT, HMAT, PMTT)");
        }

        data_end_offset_in_mdr += current_table_length;

        // Store current Table identification (signature and data start and end offset within MDR blob for parser use)
        m_acpi_table_type_list.push_back(AcpiTableType{acpi_signature, data_offset_in_mdr, data_end_offset_in_mdr});

        data_offset_in_mdr += current_table_length; // Set data offset for the next table (if present)
        table_start += current_table_length; // Move pointer to the next table header
    }
}

std::string AcpiTableEntryPoint::get_signature() const {
    return {reinterpret_cast<char const*>(m_entry_point->Signature), sizeof(m_entry_point->Signature)};
}

std::uint32_t AcpiTableEntryPoint::get_length() const {
    return m_entry_point->Length;
}

std::uint8_t AcpiTableEntryPoint::get_revision() const {
    return m_entry_point->Revision;
}

std::uint64_t AcpiTableEntryPoint::get_table_data_offset(const std::string& table_signature) const {
    for (const auto& acpi_table_type : m_acpi_table_type_list) {
        if (acpi_table_type.signature == table_signature) {
            return acpi_table_type.data_start_offset;
        }
    }
    return 0;
}

std::uint64_t AcpiTableEntryPoint::get_table_data_end_offset(const std::string& table_signature) const {
    for (const auto& acpi_table_type : m_acpi_table_type_list) {
        if (acpi_table_type.signature == table_signature) {
            return acpi_table_type.data_end_offset;
        }
    }
    return 0;
}

std::uint64_t AcpiTableEntryPoint::get_struct_table_address() const {
    return m_struct_table_address;
}

void AcpiTableEntryPoint::set_struct_table_address(std::uint64_t table_data_offset) {
    m_struct_table_address = table_data_offset;
}

std::uint64_t AcpiTableEntryPoint::get_struct_table_end_address() const {
    return m_struct_table_end_address;
}

void AcpiTableEntryPoint::set_struct_table_end_address(std::uint64_t table_data_end_offset) {
    m_struct_table_end_address = table_data_end_offset;
}

std::uint8_t AcpiTableEntryPoint::get_major_version() const {
    throw Exception("Method not implemented: get_major_version()");
}

std::uint8_t AcpiTableEntryPoint::get_minor_version() const {
    throw Exception("Method not implemented: get_minor_version()");
}

std::string AcpiTableEntryPoint::get_anchor_string() const {
    throw Exception("Method not implemented: get_anchor_string()");
}
