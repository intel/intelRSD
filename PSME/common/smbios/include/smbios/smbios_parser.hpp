/*!
 * @copyright
 * Copyright (c) 2016-2017 Intel Corporation
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
 * @file signal.hpp
 *
 * @brief Signal interface
 * */

#pragma once
#include "structs/smbios_structs.hpp"
#include "smbios_entry_point.hpp"
#include <stdexcept>
#include <vector>
#include <type_traits>
#include <memory>

namespace smbios {
namespace parser {

/*!
 * @brief SmbiosParser parses System Management BIOS (Smbios) structures
 */
class SmbiosParser {
public:
    using Ptr = std::shared_ptr<SmbiosParser>;
    /*!
     * @brief Exception type that will be thrown when error condition is
     * discovered while parsing Smbios blob
     */
    class Exception : public std::runtime_error {
    public:
        /*!
         * @brief constructs exception object
         * @param what_arg Describes what hapened
         */
        explicit Exception(const std::string &what_arg) : std::runtime_error("Smbios parser - " + what_arg) { }

        Exception(const Exception &) = default;
        Exception& operator=(const Exception&) = default;
        Exception(Exception&&) = default;
        Exception& operator=(Exception&&) = default;


        virtual ~Exception();
    };

    /*!
     * @brief Represents structure of type T
     *
     * Objects of this type along with structure keep string table that
     * may follow the struct. It also has private shared_ptr to enclosing buffer that is
     * also shared by the parser.
     */
    template <typename T>
    class StructEnhanced {
    public:
        using DataType = decltype(T::data);
        /*!
         * @brief Constructs StructEnhanced wrapping SMBIOS structure
         * @param structure_with_header reference to memory that contains structure of type T
         * @param _strings vector of strings accompanying structure
         * @param _raw_buffer shared_ptr that keeps the internal buffer alive as long as StructEnhanced exists
         */
        StructEnhanced(const T& structure_with_header, std::vector<std::string>&& _strings, std::shared_ptr<const uint8_t> _raw_buffer) :
                data(structure_with_header.data),
                header(structure_with_header.header),
                strings(std::move(_strings)),
                raw_buffer(_raw_buffer)
                {}

        /*!
         * @brief const reference to header that comes along with structure
         */
        const SMBIOS_OOB_HEADER& header;

        /*!
         * @brief const reference to structure that it wraps
         */
        const DataType& data;

        /*!
         * @brief returns strings that are referred by struct T
         * @param[in] idx index of string to return (indices start from 1)
         */
        std::string get_string(unsigned idx) const {
            if (idx > 0 && idx <= strings.size()) {
                return strings[idx - 1];
            }
            return "";
        }

    private:
        // 'structure' field points to buffer kept in parser object
        // this shared pointer keeps parser alive as long as output from parser is alive
        std::shared_ptr<const uint8_t> raw_buffer {nullptr};
        std::vector<std::string> strings {};
    };

    /*!
     * @brief Type of collection of structures of given type
     */
    template<typename T>
    using StructVec = std::vector<StructEnhanced<T>>;

    /*!
     * @brief Construct parser object
     * @param _buf Buffer with Smbios blob
     * @param _buf_size size of buffer to
     */
    SmbiosParser(const uint8_t* _buf, size_t _buf_size) :
        buf_size(_buf_size),
        buf(init_buffer(_buf, _buf_size)),
        entry_point(read_entry_point()) { }


    /*!
     * @brief returns collection of structures of given type
     * @param[in] handle optional handle of structure to return
     * @return collection of structures
     */
    template<typename T>
    StructVec<T> get_all(const uint16_t* handle = nullptr) const;

protected:
    std::shared_ptr<const uint8_t> init_buffer(const uint8_t* _buf, size_t _buf_size);

    /*!
     * @brief reads and validates Smbios entry point table.
     * @return entry point structure
     */
    const SmbiosEntryPoint &read_entry_point() const;

    /*!
     * @brief casts pointer to particular structure
     * @param offset offset from the beginning of blob
     */
    template <typename T>
    StructEnhanced<T> read_struct(uint32_t& offset) const {
        const T* structure = reinterpret_cast<const T*>(buf.get() + offset);
        std::vector<std::string> strings;

        offset += structure->header.length;

        // read strings that may follow structure
        while (offset < buf_size && *(reinterpret_cast<const uint16_t*>(buf.get() + offset)) != 0) {
            uint32_t str_len = offset;
            while (str_len < buf_size && (*(buf.get() + str_len) != 0)) {
                str_len++;
            }
            strings.emplace_back(reinterpret_cast<const char*>(buf.get() + offset), (str_len - offset));
            offset = str_len;
            if (offset < buf_size && (*(reinterpret_cast<const uint16_t*>(buf.get() + offset)) != 0)) {
                offset++; //jump to the next string (skip single null that separates strings)
            }
        }

        // skip two null bytes
        offset += 2;

        StructEnhanced<T> entry{ *structure, std::move(strings), buf };

        return entry;
    }

    std::shared_ptr<const uint8_t> buf;
    const size_t buf_size;
    const SmbiosEntryPoint &entry_point;

    friend std::ostream &operator<<(std::ostream &os, const SmbiosParser &parser);
};


/*
 * Verifies structure length.
 * Default implementation validates only OEM structures, as the ones we parse
 * do not follow best extention guidelines. This approach allows to parse future
 * extensions of structures defined by SMBIOS spec.
 * @return true if length is acceptable, false otherwise.
 */
template <typename T>
bool is_valid_structure_length(const uint8_t length) {
    static_assert(std::is_pod<T>::value, "Invalid struct type");
    static constexpr uint8_t OEM_START = 0x80;
    static constexpr uint8_t OEM_END = 0xff;
    return (T::ID >= OEM_START && T::ID <= OEM_END) ? length == sizeof(T) : true;
}

std::ostream& operator<<(std::ostream& os, const SmbiosParser& parser);
std::ostream& operator<<(std::ostream& os, const SmbiosParser::StructEnhanced<SMBIOS_SYSTEM_INFO_DATA>& data);
std::ostream& operator<<(std::ostream& os, const SmbiosParser::StructEnhanced<SMBIOS_PCIE_INFO_DATA>& data);
std::ostream& operator<<(std::ostream& os, const SmbiosParser::StructEnhanced<SMBIOS_STORAGE_INFO_DATA>& data);
std::ostream& operator<<(std::ostream& os, const SmbiosParser::StructEnhanced<SMBIOS_STORAGE_INFO_DATA_V2>& data);
std::ostream& operator<<(std::ostream& os, const SmbiosParser::StructEnhanced<SMBIOS_NIC_INFO_DATA>& data);

template<typename T>
SmbiosParser::StructVec<T> SmbiosParser::get_all(const uint16_t* handle) const {
    static_assert(std::is_const<decltype(T::ID)>::value, "Invalid struct type");
    static_assert(std::is_pod<T>::value, "Invalid struct type");
    StructVec<T> collection;

    uint32_t offset = entry_point.struct_table_address;
    while (offset + sizeof(SMBIOS_OOB_HEADER) < buf_size) {
        const SMBIOS_OOB_HEADER& header = *reinterpret_cast<const SMBIOS_OOB_HEADER*>(buf.get() + offset);
        if (offset + header.length > buf_size) {
            throw Exception("Unexpectedly reached end of smbios blob");
        }
        if ((T::ID == header.type)
             && (nullptr == handle || *handle == header.handle)
             && is_valid_structure_length<T>(header.length)) {
            auto entry = read_struct<T>(offset);
            collection.emplace_back(std::move(entry));
        }
        else {
            offset += header.length;
            /* Get past trailing string-list - double-null */
            while (offset < buf_size && (*(reinterpret_cast<const uint16_t*>(buf.get() + offset)) != 0)) {
                offset++;
            }
            offset += 2; // jump to the next structure
        }
    }

    return collection;
}


}
}
