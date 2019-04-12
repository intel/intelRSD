/*!
 * @brief SetAuthorizedCert command interface.
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
 * @file set_authorized_cert.hpp
 */

#pragma once



#include "ipmi/request.hpp"
#include "ipmi/response.hpp"
#include "ipmi/command/sdv/enums.hpp"



namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief SetAuthorizedCert request command
 */
class SetAuthorizedCert : public Request {
public:
    using CertificateChunk = std::vector<std::uint8_t>;


    /*!
     * @brief Constructor
     */
    SetAuthorizedCert();
    SetAuthorizedCert(const SetAuthorizedCert&) = default;
    SetAuthorizedCert(SetAuthorizedCert&&) = default;


    /*!
     * @brief Destructor
     */
    virtual ~SetAuthorizedCert();


    const char* get_command_name() const override {
        return "SetAuthorizedCert";
    }


    /*!
     * @brief Set certificate type
     * @param certificate_type Certificate type
     */
    void set_certificate_type(const CertificateType certificate_type) {
        m_certificate_type = certificate_type;
    }


    /*!
     * @brief Set certificate total length
     * @param certificate_length Certificate length
     */
    void set_certificate_length(const std::uint16_t certificate_length) {
        m_certificate_length = certificate_length;
    }


    /*!
     * @brief Set current certificate data chunk number
     * @param current_chunk_number Current certificate data chunk number
     */
    void set_current_chunk_number(const std::uint8_t current_chunk_number) {
        m_current_chunk_number = current_chunk_number;
    }


    /*!
     * @brief Set current certificate chunk data
     * @param certificate_chunk Current certificate chunk data
     */
    void set_certificate_chunk(const CertificateChunk& certificate_chunk) {
        m_certificate_chunk = certificate_chunk;
        m_current_chunk_length = static_cast<std::uint8_t>(certificate_chunk.size());
    }


private:
    void pack(IpmiInterface::ByteBuffer& data) const override;

    CertificateType m_certificate_type{CertificateType::PODM};
    std::uint16_t m_certificate_length{};
    std::uint8_t m_current_chunk_number{};
    std::uint8_t m_current_chunk_length{};
    CertificateChunk m_certificate_chunk{};
};

}

namespace response {

class SetAuthorizedCert : public Response {
public:
    /*!
     * @brief Constructor
     */
    SetAuthorizedCert();
    SetAuthorizedCert(const SetAuthorizedCert&) = default;
    SetAuthorizedCert(SetAuthorizedCert&&) = default;


    /*!
     * @brief Destructor
     */
    virtual ~SetAuthorizedCert();


    const char* get_command_name() const override {
        return "SetAuthorizedCert";
    }


private:
    static constexpr std::size_t RESPONSE_SIZE = 1;

    void unpack(const IpmiInterface::ByteBuffer&) override {}
};

}

}
}
}
