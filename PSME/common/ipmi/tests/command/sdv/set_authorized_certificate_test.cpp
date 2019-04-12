/*!
 * @brief SetAuthorizedCertHash command unit tests.
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
 * @file set_authorized_certificate_test.cpp
 */

#include "ipmi/command/sdv/rmm/set_authorized_cert.hpp"
#include <gtest/gtest.h>



using namespace ipmi;
using namespace ipmi::command::sdv;

TEST(SetAuthorizedCertificateHashTest, TestRequestPacking) {
    request::SetAuthorizedCert request{};

    CertificateType certificate_type{CertificateType::PODM};
    std::uint16_t cert_size{300};
    std::uint8_t current_chunk_number{2};
    request::SetAuthorizedCert::CertificateChunk certificate_chunk{0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};

    ipmi::IpmiInterface::ByteBuffer request_byte_vector{};
    ipmi::IpmiInterface::ByteBuffer expected_request_byte_vector{0x00, 0x2c, 0x01, 0x02, 0x08, 0x01, 0x02, 0x03, 0x04,
                                                                 0x05, 0x06, 0x07, 0x08};

    request.set_certificate_type(certificate_type);
    request.set_certificate_length(cert_size);
    request.set_current_chunk_number(current_chunk_number);
    request.set_certificate_chunk(certificate_chunk);

    request_byte_vector = request.do_pack();

    ASSERT_EQ(expected_request_byte_vector, request_byte_vector);
}
