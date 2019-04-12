/*!
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
 * @file crc/crc32.hpp
 */

#pragma once

#include <cstdint>

namespace crc {

/*!
 * @brief Representation of the CRC32/zlib checksum
 * Other names: CRC-32/ADCCP, PKZIP
 */
class Crc32 final {
public:

    /*! Base type for CRC32 numeric representation */
    using BaseType = std::uint32_t;

    /*!
     * @brief Constructs an initial CRC32 checksum
     */
    Crc32(): m_crc(init()) {}

    /*
     * @brief Constructs a CRC32 object from its numeric representation
     */
    Crc32(const BaseType& crc): m_crc(crc) {}

    /*!
     * @brief Constructs CRC32 checksum from a buffer of a given size
     * @param buffer Pointer to the first byte of the buffer
     * @param size Size of the buffer
     */
    Crc32(const uint8_t* buffer, std::size_t size)
        : m_crc(calculate(init(), buffer, size)) {}

    /*!
     * @brief Constructs CRC32 checksum from a buffer of a given size
     * @param crc Previous CRC32 value
     * @param buffer Pointer to the first byte of the buffer
     * @param size Size of the buffer
     */
    Crc32(const BaseType& crc, const uint8_t* buffer, std::size_t size)
        : m_crc(calculate(crc, buffer, size)) {}

    /*! Enable copy and move constructors */
    Crc32(const Crc32&) = default;
    Crc32(Crc32&&) = default;

    /*! Enable copy and move assignment operators */
    Crc32& operator=(const Crc32&) = default;
    Crc32& operator=(Crc32&&) = default;

    /*! Destructor */
    ~Crc32() {}

    /*! Convertion to the numeric CRC32 representation */
    operator BaseType() const {
        return m_crc;
    }

    /*!
     * @brief Adds buffer data to the checksum and recalculates it
     * @return Reference to itself
     */
    Crc32& update(const uint8_t* buffer, std::size_t size) {
        m_crc = calculate(m_crc, buffer, size);
        return *this;
    }

    /*!
     * @brief Resets the CRC checksum, restores the initial value
     * @return Refernece to itself
     */
    Crc32& reset() {
        m_crc = init();
        return *this;
    }

private:

    static BaseType calculate(BaseType seed_crc, const uint8_t* buffer, std::size_t size);
    static BaseType init();

    BaseType m_crc{};
};

}
