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
 * @file nvmf_subsystem_add_listener_connector.cpp
 */


#include "spdk/spdk_api.hpp"
#include "nvmf_subsytem_add_ns_connector.hpp"



using namespace spdk;
using namespace json_rpc;

constexpr const char spdk::NvmfSubsystemAddNsConnector::SUBSYSTEM_NQN[];


spdk::NvmfSubsystemAddNsConnector::NvmfSubsystemAddNsConnector() {
    model::NvmfSubsystem subsystem{};
    subsystem.set_nqn(SUBSYSTEM_NQN);
    subsystem.set_subtype("NVMe");
    nvmf_subsystem = subsystem;
}


std::string spdk::NvmfSubsystemAddNsConnector::send_request(const std::string& message) {

    json::Json raw_request = json::Json::parse(message);
    json::Json params = raw_request["params"];
    auto id = raw_request["id"];

    json::Json response{};
    if (raw_request["method"] == Method::GET_NVMF_SUBSYSTEMS) {
        response.push_back(nvmf_subsystem);
        return JsonRpcResponse(response, id).to_json().dump();
    }
    else if (raw_request["method"] == Method::NVMF_SUBSYSTEM_ADD_NS) {
        response = 0;
        if (params.count(model::NvmfSubsystem::NAMESPACE) &&
            params.count(model::NvmfSubsystem::NQN)) {

            if (params[model::NvmfSubsystem::NQN] == nvmf_subsystem[model::NvmfSubsystem::NQN] &&
                params[model::NvmfSubsystem::NAMESPACE].count(model::Namespace::BDEV_NAME)) {

                auto bdev = params[model::NvmfSubsystem::NAMESPACE][model::Namespace::BDEV_NAME].get<std::string>();
                if (bdev.empty()) {
                    throw JsonRpcException(json_rpc::common::ERROR_RPC_INVALID_PARAMS, "Invalid parameter");
                }

                std::vector<model::Namespace> namespaces{};
                model::Namespace ns{};
                ns.set_bdev_name(bdev);
                ns.set_namespace_id(1);
                ns.set_nguid("testestest");
                ns.set_eui_64("1234567890");
                ns.set_uuid("1234-4567-8900");

                namespaces.push_back(ns);
                nvmf_subsystem[model::NvmfSubsystem::NAMESPACES] = namespaces;
                response = 1;
            }
        }

        return JsonRpcResponse(response, id).to_json().dump();
    }
    else {
        throw std::logic_error("Improper method name. Given method hasn't got any implementation.");
    }
}


