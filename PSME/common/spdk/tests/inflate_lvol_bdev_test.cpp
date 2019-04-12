/*!
 * @copyright
 * Copyright (c) 2019 Intel Corporation
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
 * @file inflate_lvol_bdev_test.cpp
 */

#include "spdk/spdk_api.hpp"

#include "connectors/inflate_lvol_bdev_connector.hpp"

#include "gtest/gtest.h"

#include "spdk/model/drivers/lvol_driver.hpp"
#include "spdk/model/drivers/nvme_driver.hpp"



class InflateLvolBdevTest : public ::testing::Test {

protected:

    std::shared_ptr<spdk::SpdkApi> spdk_api = {};


    void SetUp() override {
        std::shared_ptr<spdk::InflateLvolBdevConnector> stub_connector = std::make_shared<spdk::InflateLvolBdevConnector>();
        std::shared_ptr<json_rpc::JsonRpcRequestInvoker> invoker = std::make_shared<json_rpc::JsonRpcRequestInvoker>();
        spdk_api = std::make_shared<::spdk::SpdkApi>(stub_connector, invoker);
    }


    InflateLvolBdevTest();


    virtual ~InflateLvolBdevTest();

};


InflateLvolBdevTest::InflateLvolBdevTest() {}


InflateLvolBdevTest::~InflateLvolBdevTest() {}


namespace testing {

TEST_F(InflateLvolBdevTest, ValidMethodResultWithInflatedBdev) {

    std::vector<spdk::model::Bdev> lvol_bdevs = spdk_api->get_bdevs();

    ASSERT_EQ(lvol_bdevs.size(), 1);

    ASSERT_EQ(lvol_bdevs[0].get_name(), "11111111-2222-3333-4444-555555555555");
    ASSERT_EQ(lvol_bdevs[0].get_name(), lvol_bdevs[0].get_uuid());

    // Driver specific
    ASSERT_TRUE((bool) lvol_bdevs[0].get_driver_specific<spdk::model::drivers::LvolDriver>());
    spdk::JsonOptional<spdk::model::drivers::LvolDriver> lvol_driver = lvol_bdevs[0].get_driver_specific<spdk::model::drivers::LvolDriver>();
    ASSERT_EQ(lvol_driver->get_base_bdev(), "Nvme1n1");
    ASSERT_TRUE(lvol_driver->is_thin_provision());

    // Inflate
    bool inflated = spdk_api->inflate_lvol_bdev("11111111-2222-3333-4444-555555555555");
    ASSERT_TRUE(inflated);

    std::vector<spdk::model::Bdev> inflated_lvol_bdevs = spdk_api->get_bdevs();

    ASSERT_TRUE((bool) lvol_bdevs[0].get_driver_specific<spdk::model::drivers::LvolDriver>());
    spdk::JsonOptional<spdk::model::drivers::LvolDriver> inflated_lvol_driver = inflated_lvol_bdevs[0].get_driver_specific<spdk::model::drivers::LvolDriver>();
    ASSERT_FALSE(inflated_lvol_driver->is_thin_provision());

}


TEST_F(InflateLvolBdevTest, ValidMethodResultWithoutInflatedBdev) {

    bool inflated = spdk_api->inflate_lvol_bdev("ccaaccaa-ccaa-ccaa-ccaa-ccaaccaaccaa");

    ASSERT_FALSE(inflated);
}

}