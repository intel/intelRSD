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
 * @file destroy_lvol_bdev_test.cpp
 */

#include "spdk/spdk_api.hpp"

#include "connectors/destroy_lvol_bdev_connector.hpp"

#include "gtest/gtest.h"



class DestroyLvolBdevTest : public ::testing::Test {
protected:

    std::string valid_lvol_bdev_name = "LvolBdev";

    std::shared_ptr<spdk::SpdkApi> spdk_api = {};


    void SetUp() override {
        std::shared_ptr<spdk::DestroyLvolBdevConnector> stub_connector = std::make_shared<spdk::DestroyLvolBdevConnector>();
        std::shared_ptr<json_rpc::JsonRpcRequestInvoker> invoker = std::make_shared<json_rpc::JsonRpcRequestInvoker>();
        spdk_api = std::make_shared<::spdk::SpdkApi>(stub_connector, invoker);
    }


    DestroyLvolBdevTest();


    virtual ~DestroyLvolBdevTest();
};


DestroyLvolBdevTest::DestroyLvolBdevTest() {}


DestroyLvolBdevTest::~DestroyLvolBdevTest() {}


namespace testing {

TEST_F(DestroyLvolBdevTest, DestroyNotExistingLvolBdev) {

    ASSERT_FALSE(spdk_api->destroy_lvol_bdev("a1b2c3d4"));

    std::vector<spdk::model::Bdev> lvol_bdevs = spdk_api->get_bdevs("a1b2c3d4");

    ASSERT_TRUE(lvol_bdevs.empty());
}


TEST_F(DestroyLvolBdevTest, DestroyOneExistingLvolBdev) {

    std::vector<spdk::model::Bdev> lvol_bdevs = spdk_api->get_bdevs(valid_lvol_bdev_name);
    ASSERT_EQ(lvol_bdevs[0].get_name(), valid_lvol_bdev_name);

    ASSERT_TRUE(spdk_api->destroy_lvol_bdev(valid_lvol_bdev_name));

    lvol_bdevs = spdk_api->get_bdevs(valid_lvol_bdev_name);
    ASSERT_TRUE(lvol_bdevs.empty());
}


TEST_F(DestroyLvolBdevTest, DestroyDestroyedLvolBdev) {

    std::vector<spdk::model::Bdev> lvol_bdevs = spdk_api->get_bdevs(valid_lvol_bdev_name);
    ASSERT_EQ(lvol_bdevs[0].get_name(), valid_lvol_bdev_name);

    ASSERT_TRUE(spdk_api->destroy_lvol_bdev(valid_lvol_bdev_name));

    lvol_bdevs = spdk_api->get_bdevs(valid_lvol_bdev_name);
    ASSERT_TRUE(lvol_bdevs.empty());

    ASSERT_FALSE(spdk_api->destroy_lvol_bdev(valid_lvol_bdev_name));

    lvol_bdevs = spdk_api->get_bdevs(valid_lvol_bdev_name);
    ASSERT_TRUE(lvol_bdevs.empty());
}

}


