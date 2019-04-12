/*!
 * @brief Implementation of DiscoveryManager tests.
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
 * @file discovery_manager_test.cpp
 */

/* Test includes */
#include "mock_spdk_api.hpp"
#include "mock_sysfs_interface.hpp"

/* Agent */
#include "discovery/discovery_manager.hpp"
#include "spdk_agent_context.hpp"

/* Agent framework */
#include "agent-framework/module/managers/utils/manager_utils.hpp"
#include "agent-framework/version.hpp"

/* Interface reader */
#include "interface-reader/udev_interface_reader.hpp"

/* Sysfs */
#include "sysfs/sysfs_interface.hpp"

/* 3rd party */
#include "gtest/gtest.h"



using namespace agent_framework::model;
using namespace agent_framework::module;

class DiscoveryManagerTest : public ::testing::Test {

public:
    DiscoveryManagerTest();


    virtual void SetUp();


    virtual void TearDown();


protected:
    agent::spdk::AgentContext::SPtr m_context{};
    agent::spdk::discovery::DiscoveryManager m_discovery_manager{m_context};
    Uuid m_uuid{};

};

constexpr const char agent::spdk::testing::MockSpdkApi::LV[];
constexpr const char agent::spdk::testing::MockSpdkApi::LVS[];
constexpr const char agent::spdk::testing::MockSpdkApi::NVME[];
constexpr const char agent::spdk::testing::MockSpdkApi::NQN_TGT[];
constexpr const char agent::spdk::testing::MockSpdkApi::NQN_INIT[];


void DiscoveryManagerTest::SetUp() {
    m_context = std::make_shared<agent::spdk::AgentContext>();
    m_discovery_manager = agent::spdk::discovery::DiscoveryManager{m_context};
    m_context->spdk_api = std::make_shared<agent::spdk::testing::MockSpdkApi>();
    m_context->interface_reader = std::make_shared<interface_reader::UdevInterfaceReader>(
        std::vector<std::string>{});
    m_context->sysfs_interface = std::make_shared<::agent::spdk::testing::MockSysfsInterface>();
    m_uuid = m_discovery_manager.discover();
}


void DiscoveryManagerTest::TearDown() {

    get_manager<Manager>().clear_entries();
    get_manager<System>().clear_entries();
    get_manager<Chassis>().clear_entries();
    get_manager<StorageService>().clear_entries();
    get_manager<Fabric>().clear_entries();
    get_manager<NetworkInterface>().clear_entries();
    get_manager<Drive>().clear_entries();
    get_manager<Volume>().clear_entries();
    get_manager<StoragePool>().clear_entries();
    get_m2m_manager<Zone, Endpoint>().clear_entries();
    get_manager<Endpoint>().clear_entries();
    get_manager<Zone>().clear_entries();
}


DiscoveryManagerTest::DiscoveryManagerTest() {}


TEST_F(DiscoveryManagerTest, TestModelEntryManager) {

    auto manager_entries = get_manager<Manager>().get_entries();
    ASSERT_EQ(manager_entries.size(), 1);
    auto manager = manager_entries.front();

    ASSERT_EQ(m_uuid, manager.get_uuid());
    ASSERT_EQ(manager.get_firmware_version(), agent_framework::generic::Version::VERSION_STRING);

    auto status = manager.get_status();
    ASSERT_EQ(status.get_health(), enums::Health::OK);
    ASSERT_EQ(status.get_state(), enums::State::Enabled);

    ASSERT_EQ(manager.get_manager_type(), agent_framework::model::enums::ManagerInfoType::EnclosureManager);
}


TEST_F(DiscoveryManagerTest, TestModelEntryFabric) {
    auto fabric_entries = get_manager<Fabric>().get_entries(m_uuid);
    ASSERT_EQ(fabric_entries.size(), 1);

    auto fabric = fabric_entries.front();
    ASSERT_EQ(fabric.get_protocol(), agent_framework::model::enums::TransportProtocol::NVMeOverFabrics);

    auto status = fabric.get_status();
    ASSERT_EQ(status.get_health(), enums::Health::OK);
    ASSERT_EQ(status.get_state(), enums::State::Enabled);
}


TEST_F(DiscoveryManagerTest, TestModelEntryChassis) {
    auto chassis_entries = get_manager<Chassis>().get_entries(m_uuid);
    ASSERT_EQ(chassis_entries.size(), 1);

    auto chassis = chassis_entries.front();
    ASSERT_EQ(chassis.get_type(), agent_framework::model::enums::ChassisType::Enclosure);

    auto status = chassis.get_status();
    ASSERT_EQ(status.get_health(), enums::Health::OK);
    ASSERT_EQ(status.get_state(), enums::State::Enabled);
}


TEST_F(DiscoveryManagerTest, TestModelEntryStorageService) {
    auto storage_service_entries = get_manager<StorageService>().get_entries(m_uuid);
    ASSERT_EQ(storage_service_entries.size(), 1);

    auto storage_service = storage_service_entries.front();

    auto status = storage_service.get_status();
    ASSERT_EQ(status.get_health(), enums::Health::OK);
    ASSERT_EQ(status.get_state(), enums::State::Enabled);
}


TEST_F(DiscoveryManagerTest, TestModelEntrySystem) {
    auto system_entries = get_manager<System>().get_entries(m_uuid);
    ASSERT_EQ(system_entries.size(), 1);

    auto system = system_entries.front();
    ASSERT_EQ(system.get_system_type(), agent_framework::model::enums::SystemType::Virtual);

    auto status = system.get_status();
    ASSERT_EQ(status.get_health(), enums::Health::OK);
    ASSERT_EQ(status.get_state(), enums::State::Enabled);

    ASSERT_EQ(system.get_guid().value(), ::agent::spdk::testing::SYSTEM_FILE_TEST_VALUE);
}


TEST_F(DiscoveryManagerTest, TestModelEntryDrive) {
    auto chassis_uuid = get_manager<Chassis>().get_entries(m_uuid).front().get_uuid();
    auto drive_entries = get_manager<Drive>().get_entries(chassis_uuid);

    ASSERT_EQ(drive_entries.size(), 1);
}


TEST_F(DiscoveryManagerTest, TestModelEntryStoragePool) {

    auto storage_service_uuid = get_manager<StorageService>().get_entries(m_uuid).front().get_uuid();
    auto storage_pool_entries = get_manager<StoragePool>().get_entries(storage_service_uuid);
    ASSERT_EQ(storage_pool_entries.size(), 1);
}


TEST_F(DiscoveryManagerTest, TestModelEntryVolume) {
    auto storage_service_uuid = get_manager<StorageService>().get_entries(m_uuid).front().get_uuid();
    auto volume_entries = get_manager<Volume>().get_entries(storage_service_uuid);
    ASSERT_EQ(volume_entries.size(), 1);
}


TEST_F(DiscoveryManagerTest, TestModelEntryZone) {
    auto fabric_uuid = get_manager<Fabric>().get_entries(m_uuid).front().get_uuid();
    auto zone_entries = get_manager<Zone>().get_entries(fabric_uuid);
    ASSERT_EQ(get_manager<Zone>().get_entries().size(), 1);
}


TEST_F(DiscoveryManagerTest, TestModelEntryEndpoint) {
    auto fabric_uuid = get_manager<Fabric>().get_entries(m_uuid).front().get_uuid();
    auto endpoint_entries = get_manager<Endpoint>().get_entries(fabric_uuid);
    ASSERT_EQ(endpoint_entries.size(), 2);

}


TEST_F(DiscoveryManagerTest, CheckStoragePoolAttributes) {

    auto lvol_store = m_context->spdk_api->get_lvol_stores().front();
    auto storage_service_uuid = get_manager<StorageService>().get_entries(m_uuid).front().get_uuid();
    auto storage_pool = get_manager<StoragePool>().get_entries(storage_service_uuid).front();

    auto status = storage_pool.get_status();
    ASSERT_EQ(status.get_health(), enums::Health::OK);
    ASSERT_EQ(status.get_state(), enums::State::Enabled);

    auto capacity = storage_pool.get_capacity();
    ASSERT_EQ(capacity.get_allocated_bytes(), lvol_store.get_cluster_size() * lvol_store.get_total_data_clusters());
    ASSERT_EQ(capacity.get_consumed_bytes(),
              lvol_store.get_cluster_size() * lvol_store.get_total_data_clusters() - lvol_store.get_free_clusters());
    ASSERT_EQ(capacity.get_guaranteed_bytes(), lvol_store.get_free_clusters() * lvol_store.get_cluster_size());

    ASSERT_EQ(attribute::Identifier::get_uuid(storage_pool), agent::spdk::testing::MockSpdkApi::LVS);
}


TEST_F(DiscoveryManagerTest, CheckVolumeAttributes) {

    auto lvol_bdev = m_context->spdk_api->get_bdevs(::agent::spdk::testing::MockSpdkApi::LV).front();

    auto storage_service = get_manager<StorageService>().get_entries(m_uuid).front();
    auto volume = get_manager<Volume>().get_entries(storage_service.get_uuid()).front();

    auto status = volume.get_status();
    ASSERT_EQ(status.get_health(), enums::Health::OK);
    ASSERT_EQ(status.get_state(), enums::State::Enabled);

    ASSERT_EQ(volume.get_block_size_bytes(), lvol_bdev.get_block_size());

    auto capacity = volume.get_capacity();
    ASSERT_EQ(capacity.get_allocated_bytes(), lvol_bdev.get_num_blocks() * lvol_bdev.get_block_size());

    ASSERT_EQ(volume.get_access_capabilities().front(), enums::AccessCapability::Read);

    ASSERT_EQ(attribute::Identifier::get_uuid(volume), lvol_bdev.get_name());

}


TEST_F(DiscoveryManagerTest, CheckDriveAttributes) {

    auto chassis_uuid = get_manager<Chassis>().get_entries(m_uuid).front().get_uuid();
    auto drive = get_manager<Drive>().get_entries(chassis_uuid).front();

    auto status = drive.get_status();
    ASSERT_EQ(status.get_health(), enums::Health::OK);
    ASSERT_EQ(status.get_state(), enums::State::Enabled);

    auto nvme_bdev = m_context->spdk_api->get_bdevs(agent::spdk::testing::MockSpdkApi::NVME).front();
    ASSERT_EQ(drive.get_block_size_bytes(), nvme_bdev.get_block_size());
    ASSERT_EQ(drive.get_name(), nvme_bdev.get_name());

    ASSERT_EQ(drive.get_type(), enums::DriveType::SSD);
    ASSERT_EQ(drive.get_interface(), enums::TransportProtocol::NVMe);
    ASSERT_DOUBLE_EQ(drive.get_capacity_gb() * 1000 * 1000 * 1000,
        (double)(nvme_bdev.get_block_size() * nvme_bdev.get_num_blocks()));
}


TEST_F(DiscoveryManagerTest, CheckEndpointsAttributes) {

    auto fabrics_uuid = get_manager<Fabric>().get_entries(m_uuid).front().get_uuid();

    auto endpoints = get_manager<Endpoint>().get_entries(fabrics_uuid);
    ASSERT_EQ(endpoints.size(), 2);

    for (const auto& endpoint: endpoints) {

        auto status = endpoint.get_status();
        ASSERT_EQ(status.get_health(), enums::Health::OK);
        ASSERT_EQ(status.get_state(), enums::State::Enabled);
        ASSERT_NE(endpoint.get_connected_entities().size(), 0);

        auto connected_entity = endpoint.get_connected_entities().front();
        ASSERT_TRUE(connected_entity.get_entity_role().has_value());
        if (connected_entity.get_entity_role() == enums::EntityRole::Target) {
            ASSERT_EQ(attribute::Identifier::get_nqn(endpoint),
                      agent::spdk::testing::MockSpdkApi::NQN_TGT);
            ASSERT_TRUE(get_manager<Volume>().entry_exists(connected_entity.get_entity()));

            auto volume = get_manager<Volume>().get_entry(connected_entity.get_entity());
            ASSERT_EQ(attribute::Identifier::get_uuid(volume), ::agent::spdk::testing::MockSpdkApi::LV);
        }
        else if (connected_entity.get_entity_role() == enums::EntityRole::Initiator) {
            ASSERT_EQ(attribute::Identifier::get_nqn(endpoint),
                      agent::spdk::testing::MockSpdkApi::NQN_INIT);
        }

    }
}


TEST_F(DiscoveryManagerTest, CheckZoneAttributes) {

    auto fabrics_uuid = get_manager<Fabric>().get_entries(m_uuid).front().get_uuid();
    auto zone = get_manager<Zone>().get_entries(fabrics_uuid).front();
    auto endpoints = get_m2m_manager<Zone, Endpoint>().get_children(zone.get_uuid());

    auto status = zone.get_status();
    ASSERT_EQ(status.get_health(), enums::Health::OK);
    ASSERT_EQ(status.get_state(), enums::State::Enabled);

    //Checking if zone contains target and initiator
    ASSERT_EQ(endpoints.size(), 2);
    bool found_init = false;
    bool found_tgt = true;

    for (const auto& endpoint_uuid: endpoints) {

        ASSERT_TRUE(get_manager<Endpoint>().entry_exists(endpoint_uuid));
        auto check_if_entry_exists = get_m2m_manager<Zone, Endpoint>().entry_exists(zone.get_uuid(),
                                                                                    endpoint_uuid);
        ASSERT_TRUE(check_if_entry_exists);

        auto endpoint = get_manager<Endpoint>().get_entry(endpoint_uuid);
        auto connected_entity = endpoint.get_connected_entities().front();
        if (connected_entity.get_entity_role() == enums::EntityRole::Target) {
            found_tgt = true;
        }
        else if (connected_entity.get_entity_role() == enums::EntityRole::Initiator) {
            found_init = true;
        }

    }
    ASSERT_TRUE(found_init);
    ASSERT_TRUE(found_tgt);

}







