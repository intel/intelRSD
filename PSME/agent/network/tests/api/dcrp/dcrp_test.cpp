/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016-2017 Intel Corporation
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


#include "gtest/gtest.h"

#include "api/dcrp/remote_switch_info.hpp"
#include "agent-framework/module/model/remote_ethernet_switch.hpp"

#include "mock_sockets.hpp"

#include <string>

using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent::network::api::dcrp;
using namespace std;

class DcrpTest: public ::testing::Test {
protected:
public:
    virtual ~DcrpTest();
};

DcrpTest::~DcrpTest() {}

TEST_F(DcrpTest, load_neighbor_switches_id_pass) {
    RemoteSwitchInfo rsi{};
    rsi.load_neighbor_switches_id("");
}

TEST_F(DcrpTest, load_neighbor_switches_id_fail_on_status) {
    RemoteSwitchInfo rsi{};
    expect_command_status(1);
    ASSERT_THROW(rsi.load_neighbor_switches_id(""), runtime_error);
    expect_command_status(0);
}

TEST_F(DcrpTest, load_neighbor_switches_id_fail_on_socket) {
    RemoteSwitchInfo rsi{};
    expect_fail_on_socket(true);
    ASSERT_THROW(rsi.load_neighbor_switches_id(""), runtime_error);
    expect_fail_on_socket(false);
}

TEST_F(DcrpTest, load_neighbor_switches_id_fail_on_connect) {
    RemoteSwitchInfo rsi{};
    expect_fail_on_connect(true);
    ASSERT_THROW(rsi.load_neighbor_switches_id(""), runtime_error);
    expect_fail_on_connect(false);
}

TEST_F(DcrpTest, load_neighbor_switches_id_fail_on_write) {
    RemoteSwitchInfo rsi{};
    expect_fail_on_write(true);
    ASSERT_THROW(rsi.load_neighbor_switches_id(""), runtime_error);
    expect_fail_on_write(false);
}

TEST_F(DcrpTest, load_neighbor_switches_id_fail_on_read) {
    RemoteSwitchInfo rsi{};
    expect_fail_on_read(true);
    ASSERT_THROW(rsi.load_neighbor_switches_id(""), runtime_error);
    expect_fail_on_read(false);
}

TEST_F(DcrpTest, get_node_id_pass) {
    RemoteSwitchInfo rsi{};
    expect_get_node_id(true);
    rsi.get_local_node_id();
}

TEST_F(DcrpTest, get_node_id_fail_on_status) {
    RemoteSwitchInfo rsi{};
    expect_command_status(1);
    ASSERT_THROW(rsi.get_local_node_id(), runtime_error);
    expect_command_status(0);
    expect_get_node_id(false);
}

TEST_F(DcrpTest, get_node_details_pass) {
    RemoteSwitchInfo rsi{};
    expect_get_node_details(true);
    RemoteEthernetSwitch rs{};
    rsi.fill_response_with_switch_info("Neighbor", rs);
}

TEST_F(DcrpTest, get_node_details_fail_on_status) {
    RemoteSwitchInfo rsi{};
    expect_command_status(1);
    RemoteEthernetSwitch rs{};
    ASSERT_THROW(rsi.fill_response_with_switch_info("Neighbor", rs),
                 runtime_error);
    expect_command_status(0);
    expect_get_node_details(false);
}


