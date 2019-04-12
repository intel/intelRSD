/*!
 * @brief Intel Optane DC Persistent Memory FW Commands parser
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
 * @file dcpmem_parser.hpp
 */

#pragma once

#include "dcpmem_entry_point.hpp"
#include "structs/dcpmem_structs.hpp"
#include "mdr/generic_parser.hpp"

#include <stdexcept>
#include <type_traits>

namespace pmem {
namespace parser {

/*!
 * Traits class for the DCPMEM FW commands parser.
 */
class DcpmemTraits final {
public:
    /*!
     * @brief Exception type that will be thrown when an error condition is
     * discovered while parsing the DCPMEM command blob
     */
    class Exception : public std::runtime_error {
    public:
        /*!
         * @brief constructs exception object
         * @param what_arg Describes what happened
         */
        explicit Exception(const std::string& what_arg) :
            std::runtime_error("DCPMEM parser - " + what_arg) {
        }


        Exception(const Exception&) = default;


        Exception& operator=(const Exception&) = default;


        Exception(Exception&&) = default;


        Exception& operator=(Exception&&) = default;


        virtual ~Exception();
    };

    /*!
     * @brief Check for header type equality.
     * @tparam T The type of structure for equality check.
     * @tparam H The type of the header for equality check.
     * @param[in] header DCPMEM structure header.
     * @return true if equal, false otherwise.
     */
    template<typename T, typename H>
    static bool header_type_equal(const H& header, const uint16_t*) {
        return (T::ID == header.type) && ((header.length + sizeof(header)) == sizeof(T));
    }

    /*!
     * @brief Get the length of the ACPI table.
     * @param[in] header ACPI structure header.
     * @return length of the ACPI table.
     */
    template<typename H>
    static auto table_length(const H& header) -> decltype(header.length) {
        return header.length;
    }

    using EntryPoint = DcpmemEntryPoint;
};

/*!
 * @brief DCPMEM parser instantiation.
 */
using DcpmemParser = mdr::GenericParser<DcpmemTraits>;

} // namespace parser
} // namespace pmem

namespace mdr {

std::ostream& operator<<(std::ostream& os, const StructEnhanced<pmem::structs::DCPMEM_IDENTIFY_DIMM_DATA>& s);

std::ostream& operator<<(std::ostream& os, const StructEnhanced<pmem::structs::DCPMEM_GET_SECURITY_STATE_DATA>& s);

std::ostream& operator<<(std::ostream& os, const StructEnhanced<pmem::structs::DCPMEM_GET_POWER_MANAGEMENT_POLICY_DATA>& s);

std::ostream& operator<<(std::ostream& os, const StructEnhanced<pmem::structs::DCPMEM_GET_DIMM_PARTITION_INFO_DATA>& s);

std::ostream& operator<<(std::ostream& os, const StructEnhanced<pmem::structs::DCPMEM_GET_SMART_INFO_DATA>& s);

std::ostream& operator<<(std::ostream& os, const StructEnhanced<pmem::structs::DCPMEM_GET_MEMORY_INFO_DATA>& s);

} // namespace mdr
