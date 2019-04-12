/*!
 * @brief GetAuthorizedCertHash command unit tests.
 *
 * @copyright Copyright (c) 2015-2019 Intel Corporation.
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
 * @file get_authorized_cert_hash_test.cpp
 */


#include "ipmi/command/sdv/rmm/get_authorized_cert_hash.hpp"
#include <gtest/gtest.h>



using namespace ipmi::command::sdv;

TEST(GetAuthorizedCertificateHashTest, TestRequestPacking) {
    ipmi::IpmiInterface::ByteBuffer podm_cert_request_byte_vector{};
    ipmi::IpmiInterface::ByteBuffer rmm_cert_request_byte_vector{};
    ipmi::IpmiInterface::ByteBuffer expected_podm_cert_request_byte_vector{0x00};
    ipmi::IpmiInterface::ByteBuffer expected_rmm_cert_request_byte_vector{0x01};

    request::GetAuthorizedCertHash get_podm_cert_request{};
    request::GetAuthorizedCertHash get_rmm_cert_request{};

    get_podm_cert_request.set_certificate_type(CertificateType::PODM);
    get_rmm_cert_request.set_certificate_type(CertificateType::RMM);

    podm_cert_request_byte_vector = get_podm_cert_request.do_pack();
    rmm_cert_request_byte_vector = get_rmm_cert_request.do_pack();

    ASSERT_EQ(1, podm_cert_request_byte_vector.size());
    ASSERT_EQ(1, rmm_cert_request_byte_vector.size());

    ASSERT_EQ(podm_cert_request_byte_vector[0], expected_podm_cert_request_byte_vector[0]);
    ASSERT_EQ(rmm_cert_request_byte_vector[0], expected_rmm_cert_request_byte_vector[0]);
}


TEST(GetAuthorizedCertificateHashTest, TestResponseUnpacking) {
    ipmi::IpmiInterface::ByteBuffer byte_vector{0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11,
                                                0x12, 0x13, 0x14, 0x15, 0x16};

    response::GetAuthorizedCertHash response;
    response::GetAuthorizedCertHash::HashContainer unpacked_hash{};
    response::GetAuthorizedCertHash::HashContainer expected_hash{0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                                                                 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16};

    ASSERT_TRUE(response.get_certificate_hash().empty());

    response.do_unpack(byte_vector);

    unpacked_hash = response.get_certificate_hash();

    ASSERT_EQ(expected_hash, unpacked_hash);
}
