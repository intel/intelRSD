/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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
 * @file snapshot_lvol_bdev_test.cpp
 */

#include "spdk/spdk_api.hpp"

#include "connectors/snapshot_lvol_bdev_connector.hpp"

#include "gtest/gtest.h"

#include "spdk/model/drivers/lvol_driver.hpp"



class SnapshotLvolBdevTest : public ::testing::Test {
protected:

    std::string valid_lvol_bdev_snapshot_uuid = "cc8d7fdf-7865-4d1f-9fc6-35da8e368670";

    std::shared_ptr<spdk::SpdkApi> spdk_api = {};


    void SetUp() override {
        std::shared_ptr<spdk::SnapshotLvolBdevConnector> stub_connector = std::make_shared<spdk::SnapshotLvolBdevConnector>();
        std::shared_ptr<json_rpc::JsonRpcRequestInvoker> invoker = std::make_shared<json_rpc::JsonRpcRequestInvoker>();
        spdk_api = std::make_shared<::spdk::SpdkApi>(stub_connector, invoker);
    }


    SnapshotLvolBdevTest();


    virtual ~SnapshotLvolBdevTest();
};


SnapshotLvolBdevTest::SnapshotLvolBdevTest() {}


SnapshotLvolBdevTest::~SnapshotLvolBdevTest() {}


namespace testing {

TEST_F(SnapshotLvolBdevTest, ValidMethodResultWithUuid) {

    std::string lvol_bdev_snapshot_uuid = spdk_api->snapshot_lvol_bdev("09959197-1212-4343-8989-676711194545",
                                                                       "09090909-0909-0909-0909-090909090909");

    ASSERT_EQ(lvol_bdev_snapshot_uuid, valid_lvol_bdev_snapshot_uuid);

    std::vector<spdk::model::Bdev> lvol_bdevs = spdk_api->get_bdevs();

    ASSERT_NE(lvol_bdevs[0].get_uuid(), lvol_bdevs[1].get_uuid());
    ASSERT_EQ(lvol_bdevs[0].get_uuid().value(), "09959197-1212-4343-8989-676711194545");
    ASSERT_EQ(lvol_bdevs[1].get_uuid().value(), "cc8d7fdf-7865-4d1f-9fc6-35da8e368670");

    ASSERT_EQ(lvol_bdevs[0].get_name(), "09959197-1212-4343-8989-676711194545");
    ASSERT_EQ(lvol_bdevs[0].get_name(), lvol_bdevs[0].get_uuid());
    ASSERT_EQ(lvol_bdevs[1].get_name(), lvol_bdev_snapshot_uuid);

    ASSERT_EQ(lvol_bdevs[0].get_aliases()[0], "LVS1/11111111-2222-3333-4444-555555555555");
    ASSERT_EQ(lvol_bdevs[1].get_aliases()[0], "LVS1/09090909-0909-0909-0909-090909090909");

    // Driver specific
    spdk::model::drivers::LvolDriver lvol_driver = lvol_bdevs[0].get_driver_specific();
    ASSERT_EQ(lvol_driver.get_base_bdev(), "Nvme1n1");
    ASSERT_EQ(lvol_driver.get_lvol_store_uuid(), "413f21dc-1111-2222-3333-25e2161d69dc");
    ASSERT_TRUE(lvol_driver.is_clone());
    ASSERT_FALSE(lvol_driver.is_snapshot());
    ASSERT_FALSE(lvol_driver.is_thin_provision());
    ASSERT_TRUE((bool) lvol_driver.get_base_snapshot());
    ASSERT_EQ(lvol_driver.get_base_snapshot().value(), "09090909-0909-0909-0909-090909090909");

    spdk::model::drivers::LvolDriver lvol_driver_snapshot = lvol_bdevs[1].get_driver_specific();
    ASSERT_EQ(lvol_driver_snapshot.get_base_bdev(), "Nvme1n1");
    ASSERT_EQ(lvol_driver_snapshot.get_lvol_store_uuid(), "413f21dc-1111-2222-3333-25e2161d69dc");
    ASSERT_FALSE(lvol_driver_snapshot.is_clone());
    ASSERT_TRUE((bool) lvol_driver_snapshot.get_clones());
    std::vector<std::string> clones = lvol_driver_snapshot.get_clones().value();
    ASSERT_EQ(clones[0], "11111111-2222-3333-4444-555555555555");
    ASSERT_TRUE(lvol_driver_snapshot.is_snapshot());
    ASSERT_FALSE(lvol_driver_snapshot.is_thin_provision());
}


TEST_F(SnapshotLvolBdevTest, ValidMethodResultWithAlias) {

    std::string lvol_bdev_snapshot_uuid = spdk_api->snapshot_lvol_bdev("11111111-2222-3333-4444-555555555555",
                                                                       "09090909-0909-0909-0909-090909090909");

    ASSERT_EQ(lvol_bdev_snapshot_uuid, valid_lvol_bdev_snapshot_uuid);

    std::vector<spdk::model::Bdev> lvol_bdevs = spdk_api->get_bdevs();

    ASSERT_NE(lvol_bdevs[0].get_uuid(), lvol_bdevs[1].get_uuid());
    ASSERT_EQ(lvol_bdevs[0].get_uuid().value(), "09959197-1212-4343-8989-676711194545");
    ASSERT_EQ(lvol_bdevs[1].get_uuid().value(), "cc8d7fdf-7865-4d1f-9fc6-35da8e368670");

    ASSERT_EQ(lvol_bdevs[0].get_name(), "09959197-1212-4343-8989-676711194545");
    ASSERT_EQ(lvol_bdevs[0].get_name(), lvol_bdevs[0].get_uuid());
    ASSERT_EQ(lvol_bdevs[1].get_name(), lvol_bdev_snapshot_uuid);

    ASSERT_EQ(lvol_bdevs[0].get_aliases()[0], "LVS1/11111111-2222-3333-4444-555555555555");
    ASSERT_EQ(lvol_bdevs[1].get_aliases()[0], "LVS1/09090909-0909-0909-0909-090909090909");

    // Driver specific
    spdk::model::drivers::LvolDriver lvol_driver = lvol_bdevs[0].get_driver_specific();
    ASSERT_EQ(lvol_driver.get_base_bdev(), "Nvme1n1");
    ASSERT_EQ(lvol_driver.get_lvol_store_uuid(), "413f21dc-1111-2222-3333-25e2161d69dc");
    ASSERT_TRUE(lvol_driver.is_clone());
    ASSERT_FALSE(lvol_driver.is_snapshot());
    ASSERT_FALSE(lvol_driver.is_thin_provision());
    ASSERT_TRUE((bool) lvol_driver.get_base_snapshot());
    ASSERT_EQ(lvol_driver.get_base_snapshot().value(), "09090909-0909-0909-0909-090909090909");

    spdk::model::drivers::LvolDriver lvol_driver_snapshot = lvol_bdevs[1].get_driver_specific();
    ASSERT_EQ(lvol_driver_snapshot.get_base_bdev(), "Nvme1n1");
    ASSERT_EQ(lvol_driver_snapshot.get_lvol_store_uuid(), "413f21dc-1111-2222-3333-25e2161d69dc");
    ASSERT_FALSE(lvol_driver_snapshot.is_clone());
    ASSERT_TRUE((bool) lvol_driver_snapshot.get_clones());
    std::vector<std::string> clones = lvol_driver_snapshot.get_clones().value();
    ASSERT_EQ(clones[0], "11111111-2222-3333-4444-555555555555");
    ASSERT_TRUE(lvol_driver_snapshot.is_snapshot());
    ASSERT_FALSE(lvol_driver_snapshot.is_thin_provision());
}


TEST_F(SnapshotLvolBdevTest, ValidMethodResultWithInvalidLvolName) {

    std::string lvol_bdev_snapshot_uuid = spdk_api->snapshot_lvol_bdev("00000000-0000-0000-0000-000000000000",
                                                                       "abcabcab-bbbb-aaaa-cccc-abcdabcdabcd");

    ASSERT_NE(lvol_bdev_snapshot_uuid, valid_lvol_bdev_snapshot_uuid);
}

}


