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
 * @brief Main entry for all tests
 *
 * Initialize Google C++ Mock and Google C++ Testing Framework
 * Do general cleanup after tests like delete resources from singletons
 * */

#include "gtest/gtest.h"
#include "ipmb/network_utils.hpp"

using namespace std;
using namespace agent::chassis::ipmb::network_utils;

static constexpr const std::size_t IP_BYTE_COUNT = 4;

TEST(properIpAddressToBytes, testAddrToBytes) {
    string ip = "10.20.30.40";

    auto byte_vector = addr_to_bytes(ip);

    ASSERT_EQ(IP_BYTE_COUNT, byte_vector.size()) << "Proper IP address should be converted to vector of 4 bytes.";
    ASSERT_EQ(10, byte_vector[0]) << "First byte should be 10.";
    ASSERT_EQ(20, byte_vector[1]) << "Second byte should be 20.";
    ASSERT_EQ(30, byte_vector[2]) << "Third byte should be 30.";
    ASSERT_EQ(40, byte_vector[3]) << "Fourth byte should be 40.,";
}

TEST(emptyStringToBytes, testAddrToBytes) {
    string ip = "";

    try {
        addr_to_bytes(ip);
        FAIL() << "Empty string as an argument should cause throw of invalid_argument exception.";
    } catch (const invalid_argument&) {
        SUCCEED();
    } catch (...) {
        FAIL() << "Invalid exception has been thrown. Should throw invalid_argument";
    }
}

TEST(malformedIpWithTwoDotsToBytes, testAddrToBytes) {
    string ip = "10.20.3040";

    try {
        addr_to_bytes(ip);
        FAIL() << "String with malfored IP as an argument should cause throw of invalid_argument exception.";
    } catch (const invalid_argument&) {
        SUCCEED();
    } catch (...) {
        FAIL() << "Invalid exception has been thrown. Should throw invalid_argument";
    }
}

TEST(malformedIpWithOneDotToBytes, testAddrToBytes) {
    string ip = "10.203040";

    try {
        addr_to_bytes(ip);
        FAIL() << "String with malfored IP as an argument should cause throw of invalid_argument exception.";
    } catch (const invalid_argument&) {
        SUCCEED();
    } catch (...) {
        FAIL() << "Invalid exception has been thrown. Should throw invalid_argument";
    }
}

TEST(malformedIpWithoutDotsToBytes, testAddrToBytes) {
    string ip = "10203040";

    try {
        addr_to_bytes(ip);
        FAIL() << "String with malformed IP as an argument should cause throw of invalid_argument exception.";
    } catch (const invalid_argument&) {
        SUCCEED();
    } catch (...) {
        FAIL() << "Invalid exception has been thrown. Should throw invalid_argument";
    }
}

TEST(lettersInsteadOfIpToBytes, testAddrToBytes) {
    string ip = "a.b.c.d";

    try {
        addr_to_bytes(ip);
        FAIL() << "String with malformed IP as an argument should cause throw of invalid_argument exception.";
    } catch (const invalid_argument&) {
        SUCCEED();
    } catch (...) {
        FAIL() << "Invalid exception has been thrown. Should throw invalid_argument";
    }
}

TEST(BadIpAddressToBytes, testAddrToBytes) {
    string ip = "256.256.256.256";

    try {
        addr_to_bytes(ip);
        FAIL() << "String with bad IP address as an argument should cause throw of invalid_argument exception.";
    } catch (const invalid_argument&) {
        SUCCEED();
    } catch (...) {
        FAIL() << "Invalid exception has been thrown. Should throw invalid_argument";
    }
}
