/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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

#include "../mocks/mock_logger_ext.hpp"

#include "agent-framework/module/vlan.hpp"
#include "configuration/configuration.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace agent_framework::generic;
using namespace agent_framework::testing;
using ::testing::Return;
using ::testing::Throw;

using VlanSharedPtr = Vlan::VlanSharedPtr;

class VlanTest : public ::testing::Test {
protected:
    VlanSharedPtr m_vlan{};
    std::unique_ptr<logger_cpp::Logger> m_logusrmock{};

    static constexpr const uint TEST_ID = 1;

    virtual void SetUp() {
        m_logusrmock.reset(new MockLogger);
        m_vlan.reset(new Vlan());
    }

    virtual void TearDown() {
        m_logusrmock.reset(nullptr);
    }

    virtual ~VlanTest();
};

constexpr uint VlanTest::TEST_ID;

VlanTest::~VlanTest() {}

/* Positive. */

TEST_F(VlanTest, PositiveSetVlanId) {
    json::Value vlan_config;
    vlan_config["id"] = TEST_ID;

    EXPECT_NO_THROW(m_vlan->read_configuration(vlan_config));

    ASSERT_EQ(m_vlan->get_id(), TEST_ID);
}
