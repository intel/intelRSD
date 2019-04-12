/*!
 * @copyright Copyright (c) 2018-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file supported_io_types.hpp
 */

#pragma once



#include <string>
#include "json-wrapper/json-wrapper.hpp"



namespace spdk {
namespace model {

/*!
 * @brief Implementation of SupportedIoTypes model based on SPDK
 */
class SupportedIoTypes {
public:

    /*!
     * @brief Lists current info about SupportedIoTypes
     * @return info about SupportedIoTypes as a string
     */
    std::string to_string() const;


    /*!
     * @brief Check whether the block device supports the I/O type - read
     * @return true if supports, false otherwise
     */
    bool is_read() const {
        return m_read;
    }


    /*!
     * @brief Sets I/O type read
     * @param read true for support, false otherwise
     */
    void set_read(bool read) {
        m_read = read;
    }


    /*!
     * @brief Check whether the block device supports the I/O type - write
     * @return true if supports, false otherwise
     */
    bool is_write() const {
        return m_write;
    }


    /*!
     * @brief Sets I/O types write
     * @param write true for support, false otherwise
     */
    void set_write(bool write) {
        m_write = write;
    }


    /*!
     * @brief Check whether the block device supports the I/O type - unmap
     * @return unmap true if supports, false otherwise
     */
    bool is_unmap() const {
        return m_unmap;
    }


    /*!
     * @brief Sets I/O types unmap
     * @param unmap true for support, false otherwise
     */
    void set_unmap(bool unmap) {
        m_unmap = unmap;
    }


    /*!
    * @brief Check whether the block device supports the I/O type - write
    * @return true if supports, false otherwise
    */
    bool is_write_zeroes() const {
        return m_write_zeroes;
    }


    /*!
     * @brief Sets I/O types write zeroes
     * @param write_zeroes true for support, false otherwise
     */
    void set_write_zeroes(bool write_zeroes) {
        m_write_zeroes = write_zeroes;
    }


    /*!
     * @brief Check whether the block device supports the I/O type - flush
     * @return true if supports, false otherwise
     */
    bool is_flush() const {
        return m_flush;
    }


    /*!
     * @brief Sets I/O types flush
     * @param flush true for support, false otherwise
     */
    void set_flush(bool flush) {
        m_flush = flush;
    }


    /*!
     * @brief Check whether the block device supports the I/O type - reset
     * @return true if supports, false otherwise
     */
    bool is_reset() const {
        return m_reset;
    }


    /*!
     * @brief Sets I/O types reset
     * @param reset true if supports, false otherwise
     */
    void set_reset(bool reset) {
        m_reset = reset;
    }


    /*!
     * @brief Check whether the block device supports the I/O type - nvme admin
     * @return true if supports, false otherwise
     */
    bool is_nvme_admin() const {
        return m_nvme_admin;
    }


    /*!
     * @brief Sets I/O type nvme_admin
     * @param nvme_admin true if supports, false otherwise
     */
    void set_nvme_admin(bool nvme_admin) {
        m_nvme_admin = nvme_admin;
    }


    /*!
     * @brief Check whether the block device supports the I/O type - nvme io
     * @return true if supports, false otherwise
     */
    bool is_nvme_io() const {
        return m_nvme_io;
    }


    /*!
     * @brief Sets I/O types nvme_io
     * @param nvme_io true for support, false otherwise
     */
    void set_nvme_io(bool nvme_io) {
        m_nvme_io = nvme_io;
    }


private:
    bool m_read{};
    bool m_write{};
    bool m_unmap{};
    bool m_write_zeroes{};
    bool m_flush{};
    bool m_reset{};
    bool m_nvme_admin{};
    bool m_nvme_io{};

};


/*!
 * @brief converts from SupportedIoTypes to Json
 * @param json Json object
 * @param supported_io_types SupportedIoTypes object
 */
void from_json(const json::Json& json, SupportedIoTypes& supported_io_types);


/*!
 * @brief converts from SupportedIoTypes to Json
 * @param json Json object
 * @param supported_io_types SupportedIoTypes object
 */
void to_json(json::Json& json, const SupportedIoTypes& supported_io_types);

}
}