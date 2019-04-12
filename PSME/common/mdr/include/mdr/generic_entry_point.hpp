/*!
 * @brief Generic SMBIOS style entry point interface.
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
 * @file generic_entry_point.hpp
 */

#pragma once

#include <cstdint>
#include <memory>

namespace mdr {

/*! GenericEntryPoint interface */
class GenericEntryPoint {
public:
    using Ptr = std::unique_ptr<GenericEntryPoint>;

    /*! Destructor */
    virtual ~GenericEntryPoint();

    /*!
     * @brief Get major version
     * @return numerical value of the major version
     */
    virtual std::uint8_t get_major_version() const = 0;

    /*!
     * @brief Get minor version
     * @return numerical value of the minor version
     */
    virtual std::uint8_t get_minor_version() const = 0;

    /*!
     * @brief Get anchor string which indicates the start of the entry point structure
     * @return anchor string which indicates the start of the entry point structure
     */
    virtual std::string get_anchor_string() const = 0;

    /*!
     * @brief Get revision number of the entry point structure
     * @return entry point structure revision
     */
    virtual std::uint8_t get_revision() const = 0;

    /*!
     * @brief Get structure length in MDR
     * @return structure length in bytes
     */
    virtual std::uint32_t get_length() const = 0;

    /*!
     * @brief Return data structure offset for table with given signature
     * @param signature string signature of a table for which data offset is returned
     * @return start offset of data struct within the MDR region if table exists, 0 otherwise
     */
    virtual std::uint64_t get_table_data_offset(const std::string& signature) const = 0;

    /*!
     * @brief Return data structure end offset for table with given signature
     * @param signature string signature of a table for which data offset is returned
     * @return end offset of data struct within the MDR region if table exists, 0 otherwise
     */
    virtual std::uint64_t get_table_data_end_offset(const std::string& signature) const = 0;

    /*!
     * @brief Get data struct offset within table
     * @return start offset of data struct within the MDR region
     */
    virtual std::uint64_t get_struct_table_address() const = 0;

    /*!
     * @brief Set data struct offset within table
     * @param offset uint64_t start offset of data struct within the MDR region
     */
    virtual void set_struct_table_address(std::uint64_t offset) = 0;

    /*!
     * @brief Get data struct end offset within table
     * @return end offset of data struct within the MDR region
     */
    virtual std::uint64_t get_struct_table_end_address() const = 0;

    /*!
     * @brief Set data struct end offset within table
     * @param offset uint64_t end offset of data struct within the MDR region
     */
    virtual void set_struct_table_end_address(std::uint64_t offset) = 0;
};

}  // namespace mdr
