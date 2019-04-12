/*!
 * @brief Tests for IPMI communication classes.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file ipmi_intf_test.cpp
 */

#include "ipmi/ipmi_controller.hpp"
#include "ipmi/ipmi_interface_factory.hpp"

#include "test_message.hpp"
#include "test_connection_data.hpp"
#include "test_ipmi_interface.hpp"

#include <thread>
#include "gtest/gtest.h"

namespace testing {

/*!
 * @brief Test bridges
 */
TEST(IpmiInterface, Bridge) {
    ipmi::BridgeInfo::Address addr;
    ipmi::BridgeInfo::Channel chnl;

    ipmi::BridgeInfo br_none{};
    ASSERT_EQ(ipmi::BridgeInfo::Level::DIRECT, br_none.get_level());
    ASSERT_FALSE(br_none.get_target(addr, chnl));
    ASSERT_FALSE(br_none.get_transit(addr, chnl));

    ipmi::BridgeInfo br_single{0x21, 0x01};
    ASSERT_EQ(ipmi::BridgeInfo::Level::SINGLE_BRIDGE, br_single.get_level());
    ASSERT_FALSE(br_single.get_transit(addr, chnl));
    ASSERT_TRUE(br_single.get_target(addr, chnl));
    ASSERT_EQ(0x21, addr);
    ASSERT_EQ(0x01, chnl);

    ASSERT_TRUE(br_single.set_target(0x17));
    ASSERT_TRUE(br_single.get_target(addr, chnl));
    ASSERT_EQ(0x21, addr);
    ASSERT_EQ(0x17, chnl);

    ipmi::BridgeInfo br_dual{0x24, 0x05, 0x21, 0x01};
    ASSERT_EQ(ipmi::BridgeInfo::Level::DUAL_BRIDGE, br_dual.get_level());
    ASSERT_TRUE(br_dual.get_target(addr, chnl));
    ASSERT_EQ(0x21, addr);
    ASSERT_EQ(0x01, chnl);
    ASSERT_TRUE(br_dual.get_transit(addr, chnl));
    ASSERT_EQ(0x24, addr);
    ASSERT_EQ(0x05, chnl);

    ASSERT_TRUE(br_dual.set_transit(0x07));
    ASSERT_TRUE(br_dual.get_transit(addr, chnl));
    ASSERT_EQ(0x24, addr);
    ASSERT_EQ(0x07, chnl);
}

/*!
 * @brief Testing management controller
 */
class TestManagementController : public ipmi::IpmiController {
public:
    static void initialize_factory() {
        /* initialize factory */
        ipmi::IpmiInterfaceFactory& factory =  ipmi::IpmiInterfaceFactory::get_instance();
        factory.add_builder<testing::TestIpmiInterface>(testing::TestConnectionData::INTF_TYPE);
    }
    static size_t get_factory_size() {
        ipmi::IpmiInterfaceFactory& factory =  ipmi::IpmiInterfaceFactory::get_instance();
        return factory.interfaces.size();
    }

    TestManagementController(const std::string& descr) :
        ipmi::IpmiController(std::make_shared<TestConnectionData>(descr)) {}

    void set_descr(const std::string& descr) {
        TestConnectionData& data = dynamic_cast<TestConnectionData&>(*ipmi::IpmiController::data_to_modify());
        data.set_descr(descr);
    }
};

/*!
 * @brief Test basic mechanisms of the model
 */
TEST(IpmiInterface, Factory) {
    TestManagementController::initialize_factory();

    testing::TestManagementController ctrl1("ctrl");
    testing::TestManagementController ctrl2("ctrl");
    testing::TestManagementController ctrl3("different");

    testing::TestRequest req{};
    testing::TestResponse resp{};

    ctrl1.send(req, resp);
    ASSERT_EQ("ctrl", resp.get_descr());
    ASSERT_EQ(1, resp.get_serial()) << "First message";

    ctrl1.send(req, resp);
    ASSERT_EQ("ctrl", resp.get_descr());
    ASSERT_EQ(2, resp.get_serial()) << "Second message";

    ctrl2.send(req, resp);
    ASSERT_EQ("ctrl", resp.get_descr());
    ASSERT_EQ(3, resp.get_serial()) << "Third message (from another controller)";

    ctrl3.send(req, resp);
    ASSERT_EQ("different", resp.get_descr());
    ASSERT_EQ(1, resp.get_serial()) << "First message (to new interface)";

    ctrl1.set_descr("changed");
    ctrl1.send(req, resp);
    ASSERT_EQ("changed", resp.get_descr());
    ASSERT_EQ(1, resp.get_serial()) << "After rename";

    ASSERT_THROW(ctrl1.send(req, {1, 2}, resp), ipmi::ResponseError);

    ctrl2.send(req, resp);
    ASSERT_EQ("ctrl", resp.get_descr());
    ASSERT_EQ(4, resp.get_serial()) << "Fourth message (after rename #1)";

    ASSERT_EQ(3, TestManagementController::get_factory_size());
    ctrl2.close();
    ASSERT_EQ(2, TestManagementController::get_factory_size());

    ctrl2.send(req, resp);
    ASSERT_EQ("ctrl", resp.get_descr());
    ASSERT_EQ(1, resp.get_serial()) << "First message after close";
    ASSERT_EQ(3, TestManagementController::get_factory_size());
}

/*!
 * @brief Test request with the data, and response without
 */
TEST(IpmiInterface, EmptyResponse) {
    TestManagementController::initialize_factory();

    testing::TestManagementController ctrl1("ctrl");

    testing::EmptyResponse resp{};
    testing::DataRequest req{10};

    ctrl1.send(req, resp);
    ASSERT_EQ(1, resp.get_size());
}

static void scoped_loop() {
    testing::TestRequest req{};
    testing::TestResponse resp{};

    for (int i = 0; i < 10; i++) {
        testing::TestManagementController ctrl1("ctrl");

        {
            testing::TestManagementController ctrl2("ctrl");

            /* ctrl2 allocates the interface */
            ctrl2.send(req, resp);
            ASSERT_EQ("ctrl", resp.get_descr());

            /* ctrl1 uses just created interface */
            ctrl1.send(req, resp);
            ASSERT_EQ("ctrl", resp.get_descr());

            /* ctrl2 is out of scope, connection data is freed as well */
        }

        testing::TestManagementController ctrl3("ctrl");
        /* drop core on unassigned memory reads */
        ctrl3.send(req, resp);
        ASSERT_EQ("ctrl", resp.get_descr());
    }
}

/*!
 * @brief Random condition with objects scope
 *
 * There was an core-dumping race condition, if connection data is owned by non-existing object.
 * This only happens when several controllers use same interface (its connection data matches)
 */
TEST(IpmiInterface, ScopedReferences) {
    TestManagementController::initialize_factory();

    std::thread first(scoped_loop);
    std::thread second(scoped_loop);

    /* it takes a while.. */

    first.join();
    second.join();
}

}
