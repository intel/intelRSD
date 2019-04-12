/*!
 * @brief GetAuthorizedCertHash command interface.
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
 * @file get_authorized_cert_hash.hpp
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
 * @brief GetAuthorizedCertHash request command
 */
class GetAuthorizedCertHash : public Request {
public:
    /*!
     * @brief Constructor
     */
    GetAuthorizedCertHash();
    GetAuthorizedCertHash(const GetAuthorizedCertHash&) = default;
    GetAuthorizedCertHash(GetAuthorizedCertHash&&) = default;


    /*!
     * @brief Destructor
     */
    virtual ~GetAuthorizedCertHash();


    const char* get_command_name() const override {
        return "GetAuthorizedCertHash";
    }


    /*!
     * @brief Set certificate type
     * @param certificate_type Certificate type
     */
    void set_certificate_type(const CertificateType certificate_type) {
        m_certificate_type = certificate_type;
    }

private:
    void pack(IpmiInterface::ByteBuffer& data) const override;

    CertificateType m_certificate_type{CertificateType::PODM};
};

}

namespace response {

class GetAuthorizedCertHash : public Response {
public:
    using HashContainer = std::vector<std::uint8_t>;

    /*!
     * @brief Constructor
     */
    GetAuthorizedCertHash();
    GetAuthorizedCertHash(const GetAuthorizedCertHash&) = default;
    GetAuthorizedCertHash(GetAuthorizedCertHash&&) = default;


    /*!
     * @brief Destructor
     */
    virtual ~GetAuthorizedCertHash();


    const char* get_command_name() const override {
        return "GetAuthorizedCertHash";
    }


    /*!
     * @brief Certificate hash getter
     * @return Certificate hash
     */
    const HashContainer& get_certificate_hash() const {
        return m_certificate_hash;
    }




private:
    static constexpr std::size_t RESPONSE_SIZE = 17;
    static constexpr size_t MD5_HASH_LENGTH = 16;
    HashContainer m_certificate_hash{};


    void unpack(const IpmiInterface::ByteBuffer& data) override;
};

}

}
}
}
