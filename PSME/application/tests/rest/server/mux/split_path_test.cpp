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


#include "psme/rest/server/mux/matchers.hpp"

#include "gtest/gtest.h"

namespace psme {
namespace rest {
namespace server {
namespace mux {

using namespace testing;

TEST(SplitPathTest, testRootPath) {
    ASSERT_EQ(0, split_path("").size());
    ASSERT_EQ(split_path(""), split_path("/"));
}

TEST(SplitPathTest, testNonEmptyPaths) {
    ASSERT_EQ(1, split_path("/redfish").size());
    ASSERT_EQ(split_path("/redfish/"), split_path("/redfish"));
    ASSERT_EQ(2, split_path("/redfish/v1").size());
    ASSERT_EQ(split_path("/redfish/v1"), split_path("/redfish/v1/"));
    ASSERT_EQ(3, split_path("/redfish/v1/Systems").size());
    ASSERT_EQ(split_path("/redfish/v1/Systems"), split_path("/redfish/v1/Systems/"));
}

TEST(SplitPathTest, testSlashes) {
    ASSERT_EQ(1, split_path("//").size());
    ASSERT_EQ(2, split_path("/redfish//").size());
}

}
}
}
}
