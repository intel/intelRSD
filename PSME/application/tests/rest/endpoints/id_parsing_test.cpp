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
 * */

#include "psme/rest/endpoints/utils.hpp"
#include "agent-framework/exceptions/exception.hpp"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

class IdToUint64Test : public ::testing::Test {
public:
    ~IdToUint64Test();
};

IdToUint64Test::~IdToUint64Test() {}

TEST_F(IdToUint64Test, AllIdToUint64Tests) {
    using psme::rest::endpoint::utils::id_to_uint64;

    // The regex in the rest server only allows id strings of the form [0-9]+,
    // so there is no need to test strings containing negative numbers, letters
    // or special characters

    std::string positive = "1";
    std::string max_int_32 = "2147483647";
    std::string max_int_32_plus_one = "2147483648";
    std::string max_uint_32 = "4294967295";
    std::string max_uint_32_plus_one = "4294967296";
    std::string max_int_64 = "9223372036854775807";
    std::string max_int_64_plus_one = "9223372036854775808";
    std::string max_uint_64 = "18446744073709551615";
    std::string max_uint_64_plus_one = "18446744073709551616";
    std::string big_string = "429496729518446744073709551616";


    EXPECT_NO_THROW(id_to_uint64(positive););
    uint64_t result_positive = 1u;
    EXPECT_EQ(result_positive, id_to_uint64(positive));

    EXPECT_NO_THROW(id_to_uint64(max_int_32););
    uint64_t result_max_int_32 = 2147483647u;
    EXPECT_EQ(result_max_int_32, id_to_uint64(max_int_32));

    EXPECT_NO_THROW(id_to_uint64(max_int_32_plus_one););
    uint64_t result_max_int_32_plus_one = 2147483648u;
    EXPECT_EQ(result_max_int_32_plus_one, id_to_uint64(max_int_32_plus_one));

    EXPECT_NO_THROW(id_to_uint64(max_uint_32););
    uint64_t result_max_uint_32 = 4294967295u;
    EXPECT_EQ(result_max_uint_32, id_to_uint64(max_uint_32));

    EXPECT_NO_THROW(id_to_uint64(max_uint_32_plus_one););
    uint64_t result_max_uint_32_plus_one = 4294967296u;
    EXPECT_EQ(result_max_uint_32_plus_one, id_to_uint64(max_uint_32_plus_one));

    EXPECT_NO_THROW(id_to_uint64(max_int_64););
    uint64_t result_max_int_64 = 9223372036854775807u;
    EXPECT_EQ(result_max_int_64, id_to_uint64(max_int_64));

    EXPECT_NO_THROW(id_to_uint64(max_int_64_plus_one););
    uint64_t result_max_int_64_plus_one = 9223372036854775808u;
    EXPECT_EQ(result_max_int_64_plus_one, id_to_uint64(max_int_64_plus_one));

    EXPECT_NO_THROW(id_to_uint64(max_uint_64););
    uint64_t result_max_uint_64 = 18446744073709551615u;
    EXPECT_EQ(result_max_uint_64, id_to_uint64(max_uint_64));

    EXPECT_THROW(id_to_uint64(max_uint_64_plus_one), agent_framework::exceptions::NotFound);

    EXPECT_THROW(id_to_uint64(big_string), agent_framework::exceptions::NotFound);
}
