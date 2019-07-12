/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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

#include <agent-framework/version.hpp>
#include "gtest/gtest.h"


using agent_framework::generic::Version;

namespace {
const char* version_string = "2019.13";
}

TEST(VersionTest, CorrectMajorComponent) {
    ASSERT_EQ(2019, agent_framework::generic::get_version_component(version_string, Version::MAJOR_INDEX));
}

TEST(VersionTest, CorrectBuildComponent) {
    ASSERT_EQ(13, agent_framework::generic::get_version_component(version_string, Version::BUILD_INDEX));
}

TEST(VersionTest, CorrectMinorComponent) {
    ASSERT_EQ(0, agent_framework::generic::get_version_component(version_string, Version::MINOR_INDEX));
}

TEST(VersionTest, CorrectHotfixComponent) {
    ASSERT_EQ(0, agent_framework::generic::get_version_component(version_string, 4));
}

TEST(VersionTest, InvalidComponentIndex) {
    ASSERT_EQ(0, agent_framework::generic::get_version_component(version_string, -1));
    ASSERT_EQ(0, agent_framework::generic::get_version_component(version_string, 5));
}

TEST(VersionTest, NonNumericVersion) {
    ASSERT_EQ(0, agent_framework::generic::get_version_component("NotANumber", 0));
    ASSERT_EQ(2, agent_framework::generic::get_version_component("2.NotANumber", 0));
    ASSERT_EQ(0, agent_framework::generic::get_version_component("2.NotANumber", 1));
    ASSERT_EQ(0, agent_framework::generic::get_version_component("2.10NotANumber", 1));
}

TEST(VersionTest, NullVersion) {
    ASSERT_EQ(0, agent_framework::generic::get_version_component(nullptr, 0));
}
