/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * */


#include "ipmb/certificate.hpp"
#include "agent-framework/exceptions/invalid_value.hpp"

#include <gtest/gtest.h>
#include <vector>

using namespace agent::chassis::ipmb;
using namespace std;

class ChassisIpmbCertTest: public ::testing::Test {
protected:
    ChassisIpmbCertTest() {}
    virtual ~ChassisIpmbCertTest();
};

ChassisIpmbCertTest::~ChassisIpmbCertTest() { }

TEST_F(ChassisIpmbCertTest, add_chunk_pass) {
    const vector<uint8_t> data{1,2,3,4,5,6,7};
    size_t length{7};
    Certificate cert{};
    cert.add_chunk(length, 0, data);
    ASSERT_EQ(length, cert.get_length());
    ASSERT_EQ(data, cert.get_chunk(0));
    ASSERT_TRUE(cert.is_complete());
}

TEST_F(ChassisIpmbCertTest, chunk_not_found) {
    const vector<uint8_t> data{1,2,3,4,5,6,7};
    size_t length{7};
    Certificate cert{};
    cert.add_chunk(length, 0, data);
    ASSERT_THROW(cert.get_chunk(1), std::runtime_error);
}

TEST_F(ChassisIpmbCertTest, get_hash) {
    const vector<uint8_t> data{'a','b','c','d'};
    const vector<uint8_t> hash{0xe2,0xfc,0x71,0x4c,0x47,0x27,0xee,0x93,0x95,0xf3,0x24,0xcd,0x2e,0x7f,0x33,0x1f};
    size_t length{4};
    Certificate cert{};
    cert.add_chunk(length, 0, data);
    ASSERT_EQ(cert.get_hash(), hash);
}

TEST_F(ChassisIpmbCertTest, get_data) {
    const vector<uint8_t> data{'a','b','c','d'};
    size_t length{4};
    Certificate cert{};
    cert.add_chunk(length, 0, data);
    ASSERT_EQ(cert.get_data(), data);
}

TEST_F(ChassisIpmbCertTest, from_string) {
    Certificate cert{};
    ASSERT_EQ(cert.from_string("PODM"), Certificate::Type::PODM);
    ASSERT_EQ(cert.from_string("RMM"), Certificate::Type::RMM);
    ASSERT_THROW(cert.from_string(""), agent_framework::exceptions::InvalidValue);
    ASSERT_THROW(cert.from_string("x"), agent_framework::exceptions::InvalidValue);
}
