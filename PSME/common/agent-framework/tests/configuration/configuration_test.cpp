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

#include "../mocks/mock_logger_ext.hpp"

#include "configuration/configuration.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using configuration::Configuration;

using namespace agent_framework::testing;
using ::testing::Return;
using ::testing::Throw;

class ConfigurationTest : public ::testing::Test {
protected:

    std::unique_ptr<logger_cpp::Logger> m_logusrmock{};

    static constexpr char DEFAULT_CONFIGURATION[] = R"({
        "stringValue": "TestStringValue",
        "integerValue": 2015,
        "negativeIntegerValue": -2025,
        "arrayOfStringsValue": [ "TestArrayValue1", "TestArrayValue2", "TestArrayValue3" ],
        "arrayOfNumbersValue": [ 2014, 2013, 2010, 1900 ],
        "objectValue": {
            "objectProperty1": "TestObjectProperty1Value",
            "objectProperty2": 2020
        }
    })";
    static constexpr char STRING_VALUE[] = "stringValue";
    static constexpr char INT_VALUE[] = "integerValue";
    static constexpr char NEGATIVE_INT_VALUE[] = "negativeIntegerValue";
    static constexpr char TEST_STRING_VALUE[] = "TestStringValue";

    virtual void SetUp() {
        m_logusrmock.reset(new MockLogger);
    }

    virtual void TearDown() {
        m_logusrmock.reset(nullptr);
    }

    virtual ~ConfigurationTest();
};

constexpr char ConfigurationTest::DEFAULT_CONFIGURATION[];
constexpr char ConfigurationTest::STRING_VALUE[];
constexpr char ConfigurationTest::INT_VALUE[];
constexpr char ConfigurationTest::NEGATIVE_INT_VALUE[];
constexpr char ConfigurationTest::TEST_STRING_VALUE[];

ConfigurationTest::~ConfigurationTest() {}

/* Positive. */

TEST_F(ConfigurationTest, PositiveConfigurationCreateDefault) {

    EXPECT_NO_THROW(Configuration::get_instance()
            .set_default_configuration(DEFAULT_CONFIGURATION));
    EXPECT_NO_THROW(Configuration::get_instance().to_json());

    Configuration::cleanup();
}

TEST_F(ConfigurationTest, PositiveConfigurationStringValues) {
    Configuration::get_instance().set_default_configuration(DEFAULT_CONFIGURATION);
    const json::Json& configuration = Configuration::get_instance().to_json();
    ASSERT_EQ(configuration.value(STRING_VALUE, json::Json()).is_string(), true);
    ASSERT_EQ(configuration.value(STRING_VALUE, std::string{}), TEST_STRING_VALUE);
}

TEST_F(ConfigurationTest, PositiveConfigurationIntegerValues) {
    Configuration::get_instance().set_default_configuration(DEFAULT_CONFIGURATION);
    const json::Json& configuration = Configuration::get_instance().to_json();
    ASSERT_EQ(configuration.value(INT_VALUE, json::Json()).is_number_unsigned(), true);
    ASSERT_EQ(configuration.value(INT_VALUE, std::uint16_t{}), 2015);
}

TEST_F(ConfigurationTest, PositiveConfigurationNegativeIntegerValues) {
    Configuration::get_instance().set_default_configuration(DEFAULT_CONFIGURATION);
    const json::Json& configuration = Configuration::get_instance().to_json();
    ASSERT_EQ(configuration.value(NEGATIVE_INT_VALUE, json::Json()).is_number_integer(), true);
    ASSERT_EQ(configuration.value(NEGATIVE_INT_VALUE, int{}), -2025);
}


