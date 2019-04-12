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
 * @file get_lvol_stores_test.cpp
 */

#include "spdk/spdk_api.hpp"

#include "connectors/get_lvol_stores_connector.hpp"

#include "gtest/gtest.h"



class GetLvolStoresTest : public ::testing::Test {
protected:
    std::string valid_lvol_stores_names[2] = {
        "LVS0",
        "LVS1"
    };

    std::string valid_lvol_stores_uuids[2] = {
        "a9959197-b5e2-4f2d-8095-251ffb6985a5",
        "a9959197-b5e2-4f2d-8095-251ffb6985a6"
    };

    std::shared_ptr<spdk::SpdkApi> spdk_api = {};


    void SetUp() override {
        std::shared_ptr<spdk::GetLvolStoresConnector> stub_connector = std::make_shared<spdk::GetLvolStoresConnector>();
        std::shared_ptr<json_rpc::JsonRpcRequestInvoker> invoker = std::make_shared<json_rpc::JsonRpcRequestInvoker>();
        spdk_api = std::make_shared<::spdk::SpdkApi>(stub_connector, invoker);
    }


    GetLvolStoresTest();


    virtual ~GetLvolStoresTest();
};


GetLvolStoresTest::GetLvolStoresTest() {}


GetLvolStoresTest::~GetLvolStoresTest() {}


namespace testing {

TEST_F(GetLvolStoresTest, ValidMethodResultWithOptionalNameField) {

    std::vector<spdk::model::LvolStore> lvol_stores = spdk_api->get_lvol_stores_by_name(valid_lvol_stores_names[0]);

    ASSERT_EQ(lvol_stores.size(), 1);

    ASSERT_EQ(lvol_stores[0].get_name(), valid_lvol_stores_names[0]);
    ASSERT_EQ(lvol_stores[0].get_uuid(), valid_lvol_stores_uuids[0]);

    ASSERT_EQ(lvol_stores[0].get_base_bdev(), "NVMe1n1");
    ASSERT_EQ(lvol_stores[0].get_free_clusters(), 31);
    ASSERT_EQ(lvol_stores[0].get_cluster_size(), 4194304);
    ASSERT_EQ(lvol_stores[0].get_total_data_clusters(), 31);
    ASSERT_EQ(lvol_stores[0].get_block_size(), 4096);
}


TEST_F(GetLvolStoresTest, ValidMethodResultWithOptionalUuidField) {

    std::vector<spdk::model::LvolStore> lvol_stores = spdk_api->get_lvol_stores_by_uuid(valid_lvol_stores_uuids[1]);

    ASSERT_EQ(lvol_stores.size(), 1);

    ASSERT_EQ(lvol_stores[0].get_uuid(), valid_lvol_stores_uuids[1]);
    ASSERT_EQ(lvol_stores[0].get_name(), valid_lvol_stores_names[1]);

    ASSERT_EQ(lvol_stores[0].get_base_bdev(), "Malloc");
    ASSERT_EQ(lvol_stores[0].get_free_clusters(), 15);
    ASSERT_EQ(lvol_stores[0].get_cluster_size(), 41941);
    ASSERT_EQ(lvol_stores[0].get_total_data_clusters(), 39);
    ASSERT_EQ(lvol_stores[0].get_block_size(), 4000);
}


TEST_F(GetLvolStoresTest, ValidMethodResultWithoutOptionalField) {

    std::vector<spdk::model::LvolStore> lvol_stores = spdk_api->get_lvol_stores();

    ASSERT_EQ(lvol_stores.size(), 2);

    for (unsigned int i = 0; i < lvol_stores.size(); i++) {
        ASSERT_EQ(lvol_stores[i].get_name(), valid_lvol_stores_names[i]);
        ASSERT_EQ(lvol_stores[i].get_uuid(), valid_lvol_stores_uuids[i]);
    }

    ASSERT_EQ(lvol_stores[0].get_base_bdev(), "NVMe1n1");
    ASSERT_EQ(lvol_stores[0].get_free_clusters(), 31);
    ASSERT_EQ(lvol_stores[0].get_cluster_size(), 4194304);
    ASSERT_EQ(lvol_stores[0].get_total_data_clusters(), 31);
    ASSERT_EQ(lvol_stores[0].get_block_size(), 4096);

    ASSERT_EQ(lvol_stores[1].get_base_bdev(), "Malloc");
    ASSERT_EQ(lvol_stores[1].get_free_clusters(), 15);
    ASSERT_EQ(lvol_stores[1].get_cluster_size(), 41941);
    ASSERT_EQ(lvol_stores[1].get_total_data_clusters(), 39);
    ASSERT_EQ(lvol_stores[1].get_block_size(), 4000);

}


TEST_F(GetLvolStoresTest, ValidMethodResultWithIncorrectArguments) {

    std::vector<spdk::model::LvolStore> lvol_stores = spdk_api->get_lvol_stores_by_name("Invalid");

    ASSERT_EQ(lvol_stores.size(), 0);

}

}

