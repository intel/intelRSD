/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * @section DESCRIPTION
 *
 * @file ipmb/certificate.hpp
 * @brief Certificate
 * */

#pragma once
#include <cstdint>
#include <vector>
#include <map>
#include <mutex>

namespace agent {
namespace chassis {
namespace ipmb {

/*!
 * @brief Certificate class
 * */
class Certificate {
public:
    /*!
     * @brief Certificate enum class
     * */
    enum class Type : std::uint8_t {
        PODM = 0x00,
        RMM = 0x01
    };

    /*!
     * @brief Get length
     *
     * @return Length of the certificate
     * */
    size_t get_length() const {
        return m_length;
    }

    /*!
     * @brief Add chunk
     *
     * @param cert_length Length of the whole certificate in bytes
     * @param chunk_number Number of currently sent chunk
     * @param data Chunk in form of a vector of bytes
     * */
    void add_chunk(size_t cert_length, size_t chunk_number,
                   const std::vector<uint8_t>& data);

    /*!
     * @brief Get chunk
     *
     * @param chunk_number number of requested chunk
     *
     * @return Chunk in form of a vector of bytes
     * */
    const std::vector<uint8_t>& get_chunk(size_t chunk_number) const;

    /*!
     * @brief Get hash
     *
     * @return Certificate hash in form of a vector of bytes
     * */
    std::vector<uint8_t> get_hash() const;

    /*!
     * @brief Get data
     *
     * @return Certificate data if form of a vector of bytes
     * */
    std::vector<std::uint8_t> get_data() const;

    /*!
     * @brief From string
     *
     * @param type Type of certificate in form of a string
     *
     * @return Enum type of certificate
     * */
    static Certificate::Type from_string(const std::string& type);

    /*!
     * @brief Is complete
     *
     * @return true if the certificate is complete, false otherwise
     * */
    bool is_complete() const;

private:
    std::map<size_t, std::vector<uint8_t>> m_chunks{};
    size_t m_length{};
    size_t m_received{};
    mutable std::mutex m_mutex{};

    void make_md5_hash(const std::vector<std::uint8_t>& input,
                             std::vector<std::uint8_t>& output) const;
};

}
}
}

