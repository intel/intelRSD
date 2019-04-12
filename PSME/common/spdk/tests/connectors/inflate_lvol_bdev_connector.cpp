/*!
 * @copyright
 * Copyright (c) 2019 Intel Corporation
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
 * @file inflate_lvol_bdev_connector.cpp
 */

#include "inflate_lvol_bdev_connector.hpp"
#include "spdk/model/drivers/lvol_driver.hpp"



namespace spdk {

std::vector<model::Bdev> InflateLvolBdevConnector::m_lvol_bdev = std::vector<model::Bdev>();


InflateLvolBdevConnector::InflateLvolBdevConnector() {

    // Parent Bdev
    model::Bdev bdev{};
    // Bdev name is the same as uuid
    bdev.set_name("ccaaccaa-ccaa-ccaa-ccaa-ccaaccaaccaa");
    bdev.set_uuid("ccaaccaa-ccaa-ccaa-ccaa-ccaaccaaccaa");

    std::vector<std::string> aliases;
    aliases.emplace_back("LVS1/00000000-2222-3333-4444-111111111111");
    bdev.set_aliases(aliases);

    spdk::model::drivers::LvolDriver lvol_driver{};
    lvol_driver.set_base_bdev("Nvme1n1");
    lvol_driver.set_lvol_store_uuid("413f21dc-1111-2222-3333-25e2161d69dc");
    lvol_driver.set_clone(false);
    lvol_driver.set_snapshot(true);
    lvol_driver.set_thin_provision(false);

    std::vector<std::string> clones;
    clones.push_back("99999999-0000-9999-0000-999999999999");
    lvol_driver.set_clones(clones);

    json::Json lvol_driver_json{};
    lvol_driver_json[lvol_driver.DRIVER_NAME] = lvol_driver;
    bdev.set_driver_specific(lvol_driver_json);

    m_lvol_bdev.push_back(bdev);

    // Bdev to inflate
    model::Bdev inflate_bdev{};
    inflate_bdev.set_name("11111111-2222-3333-4444-555555555555");
    inflate_bdev.set_uuid("11111111-2222-3333-4444-555555555555");

    std::vector<std::string> inflate_aliases;
    inflate_aliases.emplace_back("LVS1/11111112-2223-3334-4445-555555555556");
    inflate_bdev.set_aliases(inflate_aliases);

    spdk::model::drivers::LvolDriver inflate_lvol_driver{};
    inflate_lvol_driver.set_base_bdev("Nvme1n1");
    inflate_lvol_driver.set_lvol_store_uuid("413f21dc-1111-2222-3333-25e2161d69dc");
    inflate_lvol_driver.set_clone(false);
    inflate_lvol_driver.set_snapshot(true);
    inflate_lvol_driver.set_thin_provision(true);

    json::Json inflate_lvol_driver_json{};
    inflate_lvol_driver_json[inflate_lvol_driver.DRIVER_NAME] = inflate_lvol_driver;
    inflate_bdev.set_driver_specific(inflate_lvol_driver_json);

    m_lvol_bdev.push_back(inflate_bdev);
}


std::string InflateLvolBdevConnector::send_request(const std::string& message) {

    json::Json raw_request = json::Json::parse(message);

    auto id = raw_request["id"];

    json::Json params = raw_request["params"];

    if (raw_request["method"] == "get_bdevs") {
        std::vector<json::Json> response = {};
        // return inflated bdev
        response.emplace_back(m_lvol_bdev[1]);
        return json_rpc::JsonRpcResponse(response, id).to_json().dump();
    }

    bool is_inflated = false;

    JsonOptional<spdk::model::drivers::LvolDriver> lvol_bdev_driver{};

    std::string lvol_bdev_alias{};

    for (auto& bdev : m_lvol_bdev) {

        lvol_bdev_alias = bdev.get_aliases()[0];
        if (bdev.get_name() == params["name"] ||
            lvol_bdev_alias.substr(lvol_bdev_alias.find('/') + 1) == params["name"]) {

            lvol_bdev_driver = bdev.get_driver_specific<model::drivers::LvolDriver>();
            if (lvol_bdev_driver->is_thin_provision()) {
                lvol_bdev_driver->set_thin_provision(false);

                json::Json inflate_lvol_driver_json{};
                inflate_lvol_driver_json[lvol_bdev_driver->DRIVER_NAME] = lvol_bdev_driver;
                bdev.set_driver_specific(inflate_lvol_driver_json);

                is_inflated = true;
            }
        }
    }

    return json_rpc::JsonRpcResponse(is_inflated, id).to_json().dump();
}

}


