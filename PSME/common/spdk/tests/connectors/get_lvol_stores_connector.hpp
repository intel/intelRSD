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
 * @file get_lvol_stores_connector.hpp
 */

#include "json-rpc/json_rpc_response.hpp"
#include "json-rpc/connectors/abstract_client_connector.hpp"
#include "json-rpc/handlers/json_rpc_request_invoker.hpp"

#include "spdk/model/lvol_store.hpp"

#include "construct_lvol_store_connector.hpp"



namespace spdk {

class GetLvolStoresConnector : public json_rpc::AbstractClientConnector {

private:

    static const json::Json LVOL_STORES;

public:

    virtual std::string send_request(const std::string& message) override {

        json::Json raw_request = json::Json::parse(message);

        auto id = raw_request["id"];

        json::Json params = raw_request["params"];

        std::vector<json::Json> response = {};

        if (params.count(model::LvolStore::NAME)) {

            for (const auto& lvol_store : LVOL_STORES) {
                if (params[model::LvolStore::NAME] == lvol_store[model::LvolStore::NAME]) {
                    response.push_back(lvol_store);
                    return json_rpc::JsonRpcResponse(response, id).to_json().dump();
                }
            }

            //Invalid searched lvol store name.
            return json_rpc::JsonRpcResponse(response, id).to_json().dump();
        }
        else if (params.count(model::LvolStore::UUID)) {

            for (const auto& lvol_store : LVOL_STORES) {
                if (params[model::LvolStore::UUID] == lvol_store[model::LvolStore::UUID]) {
                    response.push_back(lvol_store);
                    return json_rpc::JsonRpcResponse(response, id).to_json().dump();
                }
            }

            //Invalid searched lvol store uuid.
            return json_rpc::JsonRpcResponse(response, id).to_json().dump();
        }
        else {
            //If there isn't any param, return all lvol_stores.
            return json_rpc::JsonRpcResponse(LVOL_STORES, id).to_json().dump();
        }

    }
};
}