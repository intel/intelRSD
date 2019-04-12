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
 * @file construct_lvol_store_test.cpp
 */

#include "spdk/spdk_api.hpp"

#include "connectors/construct_lvol_store_connector.hpp"

#include "gtest/gtest.h"



class ConstructLvolStoreTest : public ::testing::Test {
protected:

    std::string valid_lvol_stores_uuid = "a9959197-b5e2-4f2d-8095-251ffb6985a5";

    std::shared_ptr<spdk::SpdkApi> spdk_api = {};


    void SetUp() override {
        std::shared_ptr<spdk::ConstructLvolStoresConnector> stub_connector = std::make_shared<spdk::ConstructLvolStoresConnector>();
        std::shared_ptr<json_rpc::JsonRpcRequestInvoker> invoker = std::make_shared<json_rpc::JsonRpcRequestInvoker>();
        spdk_api = std::make_shared<::spdk::SpdkApi>(stub_connector, invoker);
    }


    ConstructLvolStoreTest();


    virtual ~ConstructLvolStoreTest();
};


ConstructLvolStoreTest::ConstructLvolStoreTest() {}


ConstructLvolStoreTest::~ConstructLvolStoreTest() {}


namespace testing {

TEST_F(ConstructLvolStoreTest, ValidMethodResultWithOptionalField) {

    std::string lvol_stores_uuid = spdk_api->construct_lvol_store("Malloc", "LVS2", 39);

    ASSERT_EQ(lvol_stores_uuid, valid_lvol_stores_uuid);

    std::vector<spdk::model::LvolStore> lvol_stores = spdk_api->get_lvol_stores_by_uuid(lvol_stores_uuid);

    ASSERT_EQ(lvol_stores[0].get_block_size(), 4096);
    ASSERT_EQ(lvol_stores[0].get_total_data_clusters(), 1234);
    ASSERT_EQ(lvol_stores[0].get_cluster_size(), 39);
    ASSERT_EQ(lvol_stores[0].get_free_clusters(), 1234);
    ASSERT_EQ(lvol_stores[0].get_name(), "LVS2");
    ASSERT_EQ(lvol_stores[0].get_base_bdev(), "Malloc");

}


TEST_F(ConstructLvolStoreTest, ValidMethodResultWithoutOptionalField) {

    std::string lvol_stores_uuid = spdk_api->construct_lvol_store("NVMe1n1", "LVS3");

    ASSERT_EQ(lvol_stores_uuid, valid_lvol_stores_uuid);

    std::vector<spdk::model::LvolStore> lvol_stores = spdk_api->get_lvol_stores_by_uuid(lvol_stores_uuid);

    ASSERT_EQ(lvol_stores[0].get_block_size(), 4096);
    ASSERT_EQ(lvol_stores[0].get_total_data_clusters(), 1234);
    ASSERT_EQ(lvol_stores[0].get_free_clusters(), 1234);
    ASSERT_EQ(lvol_stores[0].get_name(), "LVS3");
    ASSERT_EQ(lvol_stores[0].get_cluster_size(), 0);
    ASSERT_EQ(lvol_stores[0].get_base_bdev(), "NVMe1n1");

}

}


