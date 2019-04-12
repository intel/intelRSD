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
 * @file snapshot_lvol_bdev_connector.cpp
 */

#include "spdk/model/drivers/lvol_driver.hpp"
#include "snapshot_lvol_bdev_connector.hpp"



namespace spdk {

SnapshotLvolBdevConnector::SnapshotLvolBdevConnector() {
    model::Bdev bdev{};
    // Bdev name is the same as uuid
    bdev.set_name("09959197-1212-4343-8989-676711194545");
    bdev.set_uuid("09959197-1212-4343-8989-676711194545");

    std::vector<std::string> aliases;
    aliases.emplace_back("LVS1/11111111-2222-3333-4444-555555555555");
    bdev.set_aliases(aliases);

    spdk::model::drivers::LvolDriver lvol_driver{};
    lvol_driver.set_base_bdev("Nvme1n1");
    lvol_driver.set_lvol_store_uuid("413f21dc-1111-2222-3333-25e2161d69dc");
    lvol_driver.set_clone(false);
    lvol_driver.set_snapshot(false);
    lvol_driver.set_thin_provision(false);
    bdev.set_driver_specific(lvol_driver);

    lvol_bdev = bdev;
}


std::string SnapshotLvolBdevConnector::send_request(const std::string& message) {

    json::Json raw_request = json::Json::parse(message);

    auto id = raw_request["id"];

    json::Json params = raw_request["params"];

    if (raw_request["method"] == "get_bdevs") {
        std::vector<json::Json> response = {};
        response.emplace_back(lvol_bdev);
        if (lvol_bdev_snapshot) {
            response.emplace_back(lvol_bdev_snapshot);
        }
        return json_rpc::JsonRpcResponse(response, id).to_json().dump();
    }

    std::string lvol_alias = lvol_bdev.get_aliases()[0];

    if (lvol_bdev.get_uuid().value() == params["lvol_name"] ||
        lvol_alias.substr(lvol_alias.find('/') + 1) == params["lvol_name"]) {

        lvol_bdev_snapshot = lvol_bdev;
        // Lvol Bdev name is the same as uuid
        lvol_bdev_snapshot->set_uuid("cc8d7fdf-7865-4d1f-9fc6-35da8e368670");
        lvol_bdev_snapshot->set_name("cc8d7fdf-7865-4d1f-9fc6-35da8e368670");

        // Snapshot Lvol Bdev alias
        std::vector<std::string> aliases;
        std::string snapshot_name = params["snapshot_name"];
        aliases.push_back(lvol_alias.substr(0, lvol_alias.find('/') + 1) + snapshot_name);
        lvol_bdev_snapshot->set_aliases(aliases);

        // Snapshot Lvol Bdev driver
        spdk::model::drivers::LvolDriver snapshot_lvol_driver{};
        snapshot_lvol_driver.set_base_bdev("Nvme1n1");
        snapshot_lvol_driver.set_clone(false);
        snapshot_lvol_driver.set_snapshot(true);
        snapshot_lvol_driver.set_lvol_store_uuid("413f21dc-1111-2222-3333-25e2161d69dc");
        snapshot_lvol_driver.set_thin_provision(false);

        std::vector<std::string> clones;
        clones.push_back(lvol_alias.substr(lvol_alias.find('/') + 1));
        snapshot_lvol_driver.set_clones(clones);
        lvol_bdev_snapshot->set_driver_specific(snapshot_lvol_driver);

        spdk::model::drivers::LvolDriver lvol_driver = lvol_bdev.get_driver_specific();
        lvol_driver.set_clone(true);
        lvol_driver.set_base_snapshot(snapshot_name);
        lvol_bdev.set_driver_specific(lvol_driver);
    }
    else {
        //Invalid searched Bdev Uuid or alias.
        return json_rpc::JsonRpcResponse("", id).to_json().dump();
    }

    return json_rpc::JsonRpcResponse(lvol_bdev_snapshot->get_uuid(), id).to_json().dump();
}

}

