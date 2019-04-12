/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation.
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
 * @file tests/string_utils.cpp
 */

#include "utils/string_utils.hpp"
#include "gtest/gtest.h"

using namespace utils;

namespace testing {

TEST(StringUtilsTests, EmptyString) {
    auto output = StringUtils::substring_after_last_of(std::string{}, '/');
    ASSERT_EQ(output, std::string{});
}

TEST(StringUtilsTests, WithoutSlash) {
    auto output = StringUtils::substring_after_last_of("example string", '/');
    ASSERT_EQ(output, std::string{});
}

TEST(StringUtilsTests, OneSlash) {
    auto output = StringUtils::substring_after_last_of("example/string", '/');
    ASSERT_EQ(output, "string");
}

TEST(StringUtilsTests, MoreSlashes) {
    auto output = StringUtils::substring_after_last_of("it/is/example/string", '/');
    ASSERT_EQ(output, "string");
}

}
