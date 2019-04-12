/*!
 * @brief Parser for the iSCSI MDR region.
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
 * @file iscsi_mdr_parser.hpp
 */

#pragma once

#include "iscsi/structs/iscsi_mdr_attempt.hpp"
#include "iscsi/structs/iscsi_mdr_header.hpp"
#include "iscsi/structs/iscsi_mdr_initiator.hpp"
#include "iscsi/structs/iscsi_mdr_target.hpp"
#include "iscsi/structs/iscsi_mdr_version.hpp"
#include "mdr/struct_enhanced.hpp"
#include "mdr/generic_parser.hpp"
#include "iscsi/iscsi_mdr_entry_point.hpp"

#include <stdexcept>
#include <vector>
#include <type_traits>
#include <memory>
#include <iosfwd>

namespace iscsi {
namespace parser {

/*!
 * Traits class for the iSCSI MDR parser.
 */
class IscsiMdrTraits final {
public:
    /*!
     * @brief Exception type that will be thrown when an error condition is
     * discovered while parsing the iSCSI MDR blob
     */
    class Exception: public std::runtime_error {
    public:
        /*!
         * @brief constructs exception object
         * @param what_arg Describes what happened
         */
        explicit Exception(const std::string& what_arg) :
            std::runtime_error("iSCSI MDR parser - " + what_arg) {
        }

        Exception(const Exception&) = default;
        Exception& operator=(const Exception&) = default;
        Exception(Exception&&) = default;
        Exception& operator=(Exception&&) = default;

        virtual ~Exception();
    };

    /*!
     * @brief Check for header type equality.
     *
     * @param[in] header iSCSI structure header.
     * @return true if equal, false otherwise.
     */
    template<typename T, typename H>
    static bool header_type_equal(const H& header, const uint16_t*) {
        return (T::ID == header.parameter_id) && (header.length == sizeof(T));
    }

    /*!
     * @brief Get the length of the iSCSI table without strings.
     *
     * @param[in] header iSCSI structure header.
     * @return length of the iSCSI table.
     */
    template<typename H>
    static auto table_length(const H& header) -> decltype(header.length) {
        // assumes SMBIOS-like length field meaning
        return header.length;
    }

    using EntryPoint = IscsiMdrEntryPoint;

};

/*!
 * @brief iSCSI parser instantiation.
 */
using IscsiMdrParser = mdr::GenericParser<IscsiMdrTraits>;

}  // namespace parser

namespace structs {

// forward declarations of structures
struct ISCSI_MDR_ATTEMPT;
struct ISCSI_MDR_INITIATOR;
struct ISCSI_MDR_TARGET;
struct ISCSI_MDR_VERSION;

}  // namespace structs
}  // namespace iscsi

namespace mdr {

std::ostream& operator<<(std::ostream& os, const StructEnhanced<iscsi::structs::ISCSI_MDR_ATTEMPT>& se);
std::ostream& operator<<(std::ostream& os, const StructEnhanced<iscsi::structs::ISCSI_MDR_INITIATOR>& se);
std::ostream& operator<<(std::ostream& os, const StructEnhanced<iscsi::structs::ISCSI_MDR_TARGET>& se);
std::ostream& operator<<(std::ostream& os, const StructEnhanced<iscsi::structs::ISCSI_MDR_VERSION>& se);

}  // namespace mdr

