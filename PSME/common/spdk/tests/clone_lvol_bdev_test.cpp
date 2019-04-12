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
 * @file clone_lvol_bdev_test.cpp
 */

#include "spdk/spdk_api.hpp"

#include "connectors/clone_lvol_bdev_connector.hpp"

#include "gtest/gtest.h"

#include "spdk/model/drivers/lvol_driver.hpp"
#include "spdk/model/drivers/nvme_driver.hpp"



class CloneLvolBdevTest : public ::testing::Test {

protected:

    std::string valid_lvol_bdev_clone_uuid = "cc8d7fdf-7865-4d1f-9fc6-35da8e368670";

    std::shared_ptr<spdk::SpdkApi> spdk_api = {};


    void SetUp() override {
        std::shared_ptr<spdk::CloneLvolBdevConnector> stub_connector = std::make_shared<spdk::CloneLvolBdevConnector>();
        std::shared_ptr<json_rpc::JsonRpcRequestInvoker> invoker = std::make_shared<json_rpc::JsonRpcRequestInvoker>();
        spdk_api = std::make_shared<::spdk::SpdkApi>(stub_connector, invoker);
    }


    CloneLvolBdevTest();


    virtual ~CloneLvolBdevTest();

};


CloneLvolBdevTest::CloneLvolBdevTest() {}


CloneLvolBdevTest::~CloneLvolBdevTest() {}


namespace testing {

TEST_F(CloneLvolBdevTest, ValidMethodResultWithUuid) {

    std::string lvol_bdev_clone_uuid = spdk_api->clone_lvol_bdev("ccaaccaa-ccaa-ccaa-ccaa-ccaaccaaccaa",
                                                                 "09090909-0909-0909-0909-090909090909");

    ASSERT_EQ(lvol_bdev_clone_uuid, valid_lvol_bdev_clone_uuid);

    std::vector<spdk::model::Bdev> lvol_bdevs = spdk_api->get_bdevs();
    ASSERT_EQ(lvol_bdevs.size(), 2);

    ASSERT_NE(lvol_bdevs[0].get_uuid(), lvol_bdevs[1].get_uuid());
    ASSERT_EQ(lvol_bdevs[0].get_uuid().value(), "ccaaccaa-ccaa-ccaa-ccaa-ccaaccaaccaa");
    ASSERT_EQ(lvol_bdevs[1].get_uuid().value(), "cc8d7fdf-7865-4d1f-9fc6-35da8e368670");

    ASSERT_EQ(lvol_bdevs[0].get_name(), "ccaaccaa-ccaa-ccaa-ccaa-ccaaccaaccaa");
    ASSERT_EQ(lvol_bdevs[0].get_name(), lvol_bdevs[0].get_uuid());
    ASSERT_EQ(lvol_bdevs[1].get_name(), lvol_bdev_clone_uuid);

    ASSERT_EQ(lvol_bdevs[0].get_aliases()[0], "LVS1/11111111-2222-3333-4444-555555555555");
    ASSERT_EQ(lvol_bdevs[1].get_aliases()[0], "LVS1/09090909-0909-0909-0909-090909090909");

    // Driver specific
    ASSERT_TRUE((bool) lvol_bdevs[0].get_driver_specific<spdk::model::drivers::LvolDriver>());
    spdk::JsonOptional<spdk::model::drivers::LvolDriver> lvol_driver = lvol_bdevs[0].get_driver_specific<spdk::model::drivers::LvolDriver>();
    ASSERT_EQ(lvol_driver->get_base_bdev(), "Nvme1n1");
    ASSERT_EQ(lvol_driver->get_lvol_store_uuid(), "413f21dc-1111-2222-3333-25e2161d69dc");
    ASSERT_FALSE(lvol_driver->is_clone());
    ASSERT_TRUE(lvol_driver->is_snapshot());
    ASSERT_FALSE(lvol_driver->is_thin_provision());
    ASSERT_TRUE((bool) lvol_driver->get_clones());
    std::vector<std::string> clones = lvol_driver->get_clones().value();
    ASSERT_EQ(clones[0], "09090909-0909-0909-0909-090909090909");

    ASSERT_TRUE((bool) lvol_bdevs[1].get_driver_specific<spdk::model::drivers::LvolDriver>());
    spdk::JsonOptional<spdk::model::drivers::LvolDriver> lvol_driver_clone = lvol_bdevs[1].get_driver_specific<spdk::model::drivers::LvolDriver>();
    ASSERT_EQ(lvol_driver_clone->get_base_bdev(), "Nvme1n1");
    ASSERT_EQ(lvol_driver_clone->get_lvol_store_uuid(), "413f21dc-1111-2222-3333-25e2161d69dc");
    ASSERT_TRUE(lvol_driver_clone->is_clone());
    ASSERT_FALSE(lvol_driver_clone->is_snapshot());
    ASSERT_FALSE(lvol_driver_clone->is_thin_provision());

}


TEST_F(CloneLvolBdevTest, ValidMethodResultWithAlias) {

    std::string lvol_bdev_clone_uuid = spdk_api->clone_lvol_bdev("11111111-2222-3333-4444-555555555555",
                                                                 "09090909-0909-0909-0909-090909090909");

    ASSERT_EQ(lvol_bdev_clone_uuid, valid_lvol_bdev_clone_uuid);

    std::vector<spdk::model::Bdev> lvol_bdevs = spdk_api->get_bdevs();
    ASSERT_EQ(lvol_bdevs.size(), 2);

    ASSERT_NE(lvol_bdevs[0].get_uuid(), lvol_bdevs[1].get_uuid());
    ASSERT_EQ(lvol_bdevs[0].get_uuid().value(), "ccaaccaa-ccaa-ccaa-ccaa-ccaaccaaccaa");
    ASSERT_EQ(lvol_bdevs[1].get_uuid().value(), "cc8d7fdf-7865-4d1f-9fc6-35da8e368670");

    ASSERT_EQ(lvol_bdevs[0].get_name(), "ccaaccaa-ccaa-ccaa-ccaa-ccaaccaaccaa");
    ASSERT_EQ(lvol_bdevs[0].get_name(), lvol_bdevs[0].get_uuid());
    ASSERT_EQ(lvol_bdevs[1].get_name(), lvol_bdev_clone_uuid);

    ASSERT_EQ(lvol_bdevs[0].get_aliases()[0], "LVS1/11111111-2222-3333-4444-555555555555");
    ASSERT_EQ(lvol_bdevs[1].get_aliases()[0], "LVS1/09090909-0909-0909-0909-090909090909");

    // Driver specific
    ASSERT_TRUE((bool) lvol_bdevs[0].get_driver_specific<spdk::model::drivers::LvolDriver>());
    spdk::JsonOptional<spdk::model::drivers::LvolDriver> lvol_driver = lvol_bdevs[0].get_driver_specific<spdk::model::drivers::LvolDriver>();
    ASSERT_EQ(lvol_driver->get_base_bdev(), "Nvme1n1");
    ASSERT_EQ(lvol_driver->get_lvol_store_uuid(), "413f21dc-1111-2222-3333-25e2161d69dc");
    ASSERT_FALSE(lvol_driver->is_clone());
    ASSERT_TRUE(lvol_driver->is_snapshot());
    ASSERT_FALSE(lvol_driver->is_thin_provision());
    ASSERT_TRUE((bool) lvol_driver->get_clones());
    std::vector<std::string> clones = lvol_driver->get_clones().value();
    ASSERT_EQ(clones[0], "09090909-0909-0909-0909-090909090909");

    ASSERT_TRUE((bool) lvol_bdevs[1].get_driver_specific<spdk::model::drivers::LvolDriver>());
    spdk::JsonOptional<spdk::model::drivers::LvolDriver> lvol_driver_clone = lvol_bdevs[1].get_driver_specific<spdk::model::drivers::LvolDriver>();
    ASSERT_EQ(lvol_driver_clone->get_base_bdev(), "Nvme1n1");
    ASSERT_EQ(lvol_driver_clone->get_lvol_store_uuid(), "413f21dc-1111-2222-3333-25e2161d69dc");
    ASSERT_TRUE(lvol_driver_clone->is_clone());
    ASSERT_FALSE(lvol_driver_clone->is_snapshot());
    ASSERT_FALSE(lvol_driver_clone->is_thin_provision());
}


TEST_F(CloneLvolBdevTest, ValidMethodResultWithInvalidLvolName) {

    std::string lvol_bdev_clone_uuid = spdk_api->clone_lvol_bdev("00000000-0000-0000-0000-000000000000",
                                                                 "abcabcab-bbbb-aaaa-cccc-abcdabcdabcd");

    ASSERT_NE(lvol_bdev_clone_uuid, valid_lvol_bdev_clone_uuid);
}

}