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
 * @file clone_lvol_bdev_connector.cpp
 */

#include "clone_lvol_bdev_connector.hpp"
#include "spdk/model/drivers/lvol_driver.hpp"



namespace spdk {

model::Bdev CloneLvolBdevConnector::m_lvol_bdev_snapshot = model::Bdev();
spdk::JsonOptional<model::Bdev> CloneLvolBdevConnector::m_lvol_bdev_clone = spdk::JsonOptional<model::Bdev>();


CloneLvolBdevConnector::CloneLvolBdevConnector() {

    model::Bdev bdev{};
    // Bdev name is the same as uuid
    bdev.set_name("ccaaccaa-ccaa-ccaa-ccaa-ccaaccaaccaa");
    bdev.set_uuid("ccaaccaa-ccaa-ccaa-ccaa-ccaaccaaccaa");

    std::vector<std::string> aliases;
    aliases.emplace_back("LVS1/11111111-2222-3333-4444-555555555555");
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

    m_lvol_bdev_snapshot = bdev;
}


std::string CloneLvolBdevConnector::send_request(const std::string& message) {

    json::Json raw_request = json::Json::parse(message);

    auto id = raw_request["id"];

    json::Json params = raw_request["params"];

    if (raw_request["method"] == "get_bdevs") {
        std::vector<json::Json> response = {};
        response.push_back(m_lvol_bdev_snapshot);
        if (m_lvol_bdev_clone) {
            response.push_back(m_lvol_bdev_clone);
        }
        return json_rpc::JsonRpcResponse(response, id).to_json().dump();
    }

    std::string lvol_snapshot_alias = m_lvol_bdev_snapshot.get_aliases()[0];

    JsonOptional<spdk::model::drivers::LvolDriver> lvol_snapshot_driver = m_lvol_bdev_snapshot.get_driver_specific<model::drivers::LvolDriver>();

    if (lvol_snapshot_driver->is_snapshot()) {
        if (m_lvol_bdev_snapshot.get_uuid().value() == params["snapshot_name"] ||
            lvol_snapshot_alias.substr(lvol_snapshot_alias.find('/') + 1) == params["snapshot_name"]) {

            m_lvol_bdev_clone = m_lvol_bdev_snapshot;

            // Lvol Bdev name is the same as uuid
            m_lvol_bdev_clone->set_uuid("cc8d7fdf-7865-4d1f-9fc6-35da8e368670");
            m_lvol_bdev_clone->set_name("cc8d7fdf-7865-4d1f-9fc6-35da8e368670");

            // Clone Lvol Bdev alias
            std::vector<std::string> aliases;
            std::string clone_name = params["clone_name"];
            aliases.push_back(lvol_snapshot_alias.substr(0, lvol_snapshot_alias.find('/') + 1) + clone_name);
            m_lvol_bdev_clone->set_aliases(aliases);

            // Clone Lvol Bdev driver
            spdk::model::drivers::LvolDriver clone_lvol_driver{};
            clone_lvol_driver.set_base_bdev("Nvme1n1");
            clone_lvol_driver.set_clone(true);
            clone_lvol_driver.set_snapshot(false);
            clone_lvol_driver.set_lvol_store_uuid("413f21dc-1111-2222-3333-25e2161d69dc");
            clone_lvol_driver.set_thin_provision(false);

            json::Json clone_lvol_driver_json{};
            clone_lvol_driver_json[clone_lvol_driver.DRIVER_NAME] = clone_lvol_driver;
            m_lvol_bdev_clone->set_driver_specific(clone_lvol_driver_json);

            // Lvol Bdev Snapshot update
            lvol_snapshot_driver->set_clone(false);
            std::vector<std::string> clones;
            clones.push_back(clone_name);
            lvol_snapshot_driver->set_clones(clones);

            json::Json lvol_driver_json{};
            lvol_driver_json[lvol_snapshot_driver->DRIVER_NAME] = lvol_snapshot_driver;
            m_lvol_bdev_snapshot.set_driver_specific(lvol_driver_json);

        }
        else {
            //Invalid searched Bdev Uuid or alias.
            return json_rpc::JsonRpcResponse("", id).to_json().dump();
        }
    }

    return json_rpc::JsonRpcResponse(m_lvol_bdev_clone->get_uuid(), id).to_json().dump();
}

}


