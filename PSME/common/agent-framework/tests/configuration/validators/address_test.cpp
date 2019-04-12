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
 * */

#include <cstdio>

#include "configuration/validators/address.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "json-wrapper/json-wrapper.hpp"

using namespace configuration;
using namespace json;

using ::testing::Return;
using ::testing::Throw;

class AddressValidatorTest : public ::testing::Test {
protected:
    static constexpr const char ADDRESS_VALUE[] = "TestAddressValue";
    static constexpr const char VALID_STRING_VALUE[] = "localhost";
    static constexpr const char VALID_ADDRESS_VALUE[] = "127.0.0.1";
    static constexpr const int INVALID_TYPE_VALUE = 5;

    virtual ~AddressValidatorTest();
};

constexpr const char AddressValidatorTest::ADDRESS_VALUE[];
constexpr const char AddressValidatorTest::VALID_STRING_VALUE[];
constexpr const char AddressValidatorTest::VALID_ADDRESS_VALUE[];
constexpr const int AddressValidatorTest::INVALID_TYPE_VALUE;

AddressValidatorTest::~AddressValidatorTest() {}

/* Positive. */

TEST_F(AddressValidatorTest, PositiveValidStringValueTest) {
    std::unique_ptr<configuration::AddressValidator> address_validator(new configuration::AddressValidator(ADDRESS_VALUE));
    json::Json json_val = VALID_STRING_VALUE;
    ASSERT_EQ(address_validator->is_valid(json_val), true);
}

TEST_F(AddressValidatorTest, PositiveValidAddressTest) {
    std::unique_ptr<configuration::AddressValidator> address_validator(new configuration::AddressValidator(ADDRESS_VALUE));
    json::Json json_val = VALID_ADDRESS_VALUE;
    ASSERT_EQ(address_validator->is_valid(json_val), true);
}

/* Negative. */

TEST_F(AddressValidatorTest, NegativeInvalidTypeTest) {
    std::unique_ptr<configuration::AddressValidator> address_validator(new configuration::AddressValidator(ADDRESS_VALUE));
    json::Json json_val = INVALID_TYPE_VALUE;
    ASSERT_EQ(address_validator->is_valid(json_val), false);
}

TEST_F(AddressValidatorTest, NegativeInvalidAddressTest) {
    std::unique_ptr<configuration::AddressValidator> address_validator(new configuration::AddressValidator(ADDRESS_VALUE));
    json::Json json_val = INVALID_TYPE_VALUE;
    ASSERT_EQ(address_validator->is_valid(json_val), false);
}


