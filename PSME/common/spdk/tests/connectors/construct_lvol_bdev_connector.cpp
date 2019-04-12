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
 * @file construct_lvol_bdev_connector.cpp
 */

#include "construct_lvol_bdev_connector.hpp"
#include "spdk/model/drivers/lvol_driver.hpp"

namespace spdk {

std::string ConstructLvolBdevConnector::send_request(const std::string& message) {

    json::Json raw_request = json::Json::parse(message);

    auto id = raw_request["id"];

    json::Json params = raw_request["params"];

    if (raw_request["method"] == "get_bdevs") {
        std::vector<json::Json> response = {};
        response.push_back(m_lvol_bdev);

        return json_rpc::JsonRpcResponse(response, id).to_json().dump();
    }

    // Constructing new Lvol Bdev
    // Lvol's name is the same as Lvol's uuid
    std::string lvol_uuid = "a9295197-p5j2-4f2d-0910-1513fb6986a2";
    ConstructLvolBdevConnector::m_lvol_bdev.set_name(lvol_uuid);
    ConstructLvolBdevConnector::m_lvol_bdev.set_product_name("Logical Volume");
    ConstructLvolBdevConnector::m_lvol_bdev.set_block_size(params["size"]);
    ConstructLvolBdevConnector::m_lvol_bdev.set_num_blocks(2501);
    ConstructLvolBdevConnector::m_lvol_bdev.set_uuid(lvol_uuid);
    ConstructLvolBdevConnector::m_lvol_bdev.set_claimed(false);
    // Default Lvol Store name
    std::string lvs_name = "LVS";

    // Driver specific
    spdk::model::drivers::LvolDriver lvol_driver{};
    lvol_driver.set_clone(false);
    lvol_driver.set_snapshot(false);
    lvol_driver.set_base_bdev("NVMe");

    if (params.count("lvs_name")) {
        lvol_driver.set_lvol_store_uuid("10000000-2000-3000-4000-abcdeabcdeabc");
        lvs_name = params["lvs_name"];
    }
    else if (params.count("uuid")) {
        lvol_driver.set_lvol_store_uuid(params["uuid"]);
    }

    if (params.count("thin_provision")) {
        lvol_driver.set_thin_provision(params["thin_provision"]);
    }
    else {
        lvol_driver.set_thin_provision(false);
    }

    m_lvol_bdev.set_driver_specific(lvol_driver);

    std::vector<std::string> aliases;
    std::string lvol_name = params["lvol_name"];
    aliases.push_back(lvs_name + "/" + lvol_name);
    ConstructLvolBdevConnector::m_lvol_bdev.set_aliases(aliases);

    // Supported IO types
    spdk::model::SupportedIoTypes supported_io_types{};
    supported_io_types.set_reset(true);
    supported_io_types.set_nvme_admin(true);
    supported_io_types.set_unmap(true);
    supported_io_types.set_read(true);
    supported_io_types.set_write_zeroes(true);
    supported_io_types.set_write(true);
    supported_io_types.set_flush(false);
    supported_io_types.set_nvme_io(false);
    m_lvol_bdev.set_supported_io_types(supported_io_types);

    m_lvol_bdev.set_qos_ios_per_sec(1);

    return json_rpc::JsonRpcResponse(m_lvol_bdev.get_uuid(), id).to_json().dump();
}

}

