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
 */

#include "json-rpc/connectors/unix_domain_socket_client_connector.hpp"
#include "json-rpc/handlers/json_rpc_request_invoker.hpp"

#include "spdk/spdk_api.hpp"



using namespace spdk;


int main() {

    auto connector = std::make_shared<json_rpc::UnixDomainSocketClientConnector>("/var/tmp/spdk.sock");
    auto invoker = std::make_shared<json_rpc::JsonRpcRequestInvoker>();
    SpdkApi spdk_api(connector, invoker);

    std::vector<spdk::model::Bdev> bdevs = spdk_api.get_bdevs();
    if (bdevs.empty()) {
        std::cout << "=> No Bdevs found, can not run example" << std::endl;
        return -1;
    }

    auto bdev_name = bdevs.front().get_name();
    std::vector<spdk::model::LvolStore> lvol_stores = spdk_api.get_lvol_stores();

    std::cout << "=> Constructing LvolStore" << std::endl;
    std::string lvol_store_uuid{};
    if (lvol_stores.empty()) {
        lvol_store_uuid = spdk_api.construct_lvol_store(bdev_name, "LvolStoreExt");
        std::cout << "=> Constructed LvolStore1 UUID: " << lvol_store_uuid << " =" << std::endl;
    }
    lvol_stores = spdk_api.get_lvol_stores();
    lvol_store_uuid = lvol_stores.front().get_uuid();

    bdevs = spdk_api.get_bdevs();

    std::cout << "=> Constructing Logical Volume" << std::endl;
    std::string uuid = spdk_api.construct_lvol_bdev_by_store_uuid("LvolToExtend", 4194304,
                                                                  lvol_stores.front().get_uuid(), false);
    std::cout << "=> UUID of Logical Volume: " << uuid << std::endl;

    bdevs = spdk_api.get_bdevs(uuid);
    auto lvol_bdev = bdevs.front();
    std::cout << "=> Logical Volume: " << std::endl << lvol_bdev.to_string() << std::endl;

    std::cout << "=> Resizing Logical Volume" << std::endl;
    if (spdk_api.resize_lvol_bdev(lvol_bdev.get_name(), 8388608)) {
        bdevs = spdk_api.get_bdevs(uuid);
        lvol_bdev = bdevs.front();
        std::cout << std::endl << "=> Resized Logical Volume: " << std::endl << lvol_bdev.to_string() << std::endl;
    }
    else {
        std::cout << "=> Failed to resize LVOL: " << lvol_bdev.get_name() << std::endl;
    }

    if (!bdevs.empty()) {
        std::cout << "=> Clean up" << std::endl;
        for (const auto& bdev : bdevs) {
            if (bdev.get_product_name() == model::BdevProductName::LOGICAL_VOLUME) {
                if (spdk_api.destroy_lvol_bdev(bdev.get_name())) {
                    std::cout << "=> Destroyed LVOL " << bdev.get_name() << std::endl;
                }
                else {
                    std::cout << "=> Failed to destroy LVOL: " << bdev.get_name() << std::endl;
                }
            }
        }
    }

    if (spdk_api.destroy_lvol_store(lvol_store_uuid)) {
        std::cout << std::endl << "=> Destroyed LVOL Store " << lvol_store_uuid << std::endl;
    }
    else {
        std::cout << std::endl << "=> Failed to destroy Logical Volume Store" << std::endl;
    }
    return 0;
}
