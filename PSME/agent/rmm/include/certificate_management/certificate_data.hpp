/*!
 * @brief Rmm CertificateData class interface
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
 * @file certificate_data.hpp
 */

#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <mutex>



extern "C" {
#include <time.h>
}

namespace agent {
namespace rmm {

class CertificateDataReference;

/*!
 * @brief Container for all data connected to a certificate.
 *
 * This class serves as a container for storing all data connected to a certificate:
 * - certificate type, as defined by CertificateType enum
 * - path to certificate file
 * - certificate hash
 * - certificate data
 * - certificate last modification time
 * It should be noted that this class does not take care of maintaining consistency
 * between the certificate data and its hash - this should be taken care of
 * by higher layers.
 */
class CertificateData {

    friend class CertificateDataReference;

public:
    using CertificateHashType = std::vector<std::uint8_t>;
    using CertificateDataType = std::vector<std::uint8_t>;

    /*!
     * @brief Enumeration type for possible certificate types.
     */
    enum class CertificateType {
        PODM,
        RMM
    };

    /*!
     * @brief Constructor.
     * @param certificate_type Type of certificate to be stored.
     */
    CertificateData(CertificateType certificate_type) : m_certificate_type(certificate_type) {}


    /*!
     * @brief Destructor.
     */
    virtual ~CertificateData();


    /*!
     * @brief Get cerificate type.
     * @return Certificate type.
     */
    CertificateType get_certificate_type() const {
        return m_certificate_type;
    }


    /*!
     * @brief Set certificate file path.
     * @param file_path Certificate file path.
     * @return Reference to object acted upon.
     */
    CertificateData& set_certificate_file_path(const std::string& file_path) {
        m_certificate_file_path = file_path;
        return *this;
    };


    /*!
     * @brief Get certificate file path.
     * @return Certificate file path.
     */
    const std::string& get_certificate_file_path() const {
        return m_certificate_file_path;
    }


    /*!
     * @brief Set certificate hash.
     * @param certificate_hash Certificate hash.
     * @return Reference to object acted upon.
     */
    CertificateData& set_certificate_hash(const CertificateHashType& certificate_hash) {
        m_certificate_hash = certificate_hash;
        return *this;
    }


    /*!
     * @brief Get certificate hash.
     * @return Certificate hash.
     */

    const CertificateHashType& get_certificate_hash() const {
        return m_certificate_hash;
    }


    /*!
     * @brief Set certificate data.
     * @param certificate_data Certificate data.
     * @return Reference to object acted upon.
     */
    CertificateData& set_certificate_data(const CertificateDataType& certificate_data) {
        m_certificate_data = certificate_data;
        return *this;
    }


    /*!
     * @brief Get certificate data.
     * @return Certificate data.
     */
    const CertificateDataType& get_certificate_data() const {
        return m_certificate_data;
    }


    /*!
     * @brief Set certificate last modification time.
     * @param last_modification_time Certificate last modification time.
     * @return Reference to object acted upon.
     */
    CertificateData& set_last_modification_time(const timespec& last_modification_time) {
        m_last_modification_time = last_modification_time;
        return *this;
    }


    /*!
     * @brief Get certificate file last modification time.
     * @return Certificate file last modification time.
     */
    const timespec& get_last_modification_time() const {
        return m_last_modification_time;
    }

protected:
    const CertificateType m_certificate_type;
    std::string m_certificate_file_path{};
    CertificateHashType m_certificate_hash{};
    CertificateDataType m_certificate_data{};
    timespec m_last_modification_time{0, 0};

    std::mutex m_mutex{};
};


/*!
 * @brief Class implementing RAII object to lock certificate data
 */
class CertificateDataReference final {

    friend class CertificateManager;

public:
    CertificateDataReference(CertificateData& certificate_data):
        m_certificate_data(certificate_data) {
        m_certificate_data.m_mutex.lock();
    }

    ~CertificateDataReference() {
        m_certificate_data.m_mutex.unlock();
    }

    CertificateData& operator*() {
        return m_certificate_data;
    }

private:
    CertificateData& m_certificate_data;
};

}
}
