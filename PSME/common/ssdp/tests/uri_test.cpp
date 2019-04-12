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

#include "ssdp/uri.hpp"
#include "gtest/gtest.h"

using namespace ssdp;

TEST(UriTest, UriConstructor) {
    auto uri_str = std::string("http://localhost:8888/redfish/v1");

    URI uri = URI(uri_str);

    EXPECT_EQ(uri_str, uri.to_string());
}

TEST(UriTest, ReplaceHostUriPart) {
    auto uri_str = std::string("http://localhost:8888/redfish/v1");

    URI uri = URI(uri_str);
    uri.set_host("192.168.0.1");
    EXPECT_EQ("http://192.168.0.1:8888/redfish/v1", uri.to_string());
}
