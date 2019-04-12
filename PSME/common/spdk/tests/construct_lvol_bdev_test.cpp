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
 * @file construct_lvol_bdev_test.cpp
 */

#include "spdk/spdk_api.hpp"

#include "spdk/model/drivers/lvol_driver.hpp"

#include "connectors/construct_lvol_bdev_connector.hpp"

#include "gtest/gtest.h"



class ConstructLvolBdevTest : public ::testing::Test {
protected:

    std::string valid_lvol_bdev_alias = "12295197-p5j2-4f2d-a910-1513fb698cba";

    std::shared_ptr<spdk::SpdkApi> spdk_api = {};


    void SetUp() override {
        std::shared_ptr<spdk::ConstructLvolBdevConnector> stub_connector = std::make_shared<spdk::ConstructLvolBdevConnector>();
        std::shared_ptr<json_rpc::JsonRpcRequestInvoker> invoker = std::make_shared<json_rpc::JsonRpcRequestInvoker>();
        spdk_api = std::make_shared<::spdk::SpdkApi>(stub_connector, invoker);
    }


    ConstructLvolBdevTest();


    virtual ~ConstructLvolBdevTest();
};


ConstructLvolBdevTest::ConstructLvolBdevTest() {}


ConstructLvolBdevTest::~ConstructLvolBdevTest() {}

namespace testing {

TEST_F(ConstructLvolBdevTest, ValidMethodResultWithLvolUuidAndOptionalField) {

    std::string lvol_bdev_uuid = spdk_api->construct_lvol_bdev_by_store_uuid(valid_lvol_bdev_alias, 2,
                                                                             "a9295197-aaaa-4f2d-bbbb-1513fb69abcd",
                                                                             false,
                                                                             ::spdk::model::ClearMethod::WRITE_ZEROES);
    ASSERT_NE(valid_lvol_bdev_alias, lvol_bdev_uuid);

    // Lvol Bdev
    std::vector<spdk::model::Bdev> lvol_bdevs = spdk_api->get_bdevs(lvol_bdev_uuid);
    ASSERT_EQ(lvol_bdevs[0].get_name(), lvol_bdev_uuid);
    ASSERT_EQ(lvol_bdevs[0].get_product_name(), "Logical Volume");
    ASSERT_EQ(lvol_bdevs[0].get_block_size(), 2);
    ASSERT_EQ(lvol_bdevs[0].get_num_blocks(), 2501);
    // Lvol's uuid is the same as Lvol's name
    ASSERT_EQ(lvol_bdevs[0].get_uuid(), lvol_bdev_uuid);
    ASSERT_EQ(lvol_bdevs[0].get_aliases()[0], "LVS/" + valid_lvol_bdev_alias);
    ASSERT_FALSE(lvol_bdevs[0].is_claimed());

    // Driver specific
    spdk::model::drivers::LvolDriver lvol_driver = lvol_bdevs[0].get_driver_specific();
    ASSERT_EQ(lvol_driver.get_base_bdev(), "NVMe");
    ASSERT_EQ(lvol_driver.get_lvol_store_uuid(), "a9295197-aaaa-4f2d-bbbb-1513fb69abcd");
    ASSERT_FALSE(lvol_driver.is_clone());
    ASSERT_FALSE(lvol_driver.is_snapshot());
    ASSERT_FALSE(lvol_driver.is_thin_provision());

    // Supported IO types
    spdk::model::SupportedIoTypes supported_io_types = lvol_bdevs[0].get_supported_io_types();
    ASSERT_TRUE(supported_io_types.is_reset());
    ASSERT_TRUE(supported_io_types.is_nvme_admin());
    ASSERT_TRUE(supported_io_types.is_unmap());
    ASSERT_TRUE(supported_io_types.is_read());
    ASSERT_TRUE(supported_io_types.is_write_zeroes());
    ASSERT_TRUE(supported_io_types.is_write());
    ASSERT_FALSE(supported_io_types.is_flush());
    ASSERT_FALSE(supported_io_types.is_nvme_io());

    ASSERT_TRUE((bool) lvol_bdevs[0].get_qos_ios_per_sec());
    ASSERT_EQ(lvol_bdevs[0].get_qos_ios_per_sec().value(), 1);

}


TEST_F(ConstructLvolBdevTest, ValidMethodResultWithLvolUuid) {

    std::string lvol_bdev_uuid = spdk_api->construct_lvol_bdev_by_store_uuid(valid_lvol_bdev_alias, 4,
                                                                             "10295497-a5j2-4g2d-1921-59513abc98a41",
                                                                             false,
                                                                             ::spdk::model::ClearMethod::WRITE_ZEROES);
    ASSERT_NE(valid_lvol_bdev_alias, lvol_bdev_uuid);

    // Lvol Bdev
    std::vector<spdk::model::Bdev> lvol_bdevs = spdk_api->get_bdevs(lvol_bdev_uuid);
    ASSERT_EQ(lvol_bdevs[0].get_name(), lvol_bdev_uuid);
    ASSERT_EQ(lvol_bdevs[0].get_product_name(), "Logical Volume");
    ASSERT_EQ(lvol_bdevs[0].get_block_size(), 4);
    ASSERT_EQ(lvol_bdevs[0].get_num_blocks(), 2501);
    // Lvol's uuid is the same as Lvol's name
    ASSERT_EQ(lvol_bdevs[0].get_uuid(), lvol_bdev_uuid);
    ASSERT_EQ(lvol_bdevs[0].get_aliases()[0], "LVS/" + valid_lvol_bdev_alias);
    ASSERT_FALSE(lvol_bdevs[0].is_claimed());

    // Driver specific
    spdk::model::drivers::LvolDriver lvol_driver = lvol_bdevs[0].get_driver_specific();
    ASSERT_EQ(lvol_driver.get_base_bdev(), "NVMe");
    ASSERT_EQ(lvol_driver.get_lvol_store_uuid(), "10295497-a5j2-4g2d-1921-59513abc98a41");
    ASSERT_FALSE(lvol_driver.is_clone());
    ASSERT_FALSE(lvol_driver.is_snapshot());
    ASSERT_FALSE(lvol_driver.is_thin_provision());

    // Supported IO types
    spdk::model::SupportedIoTypes supported_io_types = lvol_bdevs[0].get_supported_io_types();
    ASSERT_TRUE(supported_io_types.is_reset());
    ASSERT_TRUE(supported_io_types.is_nvme_admin());
    ASSERT_TRUE(supported_io_types.is_unmap());
    ASSERT_TRUE(supported_io_types.is_read());
    ASSERT_TRUE(supported_io_types.is_write_zeroes());
    ASSERT_TRUE(supported_io_types.is_write());
    ASSERT_FALSE(supported_io_types.is_flush());
    ASSERT_FALSE(supported_io_types.is_nvme_io());

    ASSERT_TRUE((bool) lvol_bdevs[0].get_qos_ios_per_sec());
    ASSERT_EQ(lvol_bdevs[0].get_qos_ios_per_sec().value(), 1);

}


TEST_F(ConstructLvolBdevTest, ValidMethodResultWithLvolNameAndOptionalField) {

    std::string lvol_bdev_uuid = spdk_api->construct_lvol_bdev_by_store_name(valid_lvol_bdev_alias, 1013,
                                                                             "NVMeStore", true,
                                                                             ::spdk::model::ClearMethod::WRITE_ZEROES);
    ASSERT_NE(valid_lvol_bdev_alias, lvol_bdev_uuid);

    // Lvol Bdev
    std::vector<spdk::model::Bdev> lvol_bdevs = spdk_api->get_bdevs(lvol_bdev_uuid);
    ASSERT_EQ(lvol_bdevs[0].get_name(), lvol_bdev_uuid);
    ASSERT_EQ(lvol_bdevs[0].get_product_name(), "Logical Volume");
    ASSERT_EQ(lvol_bdevs[0].get_block_size(), 1013);
    ASSERT_EQ(lvol_bdevs[0].get_num_blocks(), 2501);
    // Lvol's uuid is the same as Lvol's name
    ASSERT_EQ(lvol_bdevs[0].get_uuid(), lvol_bdev_uuid);
    ASSERT_EQ(lvol_bdevs[0].get_aliases()[0], "NVMeStore/" + valid_lvol_bdev_alias);
    ASSERT_FALSE(lvol_bdevs[0].is_claimed());

    // Driver specific
    spdk::model::drivers::LvolDriver lvol_driver = lvol_bdevs[0].get_driver_specific();
    ASSERT_EQ(lvol_driver.get_base_bdev(), "NVMe");
    ASSERT_EQ(lvol_driver.get_lvol_store_uuid(), "10000000-2000-3000-4000-abcdeabcdeabc");
    ASSERT_FALSE(lvol_driver.is_clone());
    ASSERT_FALSE(lvol_driver.is_snapshot());
    ASSERT_TRUE(lvol_driver.is_thin_provision());

    // Supported IO types
    spdk::model::SupportedIoTypes supported_io_types = lvol_bdevs[0].get_supported_io_types();
    ASSERT_TRUE(supported_io_types.is_reset());
    ASSERT_TRUE(supported_io_types.is_nvme_admin());
    ASSERT_TRUE(supported_io_types.is_unmap());
    ASSERT_TRUE(supported_io_types.is_read());
    ASSERT_TRUE(supported_io_types.is_write_zeroes());
    ASSERT_TRUE(supported_io_types.is_write());
    ASSERT_FALSE(supported_io_types.is_flush());
    ASSERT_FALSE(supported_io_types.is_nvme_io());

    ASSERT_TRUE((bool) lvol_bdevs[0].get_qos_ios_per_sec());
    ASSERT_EQ(lvol_bdevs[0].get_qos_ios_per_sec().value(), 1);

}


TEST_F(ConstructLvolBdevTest, ValidMethodResultWithLvolName) {

    std::string lvol_bdev_uuid = spdk_api->construct_lvol_bdev_by_store_name(valid_lvol_bdev_alias, 12,
                                                                             "NVMeStore2", false,
                                                                             ::spdk::model::ClearMethod::WRITE_ZEROES);
    ASSERT_NE(valid_lvol_bdev_alias, lvol_bdev_uuid);

    // Lvol Bdev
    std::vector<spdk::model::Bdev> lvol_bdevs = spdk_api->get_bdevs(lvol_bdev_uuid);
    ASSERT_EQ(lvol_bdevs[0].get_name(), lvol_bdev_uuid);
    ASSERT_EQ(lvol_bdevs[0].get_product_name(), "Logical Volume");
    ASSERT_EQ(lvol_bdevs[0].get_block_size(), 12);
    ASSERT_EQ(lvol_bdevs[0].get_num_blocks(), 2501);
    // Lvol's uuid is the same as Lvol's name
    ASSERT_EQ(lvol_bdevs[0].get_uuid(), lvol_bdev_uuid);
    ASSERT_EQ(lvol_bdevs[0].get_aliases()[0], "NVMeStore2/" + valid_lvol_bdev_alias);
    ASSERT_FALSE(lvol_bdevs[0].is_claimed());

    // Driver specific
    spdk::model::drivers::LvolDriver lvol_driver = lvol_bdevs[0].get_driver_specific();
    ASSERT_EQ(lvol_driver.get_base_bdev(), "NVMe");
    ASSERT_EQ(lvol_driver.get_lvol_store_uuid(), "10000000-2000-3000-4000-abcdeabcdeabc");
    ASSERT_FALSE(lvol_driver.is_clone());
    ASSERT_FALSE(lvol_driver.is_snapshot());
    ASSERT_FALSE(lvol_driver.is_thin_provision());

    // Supported IO types
    spdk::model::SupportedIoTypes supported_io_types = lvol_bdevs[0].get_supported_io_types();
    ASSERT_TRUE(supported_io_types.is_reset());
    ASSERT_TRUE(supported_io_types.is_nvme_admin());
    ASSERT_TRUE(supported_io_types.is_unmap());
    ASSERT_TRUE(supported_io_types.is_read());
    ASSERT_TRUE(supported_io_types.is_write_zeroes());
    ASSERT_TRUE(supported_io_types.is_write());
    ASSERT_FALSE(supported_io_types.is_flush());
    ASSERT_FALSE(supported_io_types.is_nvme_io());

    ASSERT_TRUE((bool) lvol_bdevs[0].get_qos_ios_per_sec());
    ASSERT_EQ(lvol_bdevs[0].get_qos_ios_per_sec().value(), 1);

}

}


