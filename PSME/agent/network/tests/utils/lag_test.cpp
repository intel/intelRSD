/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 * */


#include "gtest/gtest.h"

#include "utils/lag.hpp"
#include "utils/port.hpp"
#include "agent-framework/module/network_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/exceptions/invalid_value.hpp"
#include "agent-framework/exceptions/unsupported_value.hpp"
#include "agent-framework/exceptions/invalid_uuid.hpp"
#include "agent-framework/module/enum/network.hpp"

#include "loader/network_loader.hpp"
#include "configuration_full.hpp"

#include "json-wrapper/json-wrapper.hpp"


#include <string>

using namespace agent_framework::module;
using namespace agent_framework::model::enums;
using namespace agent_framework::exceptions;
using namespace agent::network::utils;
using namespace std;

class LagTest: public ::testing::Test {
protected:
    agent::network::loader::NetworkLoader loader{};
    json::Json config = json::Json();
public:
    virtual ~LagTest();

    void SetUp() {
        config = json::Json::parse(NETWORK_FULL_CONFIGURATION);
    }

    string get_manager_uuid() {
        return CommonComponents::get_instance()->get_module_manager().get_keys("").front();
    }

    string get_switch_uuid() {
        return NetworkComponents::get_instance()->get_switch_manager().get_keys(get_manager_uuid()).front();
    }
};

LagTest::~LagTest() {}

TEST_F(LagTest, add_logical_port) {
    ASSERT_TRUE(loader.load(config));
    auto uuid = add_logical_port("team1", get_switch_uuid());
    NetworkComponents::get_instance()->get_port_manager().get_entry(uuid);
}

TEST_F(LagTest, is_member_port) {
    ASSERT_TRUE(is_member_port("sw0p3"));
}

TEST_F(LagTest, is_not_member_port) {
    ASSERT_FALSE(is_member_port("sw0p1"));
}

TEST_F(LagTest, is_logical_port_empty) {
    ASSERT_FALSE(is_logical_port_empty("team1"));
}

TEST_F(LagTest, get_logical_port_members) {
    auto members = get_logical_port_members("team1");
    ASSERT_EQ(members.size(), 1);
    ASSERT_EQ(members.front(), "sw0p1");
}

TEST_F(LagTest, get_lag_identifier) {
    auto lag = get_lag_identifier("sw0p1");
    ASSERT_EQ(lag, "team1");
}

TEST_F(LagTest, validate_port_members) {
    string uuid1, uuid2;
    get_port_uuid_by_identifier("sw0p1", uuid1);
    get_port_uuid_by_identifier("sw0p2", uuid2);
    validate_port_members({uuid1, uuid2}, 100);
}

TEST_F(LagTest, validate_port_members_speed) {
    string uuid1, uuid2;
    get_port_uuid_by_identifier("sw0p1", uuid1);
    get_port_uuid_by_identifier("sw0p2", uuid2);
    validate_port_members({uuid1, uuid2}, -1);
}

TEST_F(LagTest, validate_port_members_fail_uuid_not_found) {
    string uuid1;
    get_port_uuid_by_identifier("sw0p1", uuid1);
    EXPECT_THROW(validate_port_members({uuid1, "08203498234"}, 100),
                 InvalidUuid);
}

TEST_F(LagTest, validate_port_members_fail_logical) {
    string uuid1, uuid2;
    get_port_uuid_by_identifier("sw0p1", uuid1);
    get_port_uuid_by_identifier("team1", uuid1);
    EXPECT_THROW(validate_port_members({uuid1, uuid2}, 100),
                 InvalidValue);
}

TEST_F(LagTest, validate_port_members_fail_already_member) {
    string uuid1, uuid2;
    get_port_uuid_by_identifier("sw0p1", uuid1);
    get_port_uuid_by_identifier("sw0p3", uuid2);
    EXPECT_THROW(validate_port_members({uuid1, uuid2}, 100),
                 InvalidValue);
}

TEST_F(LagTest, validate_port_members_fail_pcie) {
    string uuid1, uuid2;
    get_port_uuid_by_identifier("sw0p1", uuid1);
    get_port_uuid_by_identifier("sw0p37", uuid2);
    EXPECT_THROW(validate_port_members({uuid1, uuid2}, 100),
                 UnsupportedValue);
}

TEST_F(LagTest, validate_port_members_fail_mesh) {
    string uuid1, uuid2;
    get_port_uuid_by_identifier("sw0p1", uuid1);
    get_port_uuid_by_identifier("sw0p4", uuid2);

    {
        auto port = NetworkComponents::get_instance()->get_port_manager().get_entry_reference(uuid2);
        port->set_port_type(PortType::MeshPort);
    }
    EXPECT_THROW(validate_port_members({uuid1, uuid2}, 100),
                 InvalidValue);
}

TEST_F(LagTest, validate_port_members_fail_speed) {
    string uuid1, uuid2;
    get_port_uuid_by_identifier("sw0p1", uuid1);
    get_port_uuid_by_identifier("sw0p2", uuid2);
    EXPECT_THROW(validate_port_members({uuid1, uuid2}, 10),
                 InvalidValue);
}

