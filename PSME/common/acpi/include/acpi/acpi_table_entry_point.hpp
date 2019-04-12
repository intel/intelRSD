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
 * @file acpi_table_entry_point.hpp
 */

#pragma once

#include "acpi_entry_point.hpp"
#include "structs/acpi_header.hpp"
#include <vector>

namespace acpi {
namespace parser {

class AcpiTableEntryPoint: public AcpiEntryPoint {

public:
    /*!
     * @brief Create Entry point structure pointer
     * @param buf pointer to the bytes to be parsed
     * @param buf_size number of bytes to be parsed
     */
    AcpiTableEntryPoint(const std::uint8_t* buf, const size_t buf_size);

    /*! Copy constructor */
    AcpiTableEntryPoint(const AcpiTableEntryPoint&) = default;

    /*! Assignment operator */
    AcpiTableEntryPoint& operator=(const AcpiTableEntryPoint&) = default;

    /*! Destructor */
    ~AcpiTableEntryPoint() {}

    /*!
     * @brief Get string which indicates the signature of the table
     * @return string which indicates the signature of the table
     */
    std::string get_signature() const;

    /*!
     * @brief Get table length from entry point structure
     * @return table length in bytes
     */
    std::uint32_t get_length() const override;

    /*!
     * @brief Get revision number of the entry point structure
     * @return entry point structure revision
     */
    std::uint8_t get_revision() const override;

    /*!
     * @brief Return data structure start offset for table with given signature
     * @param table_signature signature of a table for which data offset is returned
     * @return start offset of data struct within the MDR region if table exists, 0 otherwise
     */
    std::uint64_t get_table_data_offset(const std::string& table_signature) const override;

    /*!
     * @brief Return data structure end offset for table with given signature
     * @param table_signature signature of a table for which data offset is returned
     * @return end offset of data struct within the MDR region if table exists, 0 otherwise
     */
    std::uint64_t get_table_data_end_offset(const std::string& table_signature) const override;

    /*!
     * @brief Get data struct offset within table
     * @return start offset of data struct within the MDR region
     */
    std::uint64_t get_struct_table_address() const override;

    /*!
     * @brief Set data struct offset within table
     * @param table_data_offset start offset of data struct within the MDR region
     */
    void set_struct_table_address(std::uint64_t table_data_offset) override;

    /*!
    * @brief Get data struct end offset within table
    * @return end offset of data struct within the MDR region
    */
    std::uint64_t get_struct_table_end_address() const override;

    /*!
     * @brief Set data struct end offset within table
     * @param table_data_end_offset end offset of data struct within the MDR region
     */
    void set_struct_table_end_address(std::uint64_t table_data_end_offset) override;

    /*!
     * @brief Not implemented, this method always throws.
     * @return None.
     */
    [[ noreturn ]]
    std::uint8_t get_major_version() const override;

    /*!
     * @brief Not implemented, this method always throws.
     * @return None.
     */
    [[ noreturn ]]
    std::uint8_t get_minor_version() const override;

    /*!
     * @brief Not implemented, this method always throws.
     * @return None.
     */
    [[ noreturn ]]
    std::string get_anchor_string() const override;

    struct AcpiTableType {
        std::string   signature;
        std::uint64_t data_start_offset; // Data structures start offset = Base Address of ACPI Table + Size of ACPI Header (40B)
        std::uint64_t data_end_offset;   // Data structures end offset   = Base Address of ACPI Table + Size of Table
    };

    using EntryPointStructure = acpi::structs::ACPI_TABLE_HEADER;

private:
    const EntryPointStructure* m_entry_point;
    std::vector<AcpiTableType> m_acpi_table_type_list{};
    std::uint64_t m_struct_table_address{0};
    std::uint64_t m_struct_table_end_address{0};
};

}
}