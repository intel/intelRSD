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
 * @file destroy_lvs_connector.cpp
 */

#include "destroy_lvs_connector.hpp"



namespace spdk {

DestroyLvsConnector::DestroyLvsConnector() {
    model::LvolStore lvs{};
    lvs.set_uuid("a9959197-b5e2-4f2d-8095-251ffb6985a5");
    lvs.set_name("Nvme");
    lvol_store = lvs;
}


std::string DestroyLvsConnector::send_request(const std::string& message) {

    json::Json raw_request = json::Json::parse(message);

    auto id = raw_request["id"];

    json::Json params = raw_request["params"];

    json::Json response = {};

    if (raw_request["method"] == "get_lvol_stores") {
        response = std::vector<json::Json>{};

        if (params.count(model::LvolStore::UUID) && lvol_store) {
            if (params[model::LvolStore::UUID] == lvol_store.value().get_uuid()) {
                response.push_back(lvol_store);
            }
        }

        return json_rpc::JsonRpcResponse(response, id).to_json().dump();
    }

    if (params.count(model::LvolStore::NAME) && lvol_store) {

        if (lvol_store.value().get_name() == params[model::LvolStore::NAME]) {
            lvol_store = {};
            response = true;
        }
        else {
            response = false;
        }
    }
    else if (params.count(model::LvolStore::UUID) && lvol_store) {

        if (lvol_store.value().get_uuid() == params[model::LvolStore::UUID]) {
            lvol_store = {};
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

