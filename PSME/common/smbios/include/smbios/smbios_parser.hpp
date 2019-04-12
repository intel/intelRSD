/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 * @file smbios_parser.hpp
 *
 * @brief Smbios parser interface
 * */

#pragma once



#include "structs/smbios_structs.hpp"
#include "smbios_entry_point.hpp"
#include "mdr/generic_parser.hpp"

#include <stdexcept>
#include <type_traits>
#include <memory>



namespace smbios {
namespace parser {

template<typename T>
bool is_valid_structure_length(const uint8_t length) {
    static_assert(std::is_pod<T>::value, "Invalid struct type");
    static constexpr uint8_t OEM_START = 0x80;
    static constexpr uint8_t OEM_END = 0xff;
    return (T::ID >= OEM_START && T::ID <= OEM_END) ? length == sizeof(T) : length >= sizeof(T);
}

/*!
 * @brief specialization for Speed Select structure, which has variable length, as it can contain
 * zero or more configurations.
 */
template<>
bool is_valid_structure_length<SMBIOS_SPEED_SELECT_INFO_DATA>(const uint8_t length);


/*!
 * Traits class for the SMBIOS MDR parser.
 */
class SmbiosMdrTraits final {
public:
    /*!
     * @brief Exception type that will be thrown when an error condition is
     * discovered while parsing the SMBIOS MDR blob
     */
    class Exception : public std::runtime_error {
    public:
        /*!
         * @brief constructs exception object
         * @param what_arg Describes what happened
         */
        explicit Exception(const std::string& what_arg) :
            std::runtime_error("Smbios parser - " + what_arg) {
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
     * @param[in] header SMBIOS structure header.
     * @return true if equal, false otherwise.
     */
    template<typename T, typename H>
    static bool header_type_equal(const H& header, const uint16_t* handle) {
        return ((T::ID == header.type) && (nullptr == handle || *handle == header.handle)
                && is_valid_structure_length<T>(header.length));
    }


    /*!
     * @brief Get the length of the SMBIOS table without strings.
     *
     * @param[in] header SMBIOS structure header.
     * @return length of the SMBIOS table.
     */
    template<typename H>
    static auto table_length(const H& header) -> decltype(header.length) {
        return header.length;
    }


    using EntryPoint = SmbiosEntryPoint;

};

/*!
 * @brief SMBIOS parser instantiation.
 */
using SmbiosParser = mdr::GenericParser<SmbiosMdrTraits>;

}
}

namespace mdr {

/*!
 * @brief specialization for Speed Select structure, with extra container to store Configurations.
 */
class SpeedSelectEnhanced : public BufferBasedStruct<smbios::parser::SMBIOS_SPEED_SELECT_INFO_DATA> {
public:
    using Configs = std::vector<smbios::parser::SPEED_SELECT_CONFIGURATION>;

    SpeedSelectEnhanced(const smbios::parser::SMBIOS_SPEED_SELECT_INFO_DATA& structure_with_header,
                        Configs&& _configs,
                        std::vector<std::string>&& _strings,
                        std::shared_ptr<const uint8_t> _raw_buffer) :
        BufferBasedStruct<smbios::parser::SMBIOS_SPEED_SELECT_INFO_DATA>(structure_with_header, std::move(_strings), _raw_buffer),
        configs(std::move(_configs)) { }

    /*!
     * @brief const Speed Select configurations read for a Speed Select Info structure
     */
    const Configs configs;
};

template<>
struct StructEnhancedHelper<smbios::parser::SMBIOS_SPEED_SELECT_INFO_DATA>{
    using type = SpeedSelectEnhanced;
};

/*!
 * @brief specialization for Speed Select structure, with extra code to handle reading
 * variable number of Configurations.
 */
template<>
StructEnhanced<smbios::parser::SMBIOS_SPEED_SELECT_INFO_DATA> parse_struct<smbios::parser::SMBIOS_SPEED_SELECT_INFO_DATA>(
    std::shared_ptr<const uint8_t> bufp, const size_t buf_size, uint64_t& offset, bool has_auxiliary_string);


std::ostream& operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_BIOS_INFO_DATA>& s);


std::ostream& operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_MODULE_INFO_DATA>& s);


std::ostream& operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_SYSTEM_INFO_DATA>& data);


std::ostream& operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_PCIE_INFO_DATA>& data);


std::ostream& operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_PROCESSOR_INFO_DATA>& s);


std::ostream& operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_CPUID_DATA>& data);


std::ostream& operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_CPUID_DATA_V2>& data);


std::ostream& operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_STORAGE_INFO_DATA>& data);


std::ostream& operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_STORAGE_INFO_DATA_V2>& data);


std::ostream& operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_NIC_INFO_DATA>& data);


std::ostream& operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_NIC_INFO_DATA_V2>& data);


std::ostream& operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_FPGA_DATA>& data);


std::ostream& operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_FPGA_DATA_OEM>& data);


std::ostream& operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_STORAGE_DEVICE_INFO_DATA>& data);


std::ostream& operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_TPM_INFO_DATA>& data);


std::ostream& operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_TXT_INFO_DATA>& data);


std::ostream& operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_MEMORY_DEVICE>& data);


std::ostream&
operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_MEMORY_DEVICE_EXTENDED_INFO_DATA>& data);


std::ostream& operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_PCIE_PORT_INFO_DATA>& data);


std::ostream& operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_SPEED_SELECT_INFO_DATA>& data);

}  // namespace mdr
