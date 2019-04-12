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
 * @file get_authorization_certificate.cpp
 * @brief IPMI command GetAuthorizationCertificate.
 * */

#include <ipmb/command/get_authorization_certificate.hpp>
#include <ipmb/certificate_manager.hpp>
#include <ipmb/ipmi_message.hpp>
#include <ipmb/utils.hpp>

#include <logger/logger_factory.hpp>

using namespace agent::chassis::ipmb::command;

GetAuthorizationCertificate::~GetAuthorizationCertificate() {}

void GetAuthorizationCertificate::Response::add_data(IpmiMessage& msg) {
    auto data = msg.get_data();
    data[OFFSET_CC] = msg.get_cc();
    data[OFFSET_CERT_LEN] = static_cast<uint8_t>(get_certificate_length() & 0xff);
    data[OFFSET_CERT_LEN+1] = static_cast<uint8_t>(get_certificate_length() >> 8 & 0xff);
    data[OFFSET_CHUNK_NO] = get_current_chunk_number();
    data[OFFSET_CRNT_CHUNK_LEN] = get_current_chunk_length();
    auto chunk = get_certificate_data();
    for (unsigned int i = 0; i < chunk.size(); i++) {
        data[OFFSET_CERT+i] = chunk[i];
    }
    msg.add_len(get_len());
}

void GetAuthorizationCertificate::unpack(IpmiMessage& msg) {
    log_debug(LOGUSR, "Unpacking Get Authorization Certificate message.");
    auto data = msg.get_data();
    if (CMD_REQUEST_DATA_LENGTH >
            (msg.get_len() - IPMB_FRAME_HDR_WITH_DATA_CHCKSUM_LEN)) {
        log_error("ipmb", "Invalid request length.");
        m_response.set_cc(CompletionCode::CC_REQ_DATA_LENGTH_INVALID);
        return;
    }
    if (CMD_REQUEST_DATA_LENGTH <
            (msg.get_len() - IPMB_FRAME_HDR_WITH_DATA_CHCKSUM_LEN)) {
        log_error("ipmb", "Request is too long: " <<
                        unsigned(msg.get_len()));
        m_response.set_cc(CompletionCode::CC_REQ_DATA_FIELD_LENGTH_EXC);
        return;
    }
    if (data) {
        m_request.set_certificate_type(Certificate::
                                    Type(data[REQ_OFFSET_CERT_TYPE]));
        m_request.set_current_chunk_number(data[REQ_OFFSET_CHUNK]);
    }

    msg.set_to_request();
}

void GetAuthorizationCertificate::pack(IpmiMessage& msg) {
    log_debug(LOGUSR, "Packing Get Authorization Certificate message.");

    msg.set_cc(CompletionCode::CC_OK);
    msg.set_to_response();
    try {
        const auto& cert = CertificateManager::get_instance()->
                    get_certificate((m_request.get_certificate_type()));
        const auto& chunk = cert.get_chunk(m_request.get_current_chunk_number());

        m_response.set_certificate_length(cert.get_length());
        m_response.set_current_chunk_number(m_request.
                    get_current_chunk_number());
        m_response.set_current_chunk_length(static_cast<uint8_t>(chunk.size()));
        m_response.set_certificate_data(chunk);
        m_response.set_len(uint16_t(CMD_MIN_RESPONSE_DATA_LENGTH +
                m_response.get_current_chunk_length()));

        m_response.add_data(msg);
    }
    catch (...) {
        msg.set_cc(CompletionCode::CC_REQ_DATA_NOT_PRESENT);
    }
}
