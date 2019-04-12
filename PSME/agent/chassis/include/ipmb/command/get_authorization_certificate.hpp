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
 * @file ipmb/command/get_authorization_certificate.hpp
 * @brief GetAuthorizationCertificate IPMI command.
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

/*! GetAuthorizationCertificate command */
class GetAuthorizationCertificate final : public Command {
public:
    /*! Default constructor */
    GetAuthorizationCertificate(){}
    /*! Copy constructor */
    GetAuthorizationCertificate(const GetAuthorizationCertificate&) = default;
    /*! Assignment constructor */
    GetAuthorizationCertificate& operator=(const GetAuthorizationCertificate&)
                                            = default;
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

    ~GetAuthorizationCertificate();

private:
    static const constexpr uint8_t CMD_REQUEST_DATA_LENGTH = 2;
    static const constexpr uint8_t CMD_MIN_RESPONSE_DATA_LENGTH = 5;

    static const constexpr uint8_t REQ_OFFSET_CERT_TYPE = 0;
    static const constexpr uint8_t REQ_OFFSET_CHUNK = 1;

    static const constexpr uint8_t OFFSET_CC = 0;
    static const constexpr uint8_t OFFSET_CERT_LEN = 1;
    static const constexpr uint8_t OFFSET_CHUNK_NO = 3;
    static const constexpr uint8_t OFFSET_CRNT_CHUNK_LEN = 4;
    static const constexpr uint8_t OFFSET_CERT = 5;

    class Request final : public Command::Request {
    private:
        Certificate::Type m_certificate_type{};
        uint8_t m_current_chunk_number{};

    public:
        /*!
         * @brief Set certificate type
         * @param type certificate type
         * */
        void set_certificate_type(Certificate::Type type) {
            m_certificate_type = type;
        }

        /*!
         * @brief Set current chunk number
         * @param number current chunk number
         * */
        void set_current_chunk_number(uint8_t number) {
            m_current_chunk_number = number;
        }

        /*!
         * @brief Get current chunk number
         * @return current chunk number
         * */
        uint8_t get_current_chunk_number() const {
            return m_current_chunk_number;
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
        size_t m_certificate_length{};
        uint8_t m_current_chunk_number{};
        uint8_t m_current_chunk_length{};
        std::vector<uint8_t> m_certificate_data{};

    public:
        /*!
         * Fill data buffer
         * @param msg IPMI Message
         * */
        void add_data(IpmiMessage& msg);

        /*!
         * Sets certificate length
         * @param length Certificate length
         * */
        void set_certificate_length(const size_t length) {
            m_certificate_length = length;
        }

        /*!
         * Gets certificate length
         * @return certificate length
         * */
        size_t get_certificate_length() const {
            return m_certificate_length;
        }

        /*!
         * @brief Set current chunk number
         * @param number current chunk number
         * */
        void set_current_chunk_number(const uint8_t number) {
            m_current_chunk_number = number;
        }

        /*!
         * @brief Get current chunk number
         * @return current chunk number
         * */
        uint8_t get_current_chunk_number() const {
            return m_current_chunk_number;
        }

        /*!
         * @brief Set current chunk length
         * @param length current chunk length
         * */
        void set_current_chunk_length(const uint8_t length) {
            m_current_chunk_length = length;
        }

        /*!
         * @brief Get current chunk number
         * @return Current chunk number
         * */
        uint8_t get_current_chunk_length() const {
            return m_current_chunk_length;
        }

        /*!
         * @brief Get certificate data
         * @return certificate data
         * */
        std::vector<uint8_t>& get_certificate_data() {
            return m_certificate_data;
        }

        /*!
         * @brief Set certificate data
         * @param data certificate data
         * */
        void set_certificate_data(const std::vector<uint8_t> data) {
            m_certificate_data = data;
        }
    };

    Response m_response{};
    Request m_request{};

};

}
}
}
}


