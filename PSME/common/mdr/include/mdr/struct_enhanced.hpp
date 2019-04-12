/*!
 * @brief Generic class used for IPMI blob parsing.
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
class BufferBasedStruct {
public:
    using DataType = decltype(T::data);
    using HeaderType = decltype(T::header);

    /*!
     * @brief Constructs BufferBasedStruct wrapping SMBIOS structure
     * @param structure_with_header reference to memory that contains structure of type T
     * @param _strings vector of strings accompanying structure
     * @param _raw_buffer shared_ptr that keeps the internal buffer alive as long as BufferBasedStruct exists
     */
    BufferBasedStruct(const T& structure_with_header, std::vector<std::string>&& _strings, std::shared_ptr<const uint8_t> _raw_buffer) :
            data(structure_with_header.data),
            header(structure_with_header.header),
            strings(std::move(_strings)),
            raw_buffer(_raw_buffer)
            {}

    /*!
     * @brief Default copy constructor.
     */
    BufferBasedStruct(const BufferBasedStruct&) = default;

    /*!
     * @brief Default move constructor.
     */
    BufferBasedStruct(BufferBasedStruct&&) = default;

    /*!
     * @brief Default assignment operator.
     */
    BufferBasedStruct& operator=(const BufferBasedStruct&) = default;

    /*!
     * @brief Default move assignment operator.
     */
    BufferBasedStruct& operator=(BufferBasedStruct&&) = default;

    /*!
     * @brief Default destructor.
     */
    ~BufferBasedStruct() = default;

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
struct StructEnhancedHelper{
    using type = BufferBasedStruct<T>;
};

/*!
 * @brief wrapper for MDR structs read by a GenericParser. For most structs, it's
 * a BufferBasedStruct, but specializing StructEnhancedHelper allows for providing a
 * custom wrapper.
 */
template<typename T>
using StructEnhanced = typename StructEnhancedHelper<T>::type;

/*!
 * @brief utility function for reading structs' strings from buffer
 * @param buf The MDR blob.
 * @param buf_size Size of the MDR blob.
 * @param[in,out] offset offset from the beginning of blob
 * @param[in,out] strings container to be filled with strings from buffer
 */
void read_auxiliary_strings(const uint8_t* buf, const size_t buf_size,
                            uint64_t& offset, std::vector<std::string>& strings);


template<typename T>
StructEnhanced<T> parse_struct(std::shared_ptr<const uint8_t> bufp, const size_t buf_size,
        uint64_t& offset, bool has_auxiliary_string) {
    auto buf = bufp.get();
    const T* structure = reinterpret_cast<const T*>(buf + offset);
    std::vector<std::string> strings;

    offset += structure->header.length;

    if (has_auxiliary_string) {
        // read strings that may follow structure
        read_auxiliary_strings(buf, buf_size, offset, strings);
    }

    StructEnhanced<T> entry{*structure, std::move(strings), bufp};

    return entry;
}

}  // namespace mdr
