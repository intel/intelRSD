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
 * @file dcpmem_command_entry_point.hpp
 */

#pragma once

#include "dcpmem_entry_point.hpp"
#include "structs/dcpmem_structs.hpp"

namespace pmem {
namespace parser {

class DcpmemCommandEntryPoint: public DcpmemEntryPoint {

public:
    /*!
     * @brief Create Entry point structure pointer
     * @param buf pointer to the bytes to be parsed
     * @param buf_size number of bytes to be parsed
     */
    DcpmemCommandEntryPoint(const std::uint8_t* buf, const size_t buf_size);

    /*! Copy constructor */
    DcpmemCommandEntryPoint(const DcpmemCommandEntryPoint&) = default;

    /*! Assignment operator */
    DcpmemCommandEntryPoint& operator=(const DcpmemCommandEntryPoint&) = default;

    /*! Destructor */
    ~DcpmemCommandEntryPoint() {}

    /*!
     * @brief Get table length from entry point structure
     * @return table length in bytes
     */
    std::uint32_t get_length() const override;

    /*!
     * @brief Get data struct offset within table
     * @return start offset of data struct
     */
    std::uint64_t get_struct_table_address() const override;

    /*!
    * @brief Get data struct end offset within table
    * @return end offset of data struct
    */
    std::uint64_t get_struct_table_end_address() const override;

    /*!
     * @brief Not implemented, this method always throws.
     * @return None.
     */
    [[ noreturn ]]
    std::uint8_t get_revision() const override;

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

    using EntryPointStructure = pmem::structs::DCPMEM_RESPONSE_HEADER;

private:
    const EntryPointStructure* m_entry_point;

};

}
}