/*!
 * @brief ACPI tables parser
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
 * @file acpi_parser.hpp
 */

#pragma once

#include "acpi_entry_point.hpp"
#include "structs/acpi_header.hpp"
#include "acpi/structs/acpi_nfit.hpp"
#include "acpi/structs/acpi_pcat.hpp"
#include "mdr/generic_parser.hpp"

#include <stdexcept>
#include <type_traits>

namespace acpi {
namespace parser {

/*!
 * Traits class for the ACPI tables parser.
 */
class AcpiTraits final {
public:
    /*!
     * @brief Exception type that will be thrown when an error condition is
     * discovered while parsing the ACPI table blob
     */
    class Exception : public std::runtime_error {
    public:
        /*!
         * @brief constructs exception object
         * @param what_arg Describes what happened
         */
        explicit Exception(const std::string& what_arg) :
            std::runtime_error("ACPI parser - " + what_arg) {
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
     * @param[in] header ACPI structure header.
     * @return true if equal, false otherwise.
     */
    template<typename T, typename H>
    static bool header_type_equal(const H& header, const uint16_t*) {
        return (T::ID == header.type) && (header.length == sizeof(T));
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

    using EntryPoint = AcpiEntryPoint;
};

/*!
 * @brief ACPI parser instantiation.
 */
using AcpiParser = mdr::GenericParser<AcpiTraits>;

} // namespace parser
} // namespace acpi

namespace mdr {

std::ostream& operator<<(std::ostream& os, const StructEnhanced<acpi::structs::NFIT_SPA_RANGE_STRUCTURE>& s);

std::ostream& operator<<(std::ostream& os, const StructEnhanced<acpi::structs::NFIT_NVDIMM_REGION_MAPPING_STRUCTURE>& s);

std::ostream& operator<<(std::ostream& os, const StructEnhanced<acpi::structs::NFIT_NVDIMM_CONTROL_REGION_STRUCTURE>& s);

std::ostream& operator<<(std::ostream& os, const StructEnhanced<acpi::structs::NFIT_NVDIMM_BLOCK_DATA_WINDOW_REGION_STRUCTURE>& s);

std::ostream& operator<<(std::ostream& os, const StructEnhanced<acpi::structs::NFIT_PLATFORM_CAPABILITIES_STRUCTURE>& s);

std::ostream& operator<<(std::ostream& os, const StructEnhanced<acpi::structs::PCAT_PLATFORM_CAPABILITY_INFORMATION_STRUCTURE>& s);

std::ostream& operator<<(std::ostream& os, const StructEnhanced<acpi::structs::PCAT_SOCKET_SKU_INFORMATION_STRUCTURE>& s);

} // namespace mdr