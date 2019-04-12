/*!
 * @brief Implementation of Nvme Key Generator tests.
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
 * @file nvme_key_generator_test.cpp
 */

#include "gtest/gtest.h"

#include "tree_stability/nvme_key_generator.hpp"
#include "loader/config.hpp"

#include <string>
#include <algorithm>

using namespace agent::nvme;
using namespace agent::nvme::loader;
using namespace agent_framework::model;

class NvmeKeyGeneratorTest: public ::testing::Test {
protected:
    NvmeKeyGenerator nkg{};
    const std::string UUID1{"4083549a-49a9-55b8-9269-11567a5e6305"};
    const std::string UUID2{"5083539a-49a9-55b8-9269-11567a6e6305"};
public:
    void SetUp() override;

    virtual ~NvmeKeyGeneratorTest();
};

NvmeKeyGeneratorTest::~NvmeKeyGeneratorTest() { }

void NvmeKeyGeneratorTest::SetUp() {
    nkg.set_agent_id("nvme_unit_tests");
}

TEST_F(NvmeKeyGeneratorTest, ManagerTest1) {
    Manager manager1{}, manager2{};
    manager1.set_ipv4_address("192.168.0.1");
    manager2.set_ipv4_address("192.168.0.1");
    ASSERT_EQ(nkg.generate_key(manager1), nkg.generate_key(manager2));
}

TEST_F(NvmeKeyGeneratorTest, ManagerTest2) {
    Manager manager1{}, manager2{};
    manager1.set_ipv4_address("192.168.0.1");
    manager2.set_ipv4_address("192.168.0.2");
    ASSERT_NE(nkg.generate_key(manager1), nkg.generate_key(manager2));
}

TEST_F(NvmeKeyGeneratorTest, ChassisTest1) {
    Chassis chassis1{}, chassis2{};
    ASSERT_EQ(nkg.generate_key(chassis1), nkg.generate_key(chassis2));
}

TEST_F(NvmeKeyGeneratorTest, ChassisTest2) {
    Chassis chassis1{}, chassis2{};
    auto key1 = nkg.generate_key(chassis1);
    NvmeConfig::get_instance()->set_parent_id("2");
    auto key2 = nkg.generate_key(chassis2);
    ASSERT_NE(key1, key2);
}

TEST_F(NvmeKeyGeneratorTest, FabricTest1) {
    Fabric fabric1{}, fabric2{};
    ASSERT_EQ(nkg.generate_key(fabric1), nkg.generate_key(fabric2));
}

TEST_F(NvmeKeyGeneratorTest, FabricTest2) {
    Fabric fabric1{}, fabric2{};
    NvmeConfig::get_instance()->set_parent_id("1");
    auto key1 = nkg.generate_key(fabric1);
    NvmeConfig::get_instance()->set_parent_id("2");
    auto key2 = nkg.generate_key(fabric2);
    ASSERT_NE(key1, key2);
}

TEST_F(NvmeKeyGeneratorTest, SystemTest1) {
    System system1{}, system2{};
    ASSERT_EQ(nkg.generate_key(system1), nkg.generate_key(system2));
}

TEST_F(NvmeKeyGeneratorTest, SystemTest2) {
    System system1{}, system2{};
    NvmeConfig::get_instance()->set_parent_id("1");
    auto key1 = nkg.generate_key(system1);
    NvmeConfig::get_instance()->set_parent_id("2");
    auto key2 = nkg.generate_key(system2);
    ASSERT_NE(key1, key2);
}

TEST_F(NvmeKeyGeneratorTest, NetworkInterfaceTest1) {
    NetworkInterface ni1{}, ni2{};
    ni1.set_mac_address("aa:bb:cc:dd:ee:ff");
    ni2.set_mac_address("aa:bb:cc:dd:ee:ff");
    ASSERT_EQ(nkg.generate_key(ni1), nkg.generate_key(ni2));
}

TEST_F(NvmeKeyGeneratorTest, NetworkInterfaceTest2) {
    NetworkInterface ni1{}, ni2{};
    ni1.set_mac_address("aa:bb:cc:dd:ee:ff");
    ni2.set_mac_address("aa:bb:cc:dd:ee:fe");
    ASSERT_NE(nkg.generate_key(ni1), nkg.generate_key(ni2));
}

TEST_F(NvmeKeyGeneratorTest, EndpointTestSameNqn) {
    Endpoint e1{}, e2{};
    e1.add_identifier({"nqnjlkjsdfoijoweirj", enums::IdentifierType::NQN});
    e2.add_identifier({"nqnjlkjsdfoijoweirj", enums::IdentifierType::NQN});
    ASSERT_EQ(nkg.generate_key(e1), nkg.generate_key(e2));
}

TEST_F(NvmeKeyGeneratorTest, EndpointTestDifferentNqn) {
    Endpoint e1{}, e2{};
    e1.add_identifier({"nqnjlkjsdfoijoweirj", enums::IdentifierType::NQN});
    e2.add_identifier({"nqnjlkjsdfoijoweiej", enums::IdentifierType::NQN});
    ASSERT_NE(nkg.generate_key(e1), nkg.generate_key(e2));
}

TEST_F(NvmeKeyGeneratorTest, EndpointTestNoKey) {
    Endpoint e{};
    ASSERT_THROW(nkg.generate_key(e), agent_framework::KeyValueMissingError);
}

TEST_F(NvmeKeyGeneratorTest, EndpointTestSameNqnButDifferentRole) {
    Endpoint e1{}, e2{};
    attribute::ConnectedEntity ce1{}, ce2{};
    // two endpoints with same NQN but different role should have different key
    e1.add_identifier({"nqnjlkjsdfoijoweirj", enums::IdentifierType::NQN});
    e2.add_identifier({"nqnjlkjsdfoijoweirj", enums::IdentifierType::NQN});
    ce1.set_entity_role(enums::EntityRole::Target);
    e1.add_connected_entity(ce1);
    ce2.set_entity_role(enums::EntityRole::Initiator);
    e2.add_connected_entity(ce2);
    ASSERT_NE(nkg.generate_key(e1), nkg.generate_key(e2));
}
