/*!
 * @brief Get Bdevs test.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file get_bdevs_test.cpp
 */

#include "spdk/spdk_api.hpp"
#include "spdk/model/drivers/nvme_driver.hpp"
#include "spdk/model/drivers/lvol_driver.hpp"

#include "connectors/get_bdevs_connector.hpp"

#include "gtest/gtest.h"



class GetBdevsTest : public ::testing::Test {
protected:
    static constexpr int BDEVS_COUNT = 5;
    int m_current_bdev{};

    std::string valid_bdev_name[BDEVS_COUNT] = {
        "NVMe1n1",
        "Malloc",
        "76f0d58f-4c03-4d97-9176-cb33057b6f57",
        "f8110801-9df4-4ea7-84cb-82a30e4ed56a",
        "4f04cb62-8280-404c-a0ef-2383be8d81db"
    };

    std::string valid_bdev_product_name[BDEVS_COUNT] = {
        "NVMe disk",
        "Malloc Disk",
        "Logical Volume",
        "Logical Volume",
        "Logical Volume"
    };

    std::uint32_t valid_bdev_block_size[BDEVS_COUNT] = {
        51205,
        512,
        500,
        4096,
        4096
    };

    std::uint64_t valid_bdev_num_blocks[BDEVS_COUNT] = {
        781422768,
        20480,
        2501,
        3014656,
        3014656
    };

    std::uint64_t valid_bdev_qos_ios_per_sec[BDEVS_COUNT] = {
        10,
        5,
        1,
        0,
        0,
    };

    std::vector<std::string> valid_bdev_aliases[BDEVS_COUNT] = {
        {},
        {},
        {"LVS1/76f0d58f-4c03-4d97-9176-cb33057b6f57"},
        {"LVS1/Clone-1"},
        {"LVS1/MySnapshot"}
    };

    spdk::JsonOptional<std::string> valid_bdev_uuid[BDEVS_COUNT] = {
        {},
        {},
        {"76f0d58f-4c03-4d97-9176-cb33057b6f57"},
        {"f8110801-9df4-4ea7-84cb-82a30e4ed56a"},
        {"4f04cb62-8280-404c-a0ef-2383be8d81db"}
    };

    std::shared_ptr<spdk::SpdkApi> spdk_api = {};


    void SetUp() override {
        std::shared_ptr<spdk::GetBdevsConnector> stub_connector = std::make_shared<spdk::GetBdevsConnector>();
        std::shared_ptr<json_rpc::JsonRpcRequestInvoker> invoker = std::make_shared<json_rpc::JsonRpcRequestInvoker>();
        spdk_api = std::make_shared<::spdk::SpdkApi>(stub_connector, invoker);
    }


    GetBdevsTest();


    virtual ~GetBdevsTest();
};

const int GetBdevsTest::BDEVS_COUNT;


GetBdevsTest::GetBdevsTest() {

}


GetBdevsTest::~GetBdevsTest() {}


namespace testing {

TEST_F(GetBdevsTest, ValidMethodResultForNVMeBdev) {
    std::vector<spdk::model::Bdev> bdevs = spdk_api->get_bdevs(valid_bdev_name[0]);
    ASSERT_EQ(bdevs.size(), 1);

    spdk::model::Bdev nvme_bdev = bdevs[0];

    ASSERT_EQ(nvme_bdev.get_name(), valid_bdev_name[0]);
    ASSERT_EQ(nvme_bdev.get_uuid(), valid_bdev_uuid[0]);
    ASSERT_EQ(nvme_bdev.get_product_name(), valid_bdev_product_name[0]);
    ASSERT_EQ(nvme_bdev.get_aliases(), valid_bdev_aliases[0]);
    ASSERT_EQ(nvme_bdev.get_block_size(), valid_bdev_block_size[0]);
    ASSERT_EQ(nvme_bdev.get_num_blocks(), valid_bdev_num_blocks[0]);
    ASSERT_TRUE(nvme_bdev.is_claimed());

    ASSERT_EQ(nvme_bdev.get_assigned_rate_limits().value().get_rw_mbytes_per_sec().value(), 100);
    ASSERT_EQ(nvme_bdev.get_assigned_rate_limits().value().get_r_mbytes_per_sec().value(), 50);
    ASSERT_EQ(nvme_bdev.get_assigned_rate_limits().value().get_w_mbytes_per_sec().value(), 50);
    ASSERT_EQ(nvme_bdev.get_assigned_rate_limits().value().get_rw_ios_per_sec().value(), 20000);

    ASSERT_FALSE((bool) nvme_bdev.get_driver_specific<spdk::model::drivers::LvolDriver>());
    ASSERT_TRUE((bool) nvme_bdev.get_driver_specific<spdk::model::drivers::NvmeDriver>());

    auto nvme_driver = nvme_bdev.get_driver_specific<spdk::model::drivers::NvmeDriver>();
    spdk::model::drivers::NvmeDriver driver = nvme_driver.value();

    ASSERT_EQ(driver.get_trid().get_trtype(), "PCIe");
    ASSERT_EQ(driver.get_trid().get_traddr(), "0000:43:00.0");
    ASSERT_EQ(driver.get_trid().get_trsvcid(), spdk::JsonOptional<std::string>());
    ASSERT_EQ(driver.get_trid().get_adrfam(), spdk::JsonOptional<std::string>());

    ASSERT_EQ(driver.get_ns_data().get_id(), 1);
    ASSERT_EQ(driver.get_pci_address(), "0000:43:00.0");
    ASSERT_EQ(driver.get_version().get_version(), "1.0");

    ASSERT_EQ(driver.get_ctrlr_data().get_firmware_revision(), "version1509");
    ASSERT_EQ(driver.get_ctrlr_data().get_serial_number(), "BTSPJ1310");
    ASSERT_EQ(driver.get_ctrlr_data().get_oacs().get_ns_manage(), 0);
    ASSERT_EQ(driver.get_ctrlr_data().get_oacs().get_security(), 0);
    ASSERT_EQ(driver.get_ctrlr_data().get_oacs().get_firmware(), 1);
    ASSERT_EQ(driver.get_ctrlr_data().get_oacs().get_format(), 1);
    ASSERT_EQ(driver.get_ctrlr_data().get_vendor_id(), "1234");
    ASSERT_EQ(driver.get_ctrlr_data().get_model_number(), "MN 0987");

    ASSERT_EQ(driver.get_csts().get_rdy(), 1);
    ASSERT_EQ(driver.get_csts().get_cfs(), 0);

    ASSERT_TRUE(nvme_bdev.get_supported_io_types().is_unmap());
    ASSERT_TRUE(nvme_bdev.get_supported_io_types().is_nvme_admin());
    ASSERT_EQ(nvme_bdev.get_qos_ios_per_sec(), valid_bdev_qos_ios_per_sec[0]);
}


TEST_F(GetBdevsTest, ValidMethodResultForMallocBdev) {

    std::vector<spdk::model::Bdev> bdevs = spdk_api->get_bdevs(valid_bdev_name[1]);
    ASSERT_EQ(bdevs.size(), 1);

    spdk::model::Bdev malloc_bdev = bdevs[0];
    ASSERT_EQ(malloc_bdev.get_name(), valid_bdev_name[1]);
    ASSERT_EQ(malloc_bdev.get_uuid(), valid_bdev_uuid[1]);
    ASSERT_EQ(malloc_bdev.get_product_name(), valid_bdev_product_name[1]);
    ASSERT_EQ(malloc_bdev.get_aliases(), valid_bdev_aliases[1]);
    ASSERT_EQ(malloc_bdev.get_block_size(), valid_bdev_block_size[1]);
    ASSERT_EQ(malloc_bdev.get_num_blocks(), valid_bdev_num_blocks[1]);
    ASSERT_FALSE(malloc_bdev.is_claimed());

    ASSERT_EQ(malloc_bdev.get_driver_specific(), json::Json::object());
    ASSERT_FALSE((bool) malloc_bdev.get_driver_specific<spdk::model::drivers::LvolDriver>());
    ASSERT_FALSE((bool) malloc_bdev.get_driver_specific<spdk::model::drivers::NvmeDriver>());

    ASSERT_EQ(malloc_bdev.get_assigned_rate_limits().value().get_rw_mbytes_per_sec().value(), 0);
    ASSERT_EQ(malloc_bdev.get_assigned_rate_limits().value().get_r_mbytes_per_sec().value(), 0);
    ASSERT_EQ(malloc_bdev.get_assigned_rate_limits().value().get_w_mbytes_per_sec().value(), 0);
    ASSERT_EQ(malloc_bdev.get_assigned_rate_limits().value().get_rw_ios_per_sec().value(), 0);

    ASSERT_TRUE(malloc_bdev.get_supported_io_types().is_unmap());
    ASSERT_FALSE(malloc_bdev.get_supported_io_types().is_nvme_admin());
    ASSERT_EQ(malloc_bdev.get_qos_ios_per_sec(), valid_bdev_qos_ios_per_sec[1]);
}


TEST_F(GetBdevsTest, ValidMethodResultForLvolBdev) {
    std::vector<spdk::model::Bdev> bdevs = spdk_api->get_bdevs(valid_bdev_name[2]);
    ASSERT_EQ(bdevs.size(), 1);

    spdk::model::Bdev lvol_bdev = bdevs[0];
    ASSERT_EQ(lvol_bdev.get_name(), valid_bdev_name[2]);
    ASSERT_EQ(lvol_bdev.get_uuid(), valid_bdev_uuid[2]);
    ASSERT_EQ(lvol_bdev.get_product_name(), valid_bdev_product_name[2]);
    ASSERT_EQ(lvol_bdev.get_aliases(), valid_bdev_aliases[2]);
    ASSERT_EQ(lvol_bdev.get_block_size(), valid_bdev_block_size[2]);
    ASSERT_EQ(lvol_bdev.get_num_blocks(), valid_bdev_num_blocks[2]);
    ASSERT_FALSE(lvol_bdev.is_claimed());

    ASSERT_TRUE((bool) lvol_bdev.get_driver_specific<spdk::model::drivers::LvolDriver>());
    ASSERT_FALSE((bool) lvol_bdev.get_driver_specific<spdk::model::drivers::NvmeDriver>());

    auto lvol_driver = lvol_bdev.get_driver_specific<spdk::model::drivers::LvolDriver>();
    spdk::model::drivers::LvolDriver driver = lvol_driver.value();
    ASSERT_EQ(driver.get_base_bdev(), "NVMe1n1");
    ASSERT_FALSE(driver.is_clone());
    ASSERT_FALSE(driver.is_snapshot());
    ASSERT_EQ(driver.get_lvol_store_uuid(), "413f21dc-0246-4d63-94e2-25e2161d69dc");
    ASSERT_FALSE(driver.is_thin_provision());
    ASSERT_FALSE((bool) driver.get_clones());
    ASSERT_FALSE((bool) driver.get_base_snapshot());

    ASSERT_TRUE(lvol_bdev.get_supported_io_types().is_unmap());
    ASSERT_FALSE(lvol_bdev.get_supported_io_types().is_nvme_admin());
    ASSERT_EQ(lvol_bdev.get_qos_ios_per_sec(), valid_bdev_qos_ios_per_sec[2]);

    ASSERT_FALSE(lvol_bdev.get_assigned_rate_limits());
}


TEST_F(GetBdevsTest, ValidMethodResultForLvolClone) {
    m_current_bdev = 3;
    std::vector<spdk::model::Bdev> bdevs = spdk_api->get_bdevs(valid_bdev_name[m_current_bdev]);
    ASSERT_EQ(bdevs.size(), 1);

    spdk::model::Bdev clone_bdev = bdevs[0];
    ASSERT_EQ(clone_bdev.get_name(), valid_bdev_name[m_current_bdev]);
    ASSERT_EQ(clone_bdev.get_uuid(), valid_bdev_uuid[m_current_bdev]);
    ASSERT_EQ(clone_bdev.get_product_name(), valid_bdev_product_name[m_current_bdev]);
    ASSERT_EQ(clone_bdev.get_aliases(), valid_bdev_aliases[m_current_bdev]);
    ASSERT_EQ(clone_bdev.get_block_size(), valid_bdev_block_size[m_current_bdev]);
    ASSERT_EQ(clone_bdev.get_num_blocks(), valid_bdev_num_blocks[m_current_bdev]);
    ASSERT_EQ(clone_bdev.get_qos_ios_per_sec(), valid_bdev_qos_ios_per_sec[m_current_bdev]);
    ASSERT_FALSE(clone_bdev.is_claimed());

    ASSERT_TRUE((bool) clone_bdev.get_driver_specific<spdk::model::drivers::LvolDriver>());
    ASSERT_FALSE((bool) clone_bdev.get_driver_specific<spdk::model::drivers::NvmeDriver>());

    auto lvol_driver = clone_bdev.get_driver_specific<spdk::model::drivers::LvolDriver>();
    spdk::model::drivers::LvolDriver driver = lvol_driver.value();
    ASSERT_EQ(driver.get_base_bdev(), "NVMe1n1");
    ASSERT_TRUE(driver.is_clone());
    ASSERT_FALSE(driver.is_snapshot());
    ASSERT_EQ(driver.get_lvol_store_uuid(), "0a86d76c-1f49-4616-b3e0-c36b1761ed28");
    ASSERT_TRUE(driver.is_thin_provision());
    ASSERT_FALSE((bool) driver.get_clones());
    ASSERT_TRUE((bool) driver.get_base_snapshot());
    ASSERT_EQ(driver.get_base_snapshot().value(), "MySnapshot");

    ASSERT_EQ(clone_bdev.get_assigned_rate_limits().value().get_rw_mbytes_per_sec().value(), 100);
    ASSERT_EQ(clone_bdev.get_assigned_rate_limits().value().get_r_mbytes_per_sec().value(), 50);
    ASSERT_EQ(clone_bdev.get_assigned_rate_limits().value().get_w_mbytes_per_sec().value(), 50);
    ASSERT_EQ(clone_bdev.get_assigned_rate_limits().value().get_rw_ios_per_sec().value(), 20000);

    ASSERT_TRUE(clone_bdev.get_supported_io_types().is_unmap());
    ASSERT_FALSE(clone_bdev.get_supported_io_types().is_nvme_admin());
    ASSERT_FALSE(clone_bdev.get_supported_io_types().is_flush());
    ASSERT_FALSE(clone_bdev.get_supported_io_types().is_nvme_io());
    ASSERT_TRUE(clone_bdev.get_supported_io_types().is_read());
    ASSERT_TRUE(clone_bdev.get_supported_io_types().is_reset());
    ASSERT_TRUE(clone_bdev.get_supported_io_types().is_write());
    ASSERT_TRUE(clone_bdev.get_supported_io_types().is_write_zeroes());
}


TEST_F(GetBdevsTest, ValidMethodResultForLvolSnapashot) {
    m_current_bdev = 4;
    std::vector<spdk::model::Bdev> bdevs = spdk_api->get_bdevs(valid_bdev_name[m_current_bdev]);
    ASSERT_EQ(bdevs.size(), 1);

    spdk::model::Bdev snapshot_bdev = bdevs[0];
    ASSERT_EQ(snapshot_bdev.get_name(), valid_bdev_name[m_current_bdev]);
    ASSERT_EQ(snapshot_bdev.get_uuid(), valid_bdev_uuid[m_current_bdev]);
    ASSERT_EQ(snapshot_bdev.get_product_name(), valid_bdev_product_name[m_current_bdev]);
    ASSERT_EQ(snapshot_bdev.get_aliases(), valid_bdev_aliases[m_current_bdev]);
    ASSERT_EQ(snapshot_bdev.get_block_size(), valid_bdev_block_size[m_current_bdev]);
    ASSERT_EQ(snapshot_bdev.get_num_blocks(), valid_bdev_num_blocks[m_current_bdev]);
    ASSERT_FALSE(snapshot_bdev.is_claimed());

    ASSERT_TRUE((bool) snapshot_bdev.get_driver_specific<spdk::model::drivers::LvolDriver>());
    ASSERT_FALSE((bool) snapshot_bdev.get_driver_specific<spdk::model::drivers::NvmeDriver>());

    auto lvol_driver = snapshot_bdev.get_driver_specific<spdk::model::drivers::LvolDriver>();
    spdk::model::drivers::LvolDriver driver = lvol_driver.value();
    ASSERT_EQ(driver.get_base_bdev(), "NVMe1n1");
    ASSERT_FALSE(driver.is_clone());
    ASSERT_TRUE(driver.is_snapshot());
    ASSERT_EQ(driver.get_lvol_store_uuid(), "0a86d76c-1f49-4616-b3e0-c36b1761ed28");
    ASSERT_FALSE(driver.is_thin_provision());
    ASSERT_TRUE((bool) driver.get_clones());
    ASSERT_NO_THROW(driver.get_clones().value().at(0));
    ASSERT_EQ(driver.get_clones().value().at(0), "Clone-1");
    ASSERT_FALSE((bool) driver.get_base_snapshot());

    ASSERT_EQ(snapshot_bdev.get_assigned_rate_limits().value().get_rw_mbytes_per_sec().value(), 100);
    ASSERT_EQ(snapshot_bdev.get_assigned_rate_limits().value().get_r_mbytes_per_sec().value(), 50);
    ASSERT_EQ(snapshot_bdev.get_assigned_rate_limits().value().get_w_mbytes_per_sec().value(), 50);
    ASSERT_EQ(snapshot_bdev.get_assigned_rate_limits().value().get_rw_ios_per_sec().value(), 20000);

    ASSERT_FALSE(snapshot_bdev.get_supported_io_types().is_unmap());
    ASSERT_FALSE(snapshot_bdev.get_supported_io_types().is_nvme_admin());
    ASSERT_FALSE(snapshot_bdev.get_supported_io_types().is_flush());
    ASSERT_FALSE(snapshot_bdev.get_supported_io_types().is_nvme_io());
    ASSERT_TRUE(snapshot_bdev.get_supported_io_types().is_read());
    ASSERT_TRUE(snapshot_bdev.get_supported_io_types().is_reset());
    ASSERT_FALSE(snapshot_bdev.get_supported_io_types().is_write());
    ASSERT_FALSE(snapshot_bdev.get_supported_io_types().is_write_zeroes());
    ASSERT_EQ(snapshot_bdev.get_qos_ios_per_sec(), valid_bdev_qos_ios_per_sec[m_current_bdev]);
}


TEST_F(GetBdevsTest, ValidMethodResultWithoutOptionalField) {
    std::vector<spdk::model::Bdev> bdevs = spdk_api->get_bdevs();

    ASSERT_EQ(bdevs.size(), BDEVS_COUNT);
    for (unsigned int i = 0; i < BDEVS_COUNT; i++) {
        ASSERT_EQ(bdevs[i].get_name(), valid_bdev_name[i]);
        ASSERT_EQ(bdevs[i].get_uuid(), valid_bdev_uuid[i]);
        ASSERT_EQ(bdevs[i].get_product_name(), valid_bdev_product_name[i]);
        ASSERT_EQ(bdevs[i].get_aliases(), valid_bdev_aliases[i]);
        ASSERT_EQ(bdevs[i].get_block_size(), valid_bdev_block_size[i]);
        ASSERT_EQ(bdevs[i].get_num_blocks(), valid_bdev_num_blocks[i]);
        ASSERT_EQ(bdevs[i].get_qos_ios_per_sec(), valid_bdev_qos_ios_per_sec[i]);
    }
}


TEST_F(GetBdevsTest, ValidMethodResultWithIncorrectArguments) {
    std::vector<spdk::model::Bdev> bdevs = spdk_api->get_bdevs("Invalid");
    ASSERT_EQ(bdevs.size(), 0);
}

}
