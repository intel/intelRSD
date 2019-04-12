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

#include "safe-string/safe_lib.hpp"

#include "gas/access_interface_factory.hpp"
#include "gas/pcie_access_interface.hpp"
#include "gas/global_address_space_registers.hpp"
#include "gas/mrpc/bind_port.hpp"
#include "gas/mrpc/unbind_port.hpp"
#include "gas/mrpc/twi_access_write.hpp"
#include "gas/mrpc/twi_access_read.hpp"
#include "gas/mrpc/link_status_retrieve.hpp"
#include "gas/utils.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <fstream>


using namespace agent::pnc::gas;
using namespace agent::pnc::gas::mrpc;
using ::testing::ElementsAre;
using P2PReturnValue = agent::pnc::gas::PortPartitionP2PBindingReturnValue;

class UtilsTest: public ::testing::Test {

protected:
    void SetUp() {
    }

public:
    UtilsTest() {}
    virtual ~UtilsTest();
};

UtilsTest::~UtilsTest() { }

TEST_F(UtilsTest, P2PBindingResult) {
    uint32_t out_of_range{888};
    ASSERT_NO_THROW(get_p2p_binding_command_result(out_of_range));
    ASSERT_EQ(get_p2p_binding_command_result(out_of_range), std::string("Unknown value: ")+std::to_string(out_of_range));
    ASSERT_EQ(get_p2p_binding_command_result(static_cast<uint32_t>(P2PReturnValue::COMMAND_SUCCEED)), "Command succeeded");
    ASSERT_EQ(get_p2p_binding_command_result(static_cast<uint32_t>(P2PReturnValue::PHY_PORT_ALREADY_BOUND)), "Physical port already bound");
    ASSERT_EQ(get_p2p_binding_command_result(static_cast<uint32_t>(P2PReturnValue::LOGICAL_BRIDGE_BINDING_INSTANCE_ALREADY_BOUND)),
                "Logical bridge binding instance already bound");
    ASSERT_EQ(get_p2p_binding_command_result(static_cast<uint32_t>(P2PReturnValue::PARTITION_DOES_NOT_EXIST)), "Partition does not exist");
    ASSERT_EQ(get_p2p_binding_command_result(static_cast<uint32_t>(P2PReturnValue::PHY_PORT_DOES_NOT_EXIST)), "Physical port does not exist");
    ASSERT_EQ(get_p2p_binding_command_result(static_cast<uint32_t>(P2PReturnValue::PHY_PORT_DISABLED)), "Physical port disabled");
    ASSERT_EQ(get_p2p_binding_command_result(static_cast<uint32_t>(P2PReturnValue::NO_LOGICAL_BRIDGE_BINDING_INSTANCE)), "No logical bridge binding instance");
    ASSERT_EQ(get_p2p_binding_command_result(static_cast<uint32_t>(P2PReturnValue::BIND_UNBIND_IN_PROGRESS)), "Bind/unbind in progress");
    ASSERT_EQ(get_p2p_binding_command_result(static_cast<uint32_t>(P2PReturnValue::BIND_UNBIND_TARGET_IS_USP)), "Bind/unbind target is USP");
    ASSERT_EQ(get_p2p_binding_command_result(static_cast<uint32_t>(P2PReturnValue::SUB_COMMAND_DOES_NOT_EXIST)), "Sub command does not exist");
    ASSERT_EQ(get_p2p_binding_command_result(static_cast<uint32_t>(P2PReturnValue::PHY_PORT_LINK_ACTIVE)), "Physical port link active");
    ASSERT_EQ(get_p2p_binding_command_result(static_cast<uint32_t>(P2PReturnValue::LOGICAL_BRIDGE_BINDING_INSTANCE_NOT_BOUND_TO_PHY_PORT)),
        "Logical bridge binding instance not bound to physical port");
    ASSERT_EQ(get_p2p_binding_command_result(static_cast<uint32_t>(P2PReturnValue::INVALID_UNBIND_OPTION)), "Invalid unbind option");

    ASSERT_EQ(get_p2p_binding_command_result((P2PReturnValue::COMMAND_SUCCEED)), "Command succeeded");
    ASSERT_EQ(get_p2p_binding_command_result((P2PReturnValue::PHY_PORT_ALREADY_BOUND)), "Physical port already bound");
    ASSERT_EQ(get_p2p_binding_command_result((P2PReturnValue::LOGICAL_BRIDGE_BINDING_INSTANCE_ALREADY_BOUND)),
                "Logical bridge binding instance already bound");
    ASSERT_EQ(get_p2p_binding_command_result((P2PReturnValue::PARTITION_DOES_NOT_EXIST)), "Partition does not exist");
    ASSERT_EQ(get_p2p_binding_command_result(static_cast<uint32_t>(P2PReturnValue::PHY_PORT_DOES_NOT_EXIST)), "Physical port does not exist");
    ASSERT_EQ(get_p2p_binding_command_result(static_cast<uint32_t>(P2PReturnValue::PHY_PORT_DISABLED)), "Physical port disabled");
    ASSERT_EQ(get_p2p_binding_command_result(static_cast<uint32_t>(P2PReturnValue::NO_LOGICAL_BRIDGE_BINDING_INSTANCE)), "No logical bridge binding instance");
    ASSERT_EQ(get_p2p_binding_command_result(static_cast<uint32_t>(P2PReturnValue::BIND_UNBIND_IN_PROGRESS)), "Bind/unbind in progress");
    ASSERT_EQ(get_p2p_binding_command_result(static_cast<uint32_t>(P2PReturnValue::BIND_UNBIND_TARGET_IS_USP)), "Bind/unbind target is USP");
    ASSERT_EQ(get_p2p_binding_command_result(static_cast<uint32_t>(P2PReturnValue::SUB_COMMAND_DOES_NOT_EXIST)), "Sub command does not exist");
    ASSERT_EQ(get_p2p_binding_command_result(static_cast<uint32_t>(P2PReturnValue::PHY_PORT_LINK_ACTIVE)), "Physical port link active");
    ASSERT_EQ(get_p2p_binding_command_result(static_cast<uint32_t>(P2PReturnValue::LOGICAL_BRIDGE_BINDING_INSTANCE_NOT_BOUND_TO_PHY_PORT)),
        "Logical bridge binding instance not bound to physical port");
    ASSERT_EQ(get_p2p_binding_command_result(static_cast<uint32_t>(P2PReturnValue::INVALID_UNBIND_OPTION)), "Invalid unbind option");
}


TEST_F(UtilsTest, UnbindOption) {
    uint32_t out_of_range{250};
    ASSERT_NO_THROW(get_unbind_option(out_of_range));
    ASSERT_EQ(get_unbind_option(out_of_range),std::string("Unknown value: ")+std::to_string(out_of_range));
    ASSERT_EQ(get_unbind_option(static_cast<uint32_t>(UnbindPortOption::UNBIND_IF_LINK_IS_DOWN)), "Link down");
    ASSERT_EQ(get_unbind_option(static_cast<uint32_t>(UnbindPortOption::IF_PHY_PORT_IS_L0_OR_L1_SIM_MANAGED_HOT_REMOVE)), "Managed hot remove");
    ASSERT_EQ(get_unbind_option(static_cast<uint32_t>(UnbindPortOption::IF_PHY_PORT_IS_L0_OR_L1_SIM_SURPRISE_HOT_REMOVE)), "Surprise hot remove");
    ASSERT_EQ(get_unbind_option(static_cast<uint32_t>(UnbindPortOption::IF_PHY_PORT_IS_L0_OR_L1_SIM_LINK_DOWN)), "Simulated link down");
}

