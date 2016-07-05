/*!
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
 * */

#include "psme/rest-ref/mapper/mapper.hpp"
#include "psme/rest-ref/cache/cache.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace psme::rest;
using namespace agent_framework::model::enums;

class MapperTest : public ::testing::Test {
public:
    Cache m_cache{};
    Mapper m_mapper{m_cache};

    ~MapperTest();
};

MapperTest::~MapperTest() {}

TEST_F(MapperTest, AddComputeModulesTest) {
    m_mapper.add(Component::ComputeModule, "", "1234");
    ASSERT_EQ(m_mapper.
              get_uuid_by_link("/redfish/v1/Drawers/1/ComputeModules/1"), "1234");

    m_mapper.add(Component::ComputeModule, "", "2345");
    ASSERT_EQ(m_mapper.
              get_uuid_by_link("/redfish/v1/Drawers/1/ComputeModules/2"), "2345");

    m_mapper.add(Component::ComputeModule, "", "3456");
    ASSERT_EQ(m_mapper.
              get_uuid_by_link("/redfish/v1/Drawers/1/ComputeModules/3"), "3456");
}

TEST_F(MapperTest, AddBladeTest) {
    m_mapper.add(Component::ComputeModule, "", "1234");
    m_mapper.add(Component::Blade, "1234", "4444");
    ASSERT_EQ(m_mapper.
              get_uuid_by_link("/redfish/v1/Drawers/1/ComputeModules/1/Blades/1"), "4444");

    m_mapper.add(Component::Blade, "1234", "6666");
    ASSERT_EQ(m_mapper.
              get_uuid_by_link("/redfish/v1/Drawers/1/ComputeModules/1/Blades/2"), "6666");

    m_mapper.add(Component::ComputeModule, "", "2345");
    m_mapper.add(Component::Blade, "2345", "6666");
    ASSERT_EQ(m_mapper.
              get_uuid_by_link("/redfish/v1/Drawers/1/ComputeModules/2/Blades/1"), "6666");

    m_mapper.add(Component::Blade, "2345", "7777");
    ASSERT_EQ(m_mapper.
              get_uuid_by_link("/redfish/v1/Drawers/1/ComputeModules/2/Blades/2"), "7777");
}

TEST_F(MapperTest, RemoveBladeExpectThrowTest) {
    m_mapper.add(Component::ComputeModule, "", "1234");
    m_mapper.add(Component::Blade, "1234", "4444");
    m_mapper.add(Component::Blade, "1234", "6666");

    m_mapper.remove("6666");
    ASSERT_THROW(m_mapper.
                 get_uuid_by_link("/redfish/v1/Drawers/1/ComputeModules/1/Blades/2"),
                 std::runtime_error);
}

TEST_F(MapperTest, RemoveBladeCheckIndexTest) {
    m_mapper.add(Component::ComputeModule, "", "1234");
    m_mapper.add(Component::Blade, "1234", "4444");
    m_mapper.add(Component::Blade, "1234", "6666");
    m_mapper.add(Component::Blade, "1234", "7777");

    m_mapper.remove("6666");
    m_mapper.add(Component::Blade, "1234", "8888");
    ASSERT_EQ(m_mapper.
              get_uuid_by_link("/redfish/v1/Drawers/1/ComputeModules/1/Blades/4"), "8888");

    m_mapper.remove("8888");
    m_mapper.add(Component::Blade, "1234", "9999");
    ASSERT_EQ(m_mapper.
              get_uuid_by_link("/redfish/v1/Drawers/1/ComputeModules/1/Blades/5"), "9999");
}

TEST_F(MapperTest, CheckLinksTest) {
    m_mapper.add(Component::ComputeModule, "", "1234");
    m_mapper.add(Component::ComputeModule, "", "2345");
    m_mapper.add(Component::Blade, "1234", "4444");
    m_mapper.add(Component::Blade, "1234", "6666");
    m_mapper.add(Component::Blade, "1234", "7777");

    std::vector<std::string> cm = {"/redfish/v1/Drawers/1/ComputeModules/1",
                                   "/redfish/v1/Drawers/1/ComputeModules/2"};
    auto links = m_mapper.get_collection_links("/redfish/v1/Drawers/1/ComputeModules");
    ASSERT_EQ(links.size(), cm.size());
    for (auto i = 0u; i < links.size(); ++i) {
        ASSERT_EQ(links[i], cm[i]);
    }
}

TEST_F(MapperTest, CheckParamsUUIDTest) {
    m_mapper.add(Component::ComputeModule, "", "1234");
    m_mapper.add(Component::ComputeModule, "", "2345");
    m_mapper.add(Component::Blade, "1234", "4444");

    ::psme::rest::server::Parameters params;
    params.set("drawerID", "1");
    params.set("bladeID", "1");
    params.set("moduleID", "1");

    auto result = m_mapper.get_params_uuid(params);

    ASSERT_EQ(result.get("moduleID"), "1234");
    ASSERT_EQ(result.get("bladeID"), "4444");
}

TEST_F(MapperTest, GetUUIDByLinkTest) {
    m_mapper.add(Component::ComputeModule, "", "1234");
    m_mapper.add(Component::ComputeModule, "", "2345");
    m_mapper.add(Component::Blade, "1234", "4444");

    ASSERT_EQ(m_mapper.get_uuid_by_link("/redfish/v1/Drawers/1/ComputeModules/1"),
                                        "1234");

    ASSERT_EQ(m_mapper.get_uuid_by_link("/redfish/v1/Drawers/1/ComputeModules/2"),
                                        "2345");

    ASSERT_EQ(m_mapper.get_uuid_by_link(
                "/redfish/v1/Drawers/1/ComputeModules/1/Blades/1"), "4444");
}

TEST_F(MapperTest, GetLinkByUUIDTest) {
    m_mapper.add(Component::ComputeModule, "", "1234");
    m_mapper.add(Component::ComputeModule, "", "2345");
    m_mapper.add(Component::Blade, "1234", "4444");

    ASSERT_EQ(m_mapper.get_link_by_uuid("1234"),
                                        "/redfish/v1/Drawers/1/ComputeModules/1");
    ASSERT_EQ(m_mapper.get_link_by_uuid("2345"),
                                        "/redfish/v1/Drawers/1/ComputeModules/2");
    ASSERT_EQ(m_mapper.get_link_by_uuid("4444"),
                                "/redfish/v1/Drawers/1/ComputeModules/1/Blades/1");
}

TEST_F(MapperTest, CheckAllComputeManagerTest) {
    m_mapper.add(Component::ComputeModule, "", "1234");

    m_mapper.add(Component::Manager, "1234", "10111");

    m_mapper.add(Component::Blade, "1234", "1222");
    m_mapper.add(Component::Blade, "1234", "2222");

    m_mapper.add(Component::Manager, "1222", "10112");
    m_mapper.add(Component::Manager, "2222", "10113");

    m_mapper.add(Component::Processor, "1222", "3111");
    m_mapper.add(Component::Processor, "1222", "3222");
    m_mapper.add(Component::Processor, "2222", "3333");
    m_mapper.add(Component::Processor, "2222", "3444");

    m_mapper.add(Component::Memory, "1222", "4111");
    m_mapper.add(Component::Memory, "1222", "4222");
    m_mapper.add(Component::Memory, "2222", "4333");
    m_mapper.add(Component::Memory, "2222", "4444");

    m_mapper.add(Component::StorageController, "1222", "5111");
    m_mapper.add(Component::StorageController, "2222", "5222");

    m_mapper.add(Component::Drive, "5111", "6111");
    m_mapper.add(Component::Drive, "5222", "6222");

    m_mapper.add(Component::NetworkInterface, "1222", "7111");
    m_mapper.add(Component::NetworkInterface, "2222", "8222");

    ASSERT_EQ(m_mapper.get_uuid_by_link("/redfish/v1/Drawers/1/ComputeModules/1"),
                                        "1234");

    ASSERT_EQ(m_mapper.get_uuid_by_link("/redfish/v1/Managers/1"), "10111");
    ASSERT_EQ(m_mapper.get_uuid_by_link("/redfish/v1/Managers/2"), "10112");
    ASSERT_EQ(m_mapper.get_uuid_by_link("/redfish/v1/Managers/3"), "10113");

    ASSERT_EQ(m_mapper.get_uuid_by_link(
                "/redfish/v1/Drawers/1/ComputeModules/1/Blades/1"), "1222");
    ASSERT_EQ(m_mapper.get_uuid_by_link(
                "/redfish/v1/Drawers/1/ComputeModules/1/Blades/2"), "2222");

    ASSERT_EQ(m_mapper.get_uuid_by_link(
    "/redfish/v1/Drawers/1/ComputeModules/1/Blades/1/Processors/1"), "3111");
    ASSERT_EQ(m_mapper.get_uuid_by_link(
    "/redfish/v1/Drawers/1/ComputeModules/1/Blades/1/Processors/2"), "3222");
    ASSERT_EQ(m_mapper.get_uuid_by_link(
    "/redfish/v1/Drawers/1/ComputeModules/1/Blades/2/Processors/1"), "3333");
    ASSERT_EQ(m_mapper.get_uuid_by_link(
    "/redfish/v1/Drawers/1/ComputeModules/1/Blades/2/Processors/2"), "3444");
}

