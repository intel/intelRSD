/*!
 * @brief Utils for handling little/big endian byte buffers
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file utils/byte_buffer.hpp
 */

#pragma once

#include <cstdint>
#include <vector>
#include <string>

namespace utils {

/*!
 * @brief Raw byte buffer with methods for little/big endian interpretation of data stored in it.
 */
class ByteBuffer final {
public:

    using iterator = std::vector<uint8_t>::iterator;
    using const_iterator = std::vector<uint8_t>::const_iterator;

    ByteBuffer() {}

    ~ByteBuffer() {}

    /*!
     * @brief Constructs a valid byte buffer
     * @param len Desired size of the buffer
     */
    ByteBuffer(std::size_t len): m_vec(len) {}

    /*! Enable copy & move constructors */
    ByteBuffer(const ByteBuffer&) = default;
    ByteBuffer(ByteBuffer&&) = default;
    ByteBuffer& operator=(const ByteBuffer& rhs) = default;
    ByteBuffer& operator=(ByteBuffer&& rhs) = default;

    /*! Enable copy & move constructors from vectors */
    ByteBuffer(const std::vector<uint8_t>& rhs): m_vec{rhs} {}
    ByteBuffer(std::vector<uint8_t>&& rhs): m_vec{std::move(rhs)} {}

    /*! Enable copy operator= from vectors */
    ByteBuffer& operator=(const std::vector<uint8_t>& rhs) {
        m_vec = rhs;
        return *this;
    }

    /*! Enable move operator= from vectors */
    ByteBuffer& operator=(std::vector<uint8_t>&& rhs) {
        m_vec = std::move(rhs);
        return *this;
    }

    /*! Conversion to vector<uint8_t> */
    operator std::vector<uint8_t>() {
        return m_vec;
    }

    /*! Forwards vector [] operator */
    uint8_t& operator[](std::size_t pos) {
        return m_vec[pos];
    }

    /*! Forwards vector [] const operator */
    const uint8_t& operator[](std::size_t pos) const {
        return m_vec[pos];
    }

    /*! Forwards vector at method */
    uint8_t& at(std::size_t pos) {
        return m_vec.at(pos);
    }

    /*! Forwards vector at const method */
    const uint8_t& at(std::size_t pos) const {
        return m_vec.at(pos);
    }

    /*! Forwards vector data method */
    uint8_t* data() {
        return m_vec.data();
    }

    /*! Forwards vector data const method */
    const uint8_t* data() const {
        return m_vec.data();
    }

    /*! Forwards vector empty method */
    bool empty() const {
        return m_vec.empty();
    }

    /*! Forwards vector size method */
    std::size_t size() const {
        return m_vec.size();
    }

    /*! Forwards vector begin method */
    iterator begin() {
        return m_vec.begin();
    }

    /*! Forwards vector begin const method */
    const_iterator begin() const {
        return m_vec.begin();
    }

    /*! Forwards vector cbegin method */
    const_iterator cbegin() const {
        return m_vec.cbegin();
    }

    /*! Forwards vector end method */
    iterator end() {
        return m_vec.end();
    }

    /*! Forwards vector end const method */
    const_iterator end() const {
        return m_vec.end();
    }

    /*! Forwards vector cend method */
    const_iterator cend() const {
        return m_vec.cend();
    }

    /*! Forwards vector clear method */
    void clear() {
        return m_vec.clear();
    }

    /*!
     * @brief Returns the raw reference to the vector
     * @return Reference to the internally stored vector
     */
    std::vector<uint8_t>& get_raw_vector() {
        return m_vec;
    }

    /*!
     * @brief Returns the raw const reference to the vector
     * @return Const reference to the internally stored vector
     */
    const std::vector<uint8_t>& get_raw_vector() const {
        return m_vec;
    }

    /*!
     * @brief Reads 2 bytes from the buffer and interprets them as a little endian number
     * @param pos First byte position
     * @return Read value
     */
    uint16_t get_le16(std::size_t pos) const;

    /*!
     * @brief Reads 4 bytes from the buffer and interprets them as a little endian number
     * @param pos First byte position
     * @return Read value
     */
    uint32_t get_le32(std::size_t pos) const;

    /*!
     * @brief Reads 6 bytes from the buffer and interprets them as a little endian number
     * @param pos First byte position
     * @return Read value
     */
    uint64_t get_le48(std::size_t pos) const;

    /*!
     * @brief Reads 8 bytes from the buffer and interprets them as a little endian number
     * @param pos First byte position
     * @return Read value
     */
    uint64_t get_le64(std::size_t pos) const;

    /*!
     * @brief Reads 2 bytes from the buffer and interprets them as a big endian number
     * @param pos First byte position
     * @return Read value
     */
    uint16_t get_be16(std::size_t pos) const;

    /*!
     * @brief Reads 4 bytes from the buffer and interprets them as a big endian number
     * @param pos First byte position
     * @return Read value
     */
    uint32_t get_be32(std::size_t pos) const;

    /*!
     * @brief Reads 6 bytes from the buffer and interprets them as a big endian number
     * @param pos First byte position
     * @return Read value
     */
    uint64_t get_be48(std::size_t pos) const;

    /*!
     * @brief Reads 8 bytes from the buffer and interprets them as a big endian number
     * @param pos First byte position
     * @return Read value
     */
    uint64_t get_be64(std::size_t pos) const;

    /*!
     * @brief Sets 2 bytes in the buffer in the form of a little endian number
     * @param pos Position of the first byte
     * @param value Value to be stored
     */
    void set_le16(std::size_t pos, uint16_t value);

    /*!
     * @brief Sets 4 bytes in the buffer in the form of a little endian number
     * @param pos Position of the first byte
     * @param value Value to be stored
     */
    void set_le32(std::size_t pos, uint32_t value);

    /*!
     * @brief Sets 6 bytes in the buffer in the form of a little endian number
     * @param pos Position of the first byte
     * @param value Value to be stored
     */
    void set_le48(std::size_t pos, uint64_t value);

    /*!
     * @brief Sets 8 bytes in the buffer in the form of a little endian number
     * @param pos Position of the first byte
     * @param value Value to be stored
     */
    void set_le64(std::size_t pos, uint64_t value);

    /*!
     * @brief Sets 2 bytes in the buffer in the form of a big endian number
     * @param pos Position of the first byte
     * @param value Value to be stored
     */
    void set_be16(std::size_t pos, uint16_t value);

    /*!
     * @brief Sets 4 bytes in the buffer in the form of a big endian number
     * @param pos Position of the first byte
     * @param value Value to be stored
     */
    void set_be32(std::size_t pos, uint32_t value);

    /*!
     * @brief Sets 6 bytes in the buffer in the form of a big endian number
     * @param pos Position of the first byte
     * @param value Value to be stored
     */
    void set_be48(std::size_t pos, uint64_t value);

    /*!
     * @brief Sets 8 bytes in the buffer in the form of a big endian number
     * @param pos Position of the first byte
     * @param value Value to be stored
     */
    void set_be64(std::size_t pos, uint64_t value);

private:
    std::vector<uint8_t> m_vec{};
};

}
