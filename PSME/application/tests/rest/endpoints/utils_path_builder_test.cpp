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
 * */


#include "psme/rest/endpoints/path_builder.hpp"
#include "psme/rest/server/request.hpp"

#include "gtest/gtest.h"

namespace psme {
namespace rest {
namespace endpoint {

using namespace testing;
using namespace psme::rest::constants;
using server::Request;

TEST(PathBuilderTest, testEmptySegment) {
    ASSERT_EQ("", PathBuilder().build());
    ASSERT_EQ("", PathBuilder("").build());
}

TEST(PathBuilderTest, testRoot) {
    ASSERT_EQ("/", PathBuilder().append("").build());
    ASSERT_EQ("//", PathBuilder().append("/").build());
}

TEST(PathBuilderTest, testConstructorWithNumber) {
    ASSERT_EQ("123456789012", PathBuilder(std::uint64_t(123456789012)).build());
    ASSERT_EQ("123456789", PathBuilder(std::uint32_t(123456789)).build());
}

TEST(PathBuilderTest, testConstructorWithString) {
    ASSERT_EQ("/redfish/v1", PathBuilder("/redfish/v1").build());
}

TEST(PathBuilderTest, testConstructorWithRequestSlashOnly) {
    Request req{};
    req.set_destination("/");
    ASSERT_EQ("/", PathBuilder(req).build());
}

TEST(PathBuilderTest, testConstructorWithRequestHavingSlashAtEnd) {
    Request req{};
    req.set_destination("/redfish/v1/");
    ASSERT_EQ("/redfish/v1", PathBuilder(req).build());
}

TEST(PathBuilderTest, testConstructorWithRequestWithoutSlashAtEnd) {
    Request req{};
    req.set_destination("/redfish/v1");
    ASSERT_EQ("/redfish/v1", PathBuilder(req).build());
}

TEST(PathBuilderTest, testStaticSegments) {
    ASSERT_EQ("/redfish/v1/Drawers", PathBuilder("/redfish/v1/Drawers").build());
    ASSERT_EQ("/redfish/v1/Drawers", PathBuilder().append("redfish/v1/Drawers").build());
    ASSERT_EQ("/redfish/v1/Drawers", PathBuilder("/redfish/v1").append("Drawers").build());
    ASSERT_EQ("/redfish/v1/Drawers", PathBuilder().append("redfish").append("v1").append("Drawers").build());
    ASSERT_EQ("/redfish/v1/Drawers", PathBuilder("/redfish").append("v1").append("Drawers").build());
}

TEST(PathBuilderTest, testSlashNotTrimmed) {
    ASSERT_EQ("//", PathBuilder().append("/").build());
    ASSERT_EQ("/redfish///v1//", PathBuilder().append("redfish//").append("v1//").build());
    ASSERT_EQ("/redfish////v1//", PathBuilder().append("redfish//").append("/v1//").build());
}

TEST(PathBuilderTest, testRegexSegment) {
    ASSERT_EQ("/redfish/v1/Chassis/{chassisId:[0-9]+}", PathBuilder("/redfish/v1")
                                                .append("Chassis")
                                                .append_regex("chassisId", "[0-9]+")
                                                .build());
}

TEST(PathBuilderTest, testVariableSegment) {
    ASSERT_EQ("/redfish/v1/Chassis/{chassisId}", PathBuilder("/redfish/v1")
                                                .append("Chassis")
                                                .append_variable("chassisId")
                                                .build());
}

TEST(PathBuilderTest, testNumericSegments) {
    ASSERT_EQ("/Chassis/123456789012",
        PathBuilder().append("Chassis").append(std::uint64_t(123456789012)).build());
    ASSERT_EQ("/Chassis/123456789",
        PathBuilder().append("Chassis").append(std::uint32_t(123456789)).build());
}

}
}
}
