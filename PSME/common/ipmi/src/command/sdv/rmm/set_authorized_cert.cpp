/*!
 * @brief SetAuthorizedCertHash command implementation.
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
 * @file set_authorized_cert.cpp
 */

#include "ipmi/command/sdv/rmm/set_authorized_cert.hpp"

#include <algorithm>
#include <iterator>



using namespace ipmi;
using namespace ipmi::command::sdv;


request::SetAuthorizedCert::SetAuthorizedCert() : ipmi::Request(NetFn::INTEL, Cmd::SET_AUTHORIZED_CERT) {}


request::SetAuthorizedCert::~SetAuthorizedCert() {}


void request::SetAuthorizedCert::pack(IpmiInterface::ByteBuffer& data) const {
    std::uint8_t certificate_length_lsb = static_cast<std::uint8_t>(m_certificate_length & (~0x00));
    std::uint8_t certificate_length_msb = static_cast<std::uint8_t>((m_certificate_length >> 8) & (~0x00));

    data.push_back(static_cast<std::uint8_t>(m_certificate_type));
    data.push_back(certificate_length_lsb);
    data.push_back(certificate_length_msb);
    data.push_back(m_current_chunk_number);
    data.push_back(m_current_chunk_length);

    std::copy(m_certificate_chunk.begin(), m_certificate_chunk.end(), std::back_inserter(data));
}


response::SetAuthorizedCert::SetAuthorizedCert() : ipmi::Response(NetFn::INTEL + 1, Cmd::SET_AUTHORIZED_CERT,
                                                                  RESPONSE_SIZE) {}


response::SetAuthorizedCert::~SetAuthorizedCert() {}
