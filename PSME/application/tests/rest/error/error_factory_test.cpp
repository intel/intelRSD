/*!
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
 * */

#include "psme/rest/server/status.hpp"
#include "psme/rest/server/error/error_factory.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace psme::rest::error;
using namespace psme::rest::server;

class ErrorFactoryTest : public testing::Test {
public:

    static const constexpr char TEST_MESSAGE[] = "Test message";
    static const constexpr char INVALID_PROPERTY[] = "InvalidProperty";
    static const constexpr char INVALID_PROPERTY_VALUE[] = "SomeValue";
    static const constexpr char ALLOWABLE_VALUES_PROPERTY[] = "Property@AllowableValues";

    void SetUp() { }

    void TearDown() { }

    ~ErrorFactoryTest();
};

ErrorFactoryTest::~ErrorFactoryTest() { }

const constexpr char ErrorFactoryTest::TEST_MESSAGE[];
const constexpr char ErrorFactoryTest::INVALID_PROPERTY[];
const constexpr char ErrorFactoryTest::INVALID_PROPERTY_VALUE[];
const constexpr char ErrorFactoryTest::ALLOWABLE_VALUES_PROPERTY[];

TEST_F(ErrorFactoryTest, NotAllowedErrorPositive) {
    const auto& error = ErrorFactory::create_error_for_not_allowable_value(TEST_MESSAGE,
                                                                           INVALID_PROPERTY,
                                                                           INVALID_PROPERTY_VALUE,
                                                                           ALLOWABLE_VALUES_PROPERTY);

    ASSERT_EQ(error.has_extended_messages(), true);
    ASSERT_EQ(error.get_http_status_code(), status_4XX::BAD_REQUEST);
    ASSERT_EQ(error.get_code(), ServerError::PROPERTY_VALUE_NOT_IN_LIST);
}
