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
 * @file get_authorization_certificate_hash.cpp
 * @brief IPMI command GetAuthorizationCertificateHash.
 * */

#include <ipmb/command/get_authorization_certificate_hash.hpp>
#include <ipmb/certificate_manager.hpp>
#include <ipmb/ipmi_message.hpp>
#include <ipmb/utils.hpp>

#include <logger/logger_factory.hpp>

using namespace agent::chassis::ipmb::command;

GetAuthorizationCertificateHash::~GetAuthorizationCertificateHash() {}

void GetAuthorizationCertificateHash::Response::add_data(IpmiMessage& msg) {
    auto data = msg.get_data();
    auto& hash = get_certificate_hash();
    for (auto i = 0u; i < hash.size(); i++) {
        data[OFFSET_HASH+i] = hash[i];
   }
    data[OFFSET_CC] = msg.get_cc();
    msg.add_len(get_len());
}

void GetAuthorizationCertificateHash::unpack(IpmiMessage& msg){
    log_debug(LOGUSR, "Unpacking Get Certificate Hash message.");
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
    }

    msg.set_to_request();
}

void GetAuthorizationCertificateHash::pack(IpmiMessage& msg){
    log_debug(LOGUSR, "Unpacking Get Certificate Hash message.");
    msg.set_cc(CompletionCode::CC_OK);
    msg.set_to_response();
    try {
        Certificate& cert = CertificateManager::get_instance()->
                            get_certificate(m_request.get_certificate_type());
        m_response.set_certificate_hash(cert.get_hash());
        m_response.set_len(CMD_RESPONSE_DATA_LENGTH);
        m_response.add_data(msg);
    }
    catch (...) {
        msg.set_cc(CompletionCode::CC_REQ_DATA_NOT_PRESENT);
    }

}

