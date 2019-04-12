/*!
 * @brief GetAuthorizedCertHash command implementation.
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
 * @file get_authorized_cert_hash.cpp
 */


#include "ipmi/command/sdv/rmm/get_authorized_cert_hash.hpp"

#include <algorithm>



using namespace ipmi;
using namespace ipmi::command::sdv;


request::GetAuthorizedCertHash::GetAuthorizedCertHash() : Request(NetFn::INTEL, Cmd::GET_AUTHORIZED_CERT_HASH) {}


request::GetAuthorizedCertHash::~GetAuthorizedCertHash() {}


void request::GetAuthorizedCertHash::pack(IpmiInterface::ByteBuffer& data) const {
    data.push_back(static_cast<std::uint8_t>(m_certificate_type));
}


response::GetAuthorizedCertHash::GetAuthorizedCertHash() : Response(NetFn::INTEL + 1, Cmd::GET_AUTHORIZED_CERT_HASH,
                                                                    RESPONSE_SIZE) {
    m_certificate_hash.reserve(MD5_HASH_LENGTH);
}


response::GetAuthorizedCertHash::~GetAuthorizedCertHash() {}


void response::GetAuthorizedCertHash::unpack(const IpmiInterface::ByteBuffer& data) {
    m_certificate_hash.resize(MD5_HASH_LENGTH);
    std::copy_n(data.begin() + OFFSET_DATA, MD5_HASH_LENGTH, m_certificate_hash.begin());
}
