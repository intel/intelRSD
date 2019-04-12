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

namespace {
void print_bdevs(const std::vector<spdk::model::Bdev>& bdevs) {

    std::cout << std::endl << "Bdevs count: " << bdevs.size() << std::endl << std::endl;
    for (const auto& bdev : bdevs) {
        std::cout << bdev.to_string() << std::endl;
    }

}


void print_lvol_stores(const std::vector<spdk::model::LvolStore>& lvol_stores) {
    std::cout << std::endl << "Lvol stores count: " << lvol_stores.size() << std::endl << std::endl;
    for (const auto& lvol_store : lvol_stores) {
        std::cout << lvol_store.to_string() << std::endl;
    }
}


void print_nvmf_subsystems(const std::vector<spdk::model::NvmfSubsystem>& nvmf_subsystems) {

    std::cout << std::endl << "NVMe subsystem count: " << nvmf_subsystems.size() << std::endl << std::endl;
    for (const auto& nvmf_subsystem : nvmf_subsystems) {
        std::cout << nvmf_subsystem.to_string() << std::endl;
    }

}


bool check_if_lvol_exists(const std::vector<spdk::model::Bdev>& bdevs) {

    for (const auto& bdev : bdevs) {
        if (bdev.get_product_name() == model::BdevProductName::LOGICAL_VOLUME) {
            return true;
        }
    }
    return false;
}

}


int main() {

    auto connector = std::make_shared<json_rpc::UnixDomainSocketClientConnector>("/var/tmp/spdk.sock");
    auto invoker = std::make_shared<json_rpc::JsonRpcRequestInvoker>();
    SpdkApi spdk_api(connector, invoker);

    std::vector<spdk::model::Bdev> bdevs = spdk_api.get_bdevs();
    print_bdevs(bdevs);

    if (bdevs.empty()) {
        std::cout << "No Bdevs found, can not run example" << std::endl;
        return -1;
    }

    std::cout << std::endl;

    auto bdev_name = bdevs.front().get_name();

    std::vector<spdk::model::LvolStore> lvol_stores = spdk_api.get_lvol_stores();
    print_lvol_stores(lvol_stores);

    /* Construct lvol store */
    std::cout << "==============Constructing LvolStore============= " << std::endl;
    std::string lvol_store_uuid{};
    if (lvol_stores.empty()) {
        lvol_store_uuid = spdk_api.construct_lvol_store(bdev_name, "LvolStore1");
        std::cout << "==============Constructed LvolStore1 UUID:============= " << std::endl << lvol_store_uuid
                  << std::endl;
    }
    lvol_stores = spdk_api.get_lvol_stores();
    lvol_store_uuid = lvol_stores.front().get_uuid();

    bdevs = spdk_api.get_bdevs();

    /* Construct lvol */
    if (!check_if_lvol_exists(bdevs)) {
        std::cout << std::endl << "==================Construct Logical Volume================" << std::endl;
        std::string uuid = spdk_api.construct_lvol_bdev_by_store_uuid("Bdev1", 10090000,
                                                                      lvol_stores.front().get_uuid(), false);
        spdk::JsonOptional<std::string> parameter;
        parameter.emplace(uuid);
        auto lvol_bdev = spdk_api.get_bdevs(parameter).front();

        std::cout << std::endl;
        lvol_bdev.to_string();
    }

    bdevs = spdk_api.get_bdevs();
    print_bdevs(bdevs);



    /* Get NVMe over Fabrics Subsystem */
    std::vector<model::NvmfSubsystem> nvmf_subsystems = spdk_api.get_nvmf_subsystems();
    print_nvmf_subsystems(nvmf_subsystems);

    std::vector<model::Namespace> namespaces{};

    for (const auto& bdev: bdevs) {
        if (bdev.get_product_name() == model::BdevProductName::LOGICAL_VOLUME) {
            model::Namespace nvmf_namespace{};
            nvmf_namespace.set_bdev_name(bdev.get_name());
            namespaces.push_back(nvmf_namespace);
        }
    }

    std::cout << "============================CONSTRUCT NVMe over Fabrics Subsystem==============" << std::endl;

    /* Construct NVMe over Fabrics Subsystem */
    std::string nqn = "nqn.2016-06.io.spdk:cnode1";

    if (spdk_api.nvmf_subsystem_create(nqn, "serial_number")) {
        std::cout << "==============NVMe over Fabrics Subsystem Have been constructed: " << std::endl;
        std::cout << "NQN : " << nqn << std::endl;
    }
    else {
        std::cout << "Failed to construct NVMe over Fabrics Subsystem" << std::endl;
    }

    for (const auto& ns : namespaces) {
        auto nsid = spdk_api.nvmf_subsystem_add_ns(nqn, ns);
        std::cout << "Attached namespace NSID: " << nsid << " to nvmf: " << nqn << std::endl;
    }

    model::ListenAddress listen_address{};
    listen_address.set_trtype(model::TransportType::RDMA);
    listen_address.set_adrfam(model::AddressFamily::IPv4);
    listen_address.set_traddr("10.2.0.10");
    listen_address.set_trsvcid("4420");
    if (spdk_api.nvmf_subsystem_add_listener(nqn, listen_address)) {
        std::cout << "NVMe over Fabrics transport applyed" << std::endl;
    }
    else {
        std::cout << "Failed to apply NVMe over Fabrics transport" << std::endl;
    }


    /* Destroy NVMe over Fabrics Subsystem */
    std::cout << "============================DESTROY NVMe over Fabrics Subsystem==============" << std::endl;
    if (spdk_api.delete_nvmf_subsystem(nqn)) {
        std::cout << "==============NVMe over Fabrics Subsystem Have been deleted ============= " << std::endl;
        std::cout << "NQN : " << nqn << std::endl;
    }
    else {
        std::cout << "=============Failed to destroy NVMe over Fabrics Subsystem : " << nqn << " ===================="
                  << std::endl;
    }

    /* Destroy lvol */
    if (!bdevs.empty()) {
        std::cout << "===================DESTROY LVOL BDEV==============================" << std::endl;
        for (const auto& bdev : bdevs) {
            if (bdev.get_product_name() == model::BdevProductName::LOGICAL_VOLUME) {
                if (spdk_api.destroy_lvol_bdev(bdev.get_name())) {
                    std::cout << std::endl << "Destroyed lvol_bdev "
                              << bdev.get_name() << std::endl;
                }
                else {
                    std::cout << "Failed to destroy lvol bdev: " << bdev.get_name() << std::endl;
                }
            }
        }
    }


    /* Destroy lvol store */
    if (!lvol_stores.empty()) {
        model::LvolStore lvol_store = lvol_stores.front();
        if (spdk_api.destroy_lvol_store(lvol_stores.front().get_uuid())) {
            std::cout << std::endl << "============Destroyed Lvol Store:========================" << std::endl;
            std::cout << std::endl << lvol_store.to_string() << std::endl << std::endl;
        }
        else {
            std::cout << std::endl << "Deletion failed for Logical Volume Store:" << lvol_stores.front().get_uuid()
                      << std::endl << std::endl;
        }
    }
}
