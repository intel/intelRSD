/*!
 * @brief Generic class used for IPMI blob parsing.
 *
 * @copyright Copyright (c) 2017 Intel Corporation
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
 * @header{Files}
 * @file struct_enhanced.hpp
 */

#pragma once

#include <vector>
#include <type_traits>
#include <memory>

namespace mdr {

/*!
 * @brief Represents structure of type T
 *
 * Objects of this type along with structure keep string table that
 * may follow the struct. It also has private shared_ptr to enclosing buffer that is
 * also shared by the parser.
 */
template <typename T>
class StructEnhanced final {
public:
    using DataType = decltype(T::data);
    using HeaderType = decltype(T::header);

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
     * @brief Default copy constructor.
     */
    StructEnhanced(const StructEnhanced&) = default;

    /*!
     * @brief Default move constructor.
     */
    StructEnhanced(StructEnhanced&&) = default;

    /*!
     * @brief Default assignment operator.
     */
    StructEnhanced& operator=(const StructEnhanced&) = default;

    /*!
     * @brief Default move assignment operator.
     */
    StructEnhanced& operator=(StructEnhanced&&) = default;

    /*!
     * @brief Default destructor.
     */
    ~StructEnhanced() = default;

    /*!
     * @brief const reference to header that comes along with structure
     */
    const HeaderType& header;

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

template<typename T>
StructEnhanced<T> parse_struct(std::shared_ptr<const uint8_t> bufp, const size_t buf_size,
        uint64_t& offset) {
    auto buf = bufp.get();
    const T* structure = reinterpret_cast<const T*>(buf + offset);
    std::vector<std::string> strings;

    offset += structure->header.length;

	// read strings that may follow structure
    while (offset + 1 < buf_size && std::uint8_t(*(buf + offset + 1) | *(buf + offset)) != 0) {
        uint64_t str_len = offset;
        while (str_len < buf_size && (*(buf + str_len) != 0)) {
            str_len++;
        }
        strings.emplace_back(reinterpret_cast<const char*>(buf + offset), (str_len - offset));
        offset = str_len;
        if (offset + 1 < buf_size && std::uint8_t(*(buf + offset + 1) | *(buf + offset)) != 0) {
            offset++; // jump to the next string (skip single null that separates strings)
        }
    }

    // skip two null bytes
    offset += 2;

    StructEnhanced<T> entry{*structure, std::move(strings), bufp};

    return entry;
}

}  // namespace mdr
