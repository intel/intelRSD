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
 * @file set_authorization_certificate.cpp
 * @brief IPMI command GetAuthorizationCertificate.
 * */

#include <ipmb/command/set_authorization_certificate.hpp>
#include <ipmb/ipmi_message.hpp>
#include <ipmb/utils.hpp>
#include <ipmb/certificate_manager.hpp>

#include <logger/logger_factory.hpp>
#include <algorithm>

using namespace agent::chassis::ipmb::command;

SetAuthorizationCertificate::~SetAuthorizationCertificate() {}

void SetAuthorizationCertificate::Response::add_data(IpmiMessage& msg) {
    auto data = msg.get_data();
    data[0] = uint8_t(this->get_cc());
    msg.add_len(get_len());
}

void SetAuthorizationCertificate::unpack(IpmiMessage& msg){
    log_debug(LOGUSR, "Unpacking Set Authorization Certificate message.");
    msg.set_to_request();

    // verify if the request is long enough
    if (CMD_REQUEST_HEADER_LENGTH + CMD_REQUEST_MIN_CHUNK_LENGTH >
                (msg.get_len() - IPMB_FRAME_HDR_WITH_DATA_CHCKSUM_LEN)) {
        log_error("ipmb", "Invalid request length.");
        m_response.set_cc(CompletionCode::CC_REQ_DATA_LENGTH_INVALID);
        return;
    }

    auto data = msg.get_data();
    if (data) {
        log_debug("ipmb", "Validating length.");
        // verify if request's length is compliant with declared
        if ((CMD_REQUEST_HEADER_LENGTH + data[OFFSET_CHUNK_LEN]) <
                (msg.get_len() - IPMB_FRAME_HDR_WITH_DATA_CHCKSUM_LEN)) {
            log_error("ipmb", "Request is too long: " <<
                    unsigned(msg.get_len()));
            m_response.set_cc(CompletionCode::CC_REQ_DATA_FIELD_LENGTH_EXC);
            return;
        }

        if ((CMD_REQUEST_HEADER_LENGTH + data[OFFSET_CHUNK_LEN]) >
                (msg.get_len() - IPMB_FRAME_HDR_WITH_DATA_CHCKSUM_LEN)) {
            log_error("ipmb", "Request is too short: " <<
                    unsigned(msg.get_len()));
            m_response.set_cc(CompletionCode::CC_REQ_DATA_FIELD_LENGTH_EXC);
            return;
        }

        m_request.set_certificate_type(Certificate::Type(data[OFFSET_TYPE]));
        m_request.set_certificate_length(uint16_t((data[OFFSET_CERT_LEN]) | data[OFFSET_CERT_LEN+1] << 8));
        m_request.set_current_chunk_number(data[OFFSET_CHUNK_NO]);
        m_request.set_current_chunk_length(data[OFFSET_CHUNK_LEN]);
        std::copy_n(data+OFFSET_CERT, m_request.get_current_chunk_length(),
                    std::back_inserter(m_request.get_certificate_data()));
    }
}

void SetAuthorizationCertificate::pack(IpmiMessage& msg){
    log_debug(LOGUSR, "Packing Set Authorization Certificate message.");

    if(CompletionCode::CC_OK != m_response.get_cc()) {
        return;
    }

    msg.set_cc(CompletionCode::CC_OK);
    msg.set_to_response();

    Certificate& cert = CertificateManager::get_instance()->
                    get_certificate((m_request.get_certificate_type()));
    cert.add_chunk(m_request.get_certificate_length(),
                   m_request.get_current_chunk_number(),
                   m_request.get_certificate_data());

    m_response.set_len(CMD_RESPONSE_DATA_LENGHT);
    m_response.add_data(msg);
}
