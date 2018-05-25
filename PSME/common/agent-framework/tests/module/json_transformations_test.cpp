/*!
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation.
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
 * @header{Filesystem}
 * @file json_transformations_test.cpp
 */

#include "json-wrapper/json-wrapper.hpp"
#include <json/json.hpp>
#include "gtest/gtest.h"
#include "agent-framework/module/utils/json_transformations.hpp"

using namespace agent_framework::model::utils;

static std::vector<std::string> test_cases{
    R"(9223372036854775807)",
    R"(2147483648)",
    R"(2147483647)",
    R"(1)",
    R"(-1)",
    R"(-2147483648)",
    R"(-2147483649)",
    R"(-9223372036854775808)",
    R"("sample string")",
    R"(3.14159)",
    R"(-3.14159)",
    R"(true)",
    R"(false)",
    R"([1,2,3,4,5])",
    R"(["one","two","three"])",
    R"({"very":{"nested":{"object":[1.4,true,"nop"]}}})",
};


// Value-parametrized test: JsonTransformationsTest is instantiated for every value in container "test_cases".
// Current parameter for the test is available from GetParam() method, as seen in RoundTripTransformationTest.

class JsonTransformationsTest : public ::testing::Test, public ::testing::WithParamInterface<std::string> {
public:
    virtual ~JsonTransformationsTest();
};

JsonTransformationsTest::~JsonTransformationsTest() {}

TEST_P(JsonTransformationsTest, RoundTripTransformationTest) {
    const auto test_case = GetParam();
    json::Json original_value;
    try {
        original_value = json::Json::parse(test_case);
    }
    catch (...) {
        ASSERT_FALSE(true) << "Cannot parse JSON";
    }

    json::Value middle_value = to_json_cxx(original_value);
    json::Json final_value = to_json_rpc(middle_value);
    ASSERT_EQ(original_value, final_value);
}

INSTANTIATE_TEST_CASE_P(Default, JsonTransformationsTest, ::testing::ValuesIn(test_cases));
