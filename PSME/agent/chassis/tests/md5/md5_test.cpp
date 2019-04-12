/*!
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
 * */

extern "C" {
#include <md5/md5.h>
}

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <vector>

static constexpr const char EMPTY_STRING[] = "";
static constexpr const char SIMPLE_STRING[] = "abc123";
static constexpr const char COMPLEX_STRING[] = "Gr4j:Ap6=kd1+;f";
static constexpr const char EXAMPLE_CERTIFICATE[] =
R"(-----BEGIN CERTIFICATE-----
MIIClTCCAfagAwIBAgIJAI8tOVxuSYNCMAoGCCqGSM49BAMCMGMxCzAJBgNVBAYT
AlBMMRIwEAYDVQQIEwlQb21vcnNraWUxDzANBgNVBAcTBkdkYW5zazEOMAwGA1UE
ChMFSW50ZWwxCzAJBgNVBAsTAklUMRIwEAYDVQQDEwlsb2NhbGhvc3QwHhcNMTUx
MTI0MTIyNzI3WhcNMTYxMTIzMTIyNzI3WjBjMQswCQYDVQQGEwJQTDESMBAGA1UE
CBMJUG9tb3Jza2llMQ8wDQYDVQQHEwZHZGFuc2sxDjAMBgNVBAoTBUludGVsMQsw
CQYDVQQLEwJJVDESMBAGA1UEAxMJbG9jYWxob3N0MIGbMBAGByqGSM49AgEGBSuB
BAAjA4GGAAQAhm2Icz0yK/Rv6F5yzJR2F2d+2YX7g181nnARuk34Q2MY/UJ6Rp+Y
4Qic7otvLNyxFJy1DT2o78oubs7wWbgLjMYBF5zNu6DzlMySpU5LNmbNdOHPELH6
ewRKkYMMnMEzxWRHSYmT7wgpzxk5ZUWg+h0DHiJLZ0xfewVrU657EhPeso+jUDBO
MB0GA1UdDgQWBBRrO+R8NUleDpEDmVfveiAYFDqbRTAfBgNVHSMEGDAWgBRrO+R8
NUleDpEDmVfveiAYFDqbRTAMBgNVHRMEBTADAQH/MAoGCCqGSM49BAMCA4GMADCB
iAJCAQR0ioT8KOGUMM3LAzv8VIOuX3YR5w2FbsY5Smtby5JTHrndCmAd1Lz0eVNB
7vMYInVw7jol0PjzbjaR6uaznOXSAkIBN6zjHpTpai66j9Su+EWeJBS4UBkuMU5v
30lIrFU63PGd8rbaFIN//gDe7LpZLVnmvv8V2HhWPllPrsOOTzo091Q=
-----END CERTIFICATE-----)";

class Md5Test : public ::testing::Test {
protected:
    MD5_CTX context;
    std::vector<std::uint8_t> hash{};


    virtual void SetUp() {
        MD5_Init(&context);
        hash.clear();
        hash.resize(16);
    }

    virtual ~Md5Test();
};

Md5Test::~Md5Test() {}

/* Positive. */

TEST_F(Md5Test, PositiveEmptyStringTest) {
    const std::vector<std::uint8_t> EMPTY_STRING_HASH = {
        0xD4, 0x1D, 0x8C, 0xD9, 0x8F, 0x00, 0xB2, 0x04,
        0xE9, 0x80, 0x09, 0x98, 0xEC, 0xF8, 0x42, 0x7E
    };
    auto data = std::string(EMPTY_STRING);

    MD5_Update(&context, data.data(), data.size());
    MD5_Final(hash.data(), &context);

    ASSERT_EQ(hash, EMPTY_STRING_HASH);
}

TEST_F(Md5Test, PositiveSimpleStringTest) {
    const std::vector<std::uint8_t> SIMPLE_STRING_HASH = {
        0xE9, 0x9A, 0x18, 0xC4, 0x28, 0xCB, 0x38, 0xD5,
        0xF2, 0x60, 0x85, 0x36, 0x78, 0x92, 0x2E, 0x03
    };
    auto data = std::string(SIMPLE_STRING);

    MD5_Update(&context, data.data(), data.size());
    MD5_Final(hash.data(), &context);

    ASSERT_EQ(hash, SIMPLE_STRING_HASH);
}

TEST_F(Md5Test, PositiveComplexStringTest) {
    const std::vector<std::uint8_t> COMPLEX_STRING_HASH = {
        0x9B, 0x60, 0xF5, 0xFD, 0x38, 0x60, 0x66, 0xC6,
        0x5A, 0xF3, 0xA1, 0x92, 0x2F, 0x04, 0xC4, 0xAC
    };
    auto data = std::string(COMPLEX_STRING);

    MD5_Update(&context, data.data(), data.size());
    MD5_Final(hash.data(), &context);

    ASSERT_EQ(hash, COMPLEX_STRING_HASH);
}

TEST_F(Md5Test, PositiveCertificateTest) {
    const std::vector<std::uint8_t> EXAMPLE_CERTIFICATE_HASH = {
        0x84, 0xB4, 0xCF, 0x23, 0x06, 0x61, 0xDE, 0xDB,
        0x34, 0xEC, 0xE1, 0x79, 0x63, 0x25, 0x72, 0x2D
    };
    auto data = std::string(EXAMPLE_CERTIFICATE);

    MD5_Update(&context, data.data(), data.size());
    MD5_Final(hash.data(), &context);

    ASSERT_EQ(hash, EXAMPLE_CERTIFICATE_HASH);
}

/* Negative. */

TEST_F(Md5Test, NegativeCertificateTest) {
    const std::vector<std::uint8_t> RANDOM_HASH = {
        0x81, 0x44, 0xC5, 0x27, 0x08, 0xA1, 0xDB, 0xDB,
        0x32, 0x3C, 0xE6, 0x76, 0x69, 0x05, 0xD2, 0x2E
    };
    auto data = std::string(COMPLEX_STRING);

    MD5_Update(&context, data.data(), data.size());
    MD5_Final(hash.data(), &context);

    ASSERT_NE(hash, RANDOM_HASH);
}
