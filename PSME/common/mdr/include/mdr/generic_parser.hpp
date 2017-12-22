/*!
 * @brief Generic SMBIOS style structure parser.
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
 * @file generic_parser.hpp
 */

#pragma once

#include "mdr/struct_enhanced.hpp"

#include <memory>
#include <vector>
#include <type_traits>
#include <ostream>

namespace mdr {

/*!
 * @brief A generic parser for SMBIOS like blobs.
 */
template<typename Traits>
class GenericParser {
public:
    using Ptr = std::shared_ptr<GenericParser>;

    template<typename T>
    using StructEnhanced = mdr::StructEnhanced<T>;

    /*!
     * @brief Exception type that will be thrown when error condition is
     * discovered while parsing blob.
     */
    using Exception = typename Traits::Exception;

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
    GenericParser(const uint8_t* _buf, size_t _buf_size) :
        buf_size(_buf_size),
        buf(init_buffer(_buf, _buf_size)),
        entry_point{Traits::EntryPoint::create(buf.get(), buf_size)} {}

    /*!
     * @brief Default copy constructor.
     */
    GenericParser(const GenericParser&) = default;

    /*!
     * @brief Default move constructor.
     */
    GenericParser(GenericParser&&) = default;

    /*!
     * @brief Default assignment operator.
     */
    GenericParser& operator=(const GenericParser&) = default;

    /*!
     * @brief Default move assignment operator.
     */
    GenericParser& operator=(GenericParser&&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~GenericParser() = default;

    /*!
     * @brief returns collection of structures of given type
     * @param[in] handle optional handle of structure to return
     * @return collection of structures
     */
    template<typename T>
    StructVec<T> get_all(const uint16_t* handle = nullptr) const;

protected:
    /*!
     * @brief Initialize the internal buffer.
     * @param[in] _buf The MDR blob.
     * @param[in] _buf_size Size of the MDR blob.
     */
    std::shared_ptr<const uint8_t> init_buffer(const uint8_t* _buf, size_t _buf_size);

    /*!
     * @brief casts pointer to particular structure
     * @param offset offset from the beginning of blob
     */
    template <typename T>
    StructEnhanced<T> read_struct(uint64_t& offset) const {
        return mdr::parse_struct<T>(buf, buf_size, offset);
    }

    std::shared_ptr<const uint8_t> buf;
    const size_t buf_size;
    typename Traits::EntryPoint::Ptr entry_point{};

    template<typename T>
    friend std::ostream &operator<<(std::ostream &os, const GenericParser<T> &parser);
};

template<typename Traits>
template<typename T>
GenericParser<Traits>::StructVec<T> GenericParser<Traits>::get_all(const uint16_t* handle) const {
    static_assert(std::is_const<decltype(T::ID)>::value, "Invalid struct type");
    static_assert(std::is_pod<T>::value, "Invalid struct type");
    StructVec<T> collection;

    using HeaderType = decltype(T::header);

    auto offset = entry_point->get_struct_table_address();
    while (offset + sizeof(HeaderType) < buf_size) {
        const HeaderType& header = *reinterpret_cast<const HeaderType*>(buf.get() + offset);
        if (offset + Traits::template table_length(header) > buf_size) {
            throw Exception("Unexpectedly reached end of MDR blob");
        }

        if (Traits::template header_type_equal<T>(header, handle)) {
            collection.emplace_back(read_struct<T>(offset));
        } else {
            offset += Traits::template table_length(header);
            /* Get past trailing string-list - double-null */
            while (offset + 1 < buf_size
                       && std::uint8_t(*(buf.get() + offset + 1) | *(buf.get() + offset)) != 0) {
                    offset++;
                }
            offset += 2; // jump to the next structure
        }
    }

    return collection;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const GenericParser<T>& parser);

}  // namespace mdr
