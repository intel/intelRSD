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
 * @file destroy_lvs_test.cpp
 */

#include "spdk/spdk_api.hpp"

#include "connectors/destroy_lvs_connector.hpp"

#include "gtest/gtest.h"



class DestroyLvsTest : public ::testing::Test {
protected:

    std::string valid_lvol_stores_uuid = "a9959197-b5e2-4f2d-8095-251ffb6985a5";

    std::shared_ptr<spdk::SpdkApi> spdk_api = {};


    void SetUp() override {
        std::shared_ptr<spdk::DestroyLvsConnector> stub_connector = std::make_shared<spdk::DestroyLvsConnector>();
        std::shared_ptr<json_rpc::JsonRpcRequestInvoker> invoker = std::make_shared<json_rpc::JsonRpcRequestInvoker>();
        spdk_api = std::make_shared<::spdk::SpdkApi>(stub_connector, invoker);
    }


    DestroyLvsTest();


    virtual ~DestroyLvsTest();
};


DestroyLvsTest::DestroyLvsTest() {}


DestroyLvsTest::~DestroyLvsTest() {}


namespace testing {

TEST_F(DestroyLvsTest, DestroyNotExistingLvolStore) {

    ASSERT_FALSE(spdk_api->destroy_lvol_store("a1b2c3d4"));

    std::vector<spdk::model::LvolStore> lvol_stores = spdk_api->get_lvol_stores_by_uuid("a1b2c3d4");

    ASSERT_TRUE(lvol_stores.empty());
}


TEST_F(DestroyLvsTest, DestroyOneExistingLvolStore) {

    std::vector<spdk::model::LvolStore> lvol_stores = spdk_api->get_lvol_stores_by_uuid(valid_lvol_stores_uuid);
    ASSERT_EQ(lvol_stores[0].get_uuid(), valid_lvol_stores_uuid);

    ASSERT_TRUE(spdk_api->destroy_lvol_store(valid_lvol_stores_uuid));

    lvol_stores = spdk_api->get_lvol_stores_by_uuid(valid_lvol_stores_uuid);
    ASSERT_TRUE(lvol_stores.empty());
}


TEST_F(DestroyLvsTest, DestroyDestroyedLvolStore) {

    std::vector<spdk::model::LvolStore> lvol_stores = spdk_api->get_lvol_stores_by_uuid(valid_lvol_stores_uuid);
    ASSERT_EQ(lvol_stores[0].get_uuid(), valid_lvol_stores_uuid);

    ASSERT_TRUE(spdk_api->destroy_lvol_store(valid_lvol_stores_uuid));

    lvol_stores = spdk_api->get_lvol_stores_by_uuid(valid_lvol_stores_uuid);
    ASSERT_TRUE(lvol_stores.empty());

    ASSERT_FALSE(spdk_api->destroy_lvol_store(valid_lvol_stores_uuid));

    lvol_stores = spdk_api->get_lvol_stores_by_uuid(valid_lvol_stores_uuid);
    ASSERT_TRUE(lvol_stores.empty());
}

}


