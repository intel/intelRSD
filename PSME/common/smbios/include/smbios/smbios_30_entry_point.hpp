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
 * @brief SMBIOS 3.0 entry point structure
 *
 * @file smbios_30_entry_point.hpp
 */

#pragma once

#include "smbios/smbios_entry_point.hpp"

namespace smbios {
namespace parser {

class Smbios30EntryPoint: public SmbiosEntryPoint {

public:
    /*!
     * @brief Create Entry point structure pointer
     * @param buf pointer to the bytes to be parsed
     * @param buf_size number of bytes to be parsed
     */
    Smbios30EntryPoint(const std::uint8_t* buf, const size_t buf_size);

    /*! Copy constructor */
    Smbios30EntryPoint(const Smbios30EntryPoint&) = default;

    /*! Assignment operator */
    Smbios30EntryPoint& operator=(const Smbios30EntryPoint&) = default;

    /*! Destructor */
    ~Smbios30EntryPoint() {}

    /*!
     * @brief Get anchor string which indicates the start of the entry point structure
     * @return anchor string which indicates the start of the entry point structure
     */
    std::string get_anchor_string() const override;

    /*!
     * @brief Get SMBIOS major version
     * @return numerical value of the SMBIOS major version
     */
    std::uint8_t get_major_version() const override;

    /*!
     * @brief Get SMBIOS minor version
     * @return numerical value of the SMBIOS minor version
     */
    std::uint8_t get_minor_version() const override;

    /*!
     * @brief Get revision number of the entry point structure
     * @return entry point structure revision
     */
    std::uint8_t get_revision() const override;

    /*!
     * @brief Get data struct offset within table
     * @return start offset of data struct within the MDR region
     */
    std::uint64_t get_struct_table_address() const override;

    /*!
     * @brief Get data struct end offset within table
     * @return end offset of data struct within the MDR region
     */
    std::uint64_t get_struct_table_end_address() const override;

    /*!
     * @brief Get structure length in MDR
     * @return structure length in bytes
     */
    std::uint32_t get_length() const override;

#pragma pack(push, 1)

/*!
 * @brief Smbios 3.0 (64-bit) Entry point structure
 * */
    struct EntryPointStructure {
        uint8_t anchor[5];
        uint8_t checksum;
        uint8_t length;
        uint8_t major;
        uint8_t minor;
        uint8_t docrev;
        uint8_t revision;
        uint8_t reserved;
        uint32_t struct_table_max_size;
        uint64_t struct_table_address;
    };

#pragma pack(pop)
private:
    const EntryPointStructure* m_entry_point;
    std::uint32_t m_region_length{0};
};

}
}
