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

#include "configuration/validators/url.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "json-wrapper/json-wrapper.hpp"

using namespace configuration;
using namespace json;

using ::testing::Return;
using ::testing::Throw;

class UrlValidatorTest : public ::testing::Test {
protected:
    static constexpr const char URL_VALUE[] = "TestUrlValue";
    static constexpr const char VALID_HTTPS_VALUE[] = "https://testurl";
    static constexpr const char VALID_HTTP_VALUE[] = "http://testurl.test/abc/#";
    static constexpr const char VALID_FTP_VALUE[] = "ftp://user:password@host:101/path";
    static constexpr const char INVALID_URL_VALUE[] = "InvalidUrlValue";
    static constexpr const int INVALID_TYPE_VALUE = 5;

    virtual ~UrlValidatorTest();
};

constexpr const char UrlValidatorTest::URL_VALUE[];
constexpr const char UrlValidatorTest::VALID_HTTPS_VALUE[];
constexpr const char UrlValidatorTest::VALID_HTTP_VALUE[];
constexpr const char UrlValidatorTest::VALID_FTP_VALUE[];
constexpr const char UrlValidatorTest::INVALID_URL_VALUE[];
constexpr const int UrlValidatorTest::INVALID_TYPE_VALUE;

UrlValidatorTest::~UrlValidatorTest() {}

/* Positive. */

TEST_F(UrlValidatorTest, PositiveValidHttpsUrlTest) {
    std::unique_ptr<configuration::UrlValidator> url_validator(new configuration::UrlValidator(URL_VALUE));
    json::Json json_val(VALID_HTTPS_VALUE);
    ASSERT_EQ(url_validator->is_valid(json_val), true);
}

TEST_F(UrlValidatorTest, PositiveValidHttpUrlTest) {
    std::unique_ptr<configuration::UrlValidator> url_validator(new configuration::UrlValidator(URL_VALUE));
    json::Json json_val(VALID_HTTP_VALUE);
    ASSERT_EQ(url_validator->is_valid(json_val), true);
}

TEST_F(UrlValidatorTest, PositiveValidFtpUrlTest) {
    std::unique_ptr<configuration::UrlValidator> url_validator(new configuration::UrlValidator(URL_VALUE));
    json::Json json_val(VALID_FTP_VALUE);
    ASSERT_EQ(url_validator->is_valid(json_val), true);
}

/* Negative. */

TEST_F(UrlValidatorTest, NegativeInvalidTypeTest) {
    std::unique_ptr<configuration::UrlValidator> url_validator(new configuration::UrlValidator(URL_VALUE));
    json::Json json_val(INVALID_TYPE_VALUE);
    ASSERT_EQ(url_validator->is_valid(json_val), false);
}

TEST_F(UrlValidatorTest, NegativeInvalidUrlValueTest) {
    std::unique_ptr<configuration::UrlValidator> url_validator(new configuration::UrlValidator(URL_VALUE));
    json::Json json_val(INVALID_URL_VALUE);
    ASSERT_EQ(url_validator->is_valid(json_val), false);
}
