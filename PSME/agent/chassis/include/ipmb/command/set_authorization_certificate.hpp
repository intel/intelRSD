/*!
 * @section LICENSE
 *
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
 * @file ipmb/command/set_authorization_certificate.hpp
 * @brief SetAuthorizationCertificate IPMI command.
 * */

#pragma once
#include <ipmb/command/command.hpp>
#include "ipmb/certificate_manager.hpp"

#include <vector>

/*! Agent namespace */
namespace agent {
/*! Chassis namespace */
namespace chassis {
/*! IPMB namespace */
namespace ipmb {
/*! Command namespace */
namespace command {

/*! SetAuthorizationCertificate command */
class SetAuthorizationCertificate final : public Command {
public:
    /*! Default constructor */
    SetAuthorizationCertificate(){}
    /*! Copy constructor */
    SetAuthorizationCertificate(const SetAuthorizationCertificate&) = default;
    /*! Assigment construcor */
    SetAuthorizationCertificate& operator=(const SetAuthorizationCertificate&) = default;

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

    ~SetAuthorizationCertificate();

private:
    static const constexpr uint8_t CMD_RESPONSE_DATA_LENGHT = 1;
    static const constexpr uint8_t CMD_REQUEST_HEADER_LENGTH = 5;
    static const constexpr uint8_t CMD_REQUEST_MIN_CHUNK_LENGTH = 1;

    static const constexpr uint8_t OFFSET_TYPE = 0;
    static const constexpr uint8_t OFFSET_CERT_LEN= 1;
    static const constexpr uint8_t OFFSET_CHUNK_NO = 3;
    static const constexpr uint8_t OFFSET_CHUNK_LEN = 4;
    static const constexpr uint8_t OFFSET_CERT = 5;

    class Request final : public Command::Request {
    private:

        Certificate::Type m_certificate_type{};
        uint16_t m_certificate_length{};
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
         * @brief Set certificate type
         * @param type certificate type
         * */
        void set_certificate_type(Certificate::Type type) {
            m_certificate_type = type;
        }

        /*!
         * @brief Get certificate type
         * @return Certificate type
         * */
        Certificate::Type get_certificate_type() const {
            return m_certificate_type;
        }

        /*!
         * Sets certificate length
         * @param length Certificate length
         * */
        void set_certificate_length(const uint16_t length) {
            m_certificate_length = length;
        }

        /*!
         * Gets certificate length
         * @return certificate length
         * */
        uint16_t get_certificate_length() const {
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

    class Response final : public Command::Response {
    public:
        /*!
         * Fill data buffer
         * @param msg IPMI Message
         * */
        void add_data(IpmiMessage& msg);
    };

    Response m_response{};
    Request m_request{};
};

}
}
}
}

