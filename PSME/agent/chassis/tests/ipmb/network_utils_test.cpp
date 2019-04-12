/*!
 * @brief Tests for parsing systemd/network files
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file network_utils_test.cpp
 */


#include "ipmb/network_utils.hpp"

#include <gtest/gtest.h>

#include <sstream>
#include <iostream>

using namespace agent::chassis::ipmb::network_utils;
using namespace std;

class NetworkUtilsTest: public ::testing::Test {
protected:
    NetworkUtilsTest() {}
    virtual ~NetworkUtilsTest();
};

NetworkUtilsTest::~NetworkUtilsTest() { }

TEST_F(NetworkUtilsTest, parse_flat) {
    /* config is streamed out in alphabetical order! */
    std::stringstream config{
        "[Match]\n"
        "Name=enp0s20f0.4094\n"
        "\n"
        "[Network]\n"
        "Address=1.2.3.4/24\n"
        "DHCP=no\n"
    };
    json::Json parsed = parse_ini(config);

    std::stringstream saved{};
    save_ini(saved, parsed);
    ASSERT_EQ(config.str(), saved.str());
}

TEST_F(NetworkUtilsTest, parse_shifted) {
    std::stringstream config{R"(
        [Match]
        Name=enp0s20f0.4094
        [Network]
        DHCP=no
        Address=1.2.3.4/24
    )"};

    json::Json parsed = parse_ini(config);
    ASSERT_EQ(parsed.count("Match"), 1);
    ASSERT_EQ(parsed.count("Name"), 0);
    ASSERT_EQ(parsed["Match"].count("Name"), 1);

    ASSERT_EQ(parsed.count("Missing"), 0);
    ASSERT_EQ(parsed["Missing"].count("MissingValue"), 0);
}

TEST_F(NetworkUtilsTest, move_address) {
    std::stringstream config{R"(
        [Match]
        Name=enp0s20f0.4094
        [Network]
        DHCP=no
        Address=1.2.3.4/24
    )"};

    std::string expected{
        "[Address]\n"
        "Address=1.2.3.4/24\n"
        "\n"
        "[Match]\n"
        "Name=enp0s20f0.4094\n"
        "\n"
        "[Network]\n"
        "DHCP=no\n"
    };

    json::Json parsed = parse_ini(config);
    parsed["Address"]["Address"] = std::move(parsed["Network"]["Address"]);
    parsed["Network"].erase("Address");

    std::stringstream saved{};
    save_ini(saved, parsed);
    ASSERT_EQ(saved.str(), expected);
}
