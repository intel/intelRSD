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
 * @file construct_lvol_stores_connector.cpp
 */

#include "construct_lvol_store_connector.hpp"



namespace spdk {

std::string ConstructLvolStoresConnector::send_request(const std::string& message) {

    json::Json raw_request = json::Json::parse(message);

    auto id = raw_request["id"];

    json::Json params = raw_request["params"];

    if (raw_request["method"] == "get_lvol_stores") {
        std::vector<json::Json> response = {};
        response.push_back(lvol_store);

        return json_rpc::JsonRpcResponse(response, id).to_json().dump();
    }

    // Constructing new lvol_store
    if (params.count("cluster_sz")) {
        ConstructLvolStoresConnector::lvol_store.set_cluster_size(params["cluster_sz"]);
    }
    else {
        ConstructLvolStoresConnector::lvol_store.set_cluster_size(0);
    }

    ConstructLvolStoresConnector::lvol_store.set_name(params["lvs_name"]);
    ConstructLvolStoresConnector::lvol_store.set_base_bdev(params["bdev_name"]);
    ConstructLvolStoresConnector::lvol_store.set_block_size(4096);
    ConstructLvolStoresConnector::lvol_store.set_free_cluster(1234);
    ConstructLvolStoresConnector::lvol_store.set_total_data_clusters(1234);
    ConstructLvolStoresConnector::lvol_store.set_uuid("a9959197-b5e2-4f2d-8095-251ffb6985a5");

    return json_rpc::JsonRpcResponse(lvol_store.get_uuid(), id).to_json().dump();
}

}

