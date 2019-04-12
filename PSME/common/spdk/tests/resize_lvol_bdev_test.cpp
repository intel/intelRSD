/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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
 *
 * @file resize_lvol_bdev_test.cpp
 */

#include "spdk/spdk_api.hpp"
#include "gtest/gtest.h"

#include "connectors/resize_lvol_bdev_connector.hpp"



class ResizeLvolBdevTest : public ::testing::Test {
protected:

    std::shared_ptr<spdk::SpdkApi> spdk_api = {};


    void SetUp() override {
        std::shared_ptr<spdk::ResizeLvolBdevConnector> stub_connector = std::make_shared<spdk::ResizeLvolBdevConnector>();
        auto invoker = std::make_shared<json_rpc::JsonRpcRequestInvoker>();
        spdk_api = std::make_shared<::spdk::SpdkApi>(stub_connector, invoker);
    }


    ResizeLvolBdevTest() = default;


    virtual ~ResizeLvolBdevTest() = default;
};


namespace testing {

TEST_F(ResizeLvolBdevTest, VolumeExtension) {
    static const std::int64_t requested_bytes = 1000000000000;

    auto bdevs = spdk_api->get_bdevs(spdk::ResizeLvolBdevConnector::LVOL_UUID);
    ASSERT_FALSE(bdevs.empty());

    auto lvol = bdevs.front();
    ASSERT_EQ(lvol.get_name(), spdk::ResizeLvolBdevConnector::LVOL_UUID);
    ASSERT_LT(lvol.get_num_blocks(), requested_bytes);

    bool result = spdk_api->resize_lvol_bdev(spdk::ResizeLvolBdevConnector::LVOL_UUID, requested_bytes);
    ASSERT_TRUE(result);

    bdevs = spdk_api->get_bdevs(spdk::ResizeLvolBdevConnector::LVOL_UUID);
    ASSERT_FALSE(bdevs.empty());

    lvol = bdevs.front();
    ASSERT_EQ(lvol.get_name(), spdk::ResizeLvolBdevConnector::LVOL_UUID);
    ASSERT_EQ(lvol.get_num_blocks(), requested_bytes);
}


TEST_F(ResizeLvolBdevTest, VolumeReducing) {
    static const std::int64_t requested_bytes = 1000;

    auto bdevs = spdk_api->get_bdevs(spdk::ResizeLvolBdevConnector::LVOL_UUID);
    ASSERT_FALSE(bdevs.empty());

    auto lvol = bdevs.front();
    ASSERT_EQ(lvol.get_name(), spdk::ResizeLvolBdevConnector::LVOL_UUID);
    ASSERT_GT(lvol.get_num_blocks(), requested_bytes);

    bool result = spdk_api->resize_lvol_bdev(spdk::ResizeLvolBdevConnector::LVOL_UUID, requested_bytes);
    ASSERT_TRUE(result);

    bdevs = spdk_api->get_bdevs(spdk::ResizeLvolBdevConnector::LVOL_UUID);
    ASSERT_FALSE(bdevs.empty());

    lvol = bdevs.front();
    ASSERT_EQ(lvol.get_name(), spdk::ResizeLvolBdevConnector::LVOL_UUID);
    ASSERT_EQ(lvol.get_num_blocks(), requested_bytes);
}


TEST_F(ResizeLvolBdevTest, InvalidResizeAmount) {
    bool result = spdk_api->resize_lvol_bdev(spdk::ResizeLvolBdevConnector::LVOL_UUID, 0);
    ASSERT_FALSE(result);
}

TEST_F(ResizeLvolBdevTest, InvalidNameRequested) {
    bool result = spdk_api->resize_lvol_bdev("MyLvolName", 0);
    ASSERT_FALSE(result);
}

}


