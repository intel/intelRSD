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
 * @file get_bdevs_connector.hpp
 */

#include "json-rpc/json_rpc_response.hpp"
#include "json-rpc/connectors/abstract_client_connector.hpp"
#include "json-rpc/handlers/json_rpc_request_invoker.hpp"

#include "spdk/model/bdev.hpp"



namespace spdk {

class GetBdevsConnector : public json_rpc::AbstractClientConnector {
private:

    static const json::Json BDEVS;

public:

    virtual std::string send_request(const std::string& message) override {
        json::Json raw_request = json::Json::parse(message);
        auto id = raw_request["id"];
        json::Json params = raw_request["params"];

        if (params.count(model::Bdev::NAME)) {
            std::vector<json::Json> response = {};

            for (const auto& bdev : BDEVS) {
                if (params[model::Bdev::NAME] == bdev[model::Bdev::NAME]) {
                    response.push_back(bdev);
                    return json_rpc::JsonRpcResponse(response, id).to_json().dump();
                }
            }

            //Invalid searched key.
            return json_rpc::JsonRpcResponse(response, id).to_json().dump();
        }
        else {
            //If there isn't any param, return all bdevs.
            return json_rpc::JsonRpcResponse(BDEVS, id).to_json().dump();
        }
    }
};

}