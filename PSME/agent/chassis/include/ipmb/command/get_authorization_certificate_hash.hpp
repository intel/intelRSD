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
 * @file ipmb/command/get_authorization_certificate_hash.hpp
 * @brief GetAuthorizationCertificateHash IPMI command.
 * */

#pragma once
#include <ipmb/command/command.hpp>
#include "ipmb/certificate.hpp"

#include <vector>
/*! Agent namespace */

namespace agent {
/*! Chassis namespace */
namespace chassis {
/*! IPMB namespace */
namespace ipmb {
/*! Command namespace */
namespace command {

/*! GetAuthorizationCertificateHash command */
class GetAuthorizationCertificateHash final : public Command {
public:
    /*! Default constructor */
    GetAuthorizationCertificateHash(){}
    /*! Copy constructor */
    GetAuthorizationCertificateHash(const GetAuthorizationCertificateHash&) = default;
    /*! Assigment construcor */
    GetAuthorizationCertificateHash& operator=(const GetAuthorizationCertificateHash&) = default;

    /*!
     * Unpacks IPMI Message
     * @param msg IPMI Message
     * */
    void unpack(IpmiMessage& msg);

    /*!
     * Packs IPMI Message
     * @param msg IPMI Message
     * */
    void pack(IpmiMessage& msg);

    ~GetAuthorizationCertificateHash();

private:
    static const constexpr uint8_t CMD_REQUEST_DATA_LENGTH = 1;
    static const constexpr uint8_t CMD_RESPONSE_DATA_LENGTH = 17;

    static const constexpr uint8_t REQ_OFFSET_CERT_TYPE = 0;

    static const constexpr uint8_t OFFSET_CC = 0;
    static const constexpr uint8_t OFFSET_HASH = 1;

    //static const constexpr uint8_t CC_REQ_DATA_NOT_PRESENT = 0xCB;

    class Request final : public Command::Request {
    private:
        Certificate::Type m_certificate_type{};

    public:
        /*!
         * @brief Set certificate type
         * @param type certificate type
         * */
        void set_certificate_type(Certificate::Type type) {
            m_certificate_type = type;
        }

        /*!
         * @brief Get certificate type
         * @return certicicate type
         * */
        Certificate::Type get_certificate_type() const {
            return m_certificate_type;
        }
    };

    class Response final : public Command::Response {
    private:
        std::vector<uint8_t> m_certificate_hash{};

    public:
        /*!
         * Fill Data buffer
         * @param msg IPMI Message
         * */
        void add_data(IpmiMessage& msg);

        /*!
         * @brief Get certificate hash
         * @return certificate hash
         * */
        std::vector<uint8_t>& get_certificate_hash() {
            return m_certificate_hash;
        }

        /*!
         * @brief Set certificate hash
         * @param hash certificate hash
         * */
        void set_certificate_hash(const std::vector<uint8_t>& hash) {
            m_certificate_hash = hash;
        }
    };

    Response m_response{};
    Request m_request{};

};

}
}
}
}
