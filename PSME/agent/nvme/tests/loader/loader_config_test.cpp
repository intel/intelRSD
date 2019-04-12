/*!
 * @brief Implementation of configuration tests.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
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
 * @file loader_config_test.cpp
 */

#include "gtest/gtest.h"

#include "loader/config.hpp"

#include <string>
#include <algorithm>

using namespace agent::nvme::loader;

class NvmeConfigTest: public ::testing::Test {
protected:
    NvmeConfig config{};
    const std::string UUID1{"4083539a-49a9-55b8-9269-11567a5e6305"};
    const std::string UUID2{"5083539a-49a9-55b8-9269-11567a5e6305"};
public:

    virtual ~NvmeConfigTest();
};

NvmeConfigTest::~NvmeConfigTest() { }

TEST_F(NvmeConfigTest, NicDriverName) {
    config.set_nic_drivers("e1000e");
    ASSERT_EQ(config.get_nic_drivers(), "e1000e");
}

TEST_F(NvmeConfigTest, Ipv4) {
    config.set_ipv4_address("127.0.0.1");
    ASSERT_EQ(config.get_ipv4_address(), "127.0.0.1");
}

TEST_F(NvmeConfigTest, LocationOffset) {
    config.set_location_offset(10);
    ASSERT_EQ(config.get_location_offset(), 10);
}

TEST_F(NvmeConfigTest, ParentId) {
    config.set_parent_id("1");
    ASSERT_EQ(config.get_parent_id(), "1");
}

TEST_F(NvmeConfigTest, RdmaPort) {
    config.set_rdma_port(123);
    ASSERT_EQ(config.get_rdma_port(), 123);
}
