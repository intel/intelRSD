/*!
 * @brief Unit tests for CertificateData container class.
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
 * @file certificate_data_test.cpp
 */

#include "certificate_management/certificate_data.hpp"

#include "gtest/gtest.h"



using namespace agent::rmm;

namespace constants {
    constexpr CertificateData::CertificateType CERT_TYPE = CertificateData::CertificateType::PODM;
    constexpr char CERT_FILE_PATH[] = "/etc/psme/certs/podm_cert.crt";
    const CertificateData::CertificateHashType CERT_HASH{0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                                                         0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0e, 0x0f};
    const CertificateData::CertificateDataType CERT_DATA{0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01};
}

TEST(CertificateDataTest, TestSettersAndGetters) {
    CertificateData cert_data{constants::CERT_TYPE};
    cert_data.set_certificate_file_path(constants::CERT_FILE_PATH).set_certificate_hash(constants::CERT_HASH).set_certificate_data(constants::CERT_DATA);

    ASSERT_EQ(constants::CERT_TYPE, cert_data.get_certificate_type());
    ASSERT_EQ(constants::CERT_FILE_PATH, cert_data.get_certificate_file_path());
    ASSERT_EQ(constants::CERT_HASH, cert_data.get_certificate_hash());
    ASSERT_EQ(constants::CERT_DATA, cert_data.get_certificate_data());
}
