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
 * @file resize_lvol_bdev_connector.cpp
 */

#include "resize_lvol_bdev_connector.hpp"
#include "json-rpc/json_rpc_response.hpp"


namespace spdk {

const constexpr char ResizeLvolBdevConnector::LVOL_UUID[];

ResizeLvolBdevConnector::ResizeLvolBdevConnector() {
    model::Bdev bdev{};

    bdev.set_name(LVOL_UUID);
    bdev.set_uuid(LVOL_UUID);

    std::vector<std::string> aliases;
    aliases.emplace_back("LvS/lVoL");
    bdev.set_aliases(aliases);

    bdev.set_num_blocks(10000);
    lvol_bdev = bdev;
}


std::string ResizeLvolBdevConnector::send_request(const std::string& message) {
    json::Json raw_request = json::Json::parse(message);

    auto id = raw_request["id"];

    json::Json params = raw_request["params"];

    if (raw_request["method"] == "get_bdevs") {
        std::vector<json::Json> response = {};
        response.emplace_back(lvol_bdev);
        return json_rpc::JsonRpcResponse(response, id).to_json().dump();
    }

    bool result{};
    std::string lvol_alias = lvol_bdev.get_aliases()[0];

    if (lvol_bdev.get_uuid().value() == params["name"] ||
        lvol_alias.substr(lvol_alias.find('/') + 1) == params["name"]) {

        std::uint64_t bytes = params["size"].get<std::uint64_t>();
        if (bytes != 0) {
            lvol_bdev.set_num_blocks(bytes); // Simplify without clusters
            result = true;
        }
    }

    return json_rpc::JsonRpcResponse(result, id).to_json().dump();
}

}

