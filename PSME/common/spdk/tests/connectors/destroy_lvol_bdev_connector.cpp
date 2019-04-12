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
 * @file destroy_lvol_bdev_connector.cpp
 */

#include <spdk/model/bdev.hpp>
#include "destroy_lvol_bdev_connector.hpp"



namespace spdk {

DestroyLvolBdevConnector::DestroyLvolBdevConnector() {
    model::Bdev bdev{};
    bdev.set_name("LvolBdev");
    bdev.set_uuid("a9959197-b5e2-4f2d-8095-251ffb6985a5");
    bdev.set_product_name("Logical Volume");
    lvol_bdev = bdev;
}


std::string DestroyLvolBdevConnector::send_request(const std::string& message) {

    json::Json raw_request = json::Json::parse(message);

    auto id = raw_request["id"];

    json::Json params = raw_request["params"];

    json::Json response = {};

    if (raw_request["method"] == "get_bdevs") {
        response = std::vector<json::Json>{};

        if (params.count(model::Bdev::NAME) && lvol_bdev) {
            if (params[model::Bdev::NAME] == lvol_bdev.value().get_name()) {
                response.push_back(lvol_bdev);
            }
        }

        return json_rpc::JsonRpcResponse(response, id).to_json().dump();
    }
    else if (params.count(model::Bdev::NAME) && lvol_bdev) {

        if (lvol_bdev.value().get_name() == params[model::Bdev::NAME]) {
            lvol_bdev = {};
            response = true;
        }
        else {
            response = false;
        }
    }
    else {
        response = false;
    }

    return json_rpc::JsonRpcResponse(response, id).to_json().dump();
}

}

