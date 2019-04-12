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
#include "nvmf_subsytem_add_listener_connector.hpp"



using namespace spdk;
using namespace json_rpc;

constexpr const char spdk::NvmfSubsystemAddListenerConnector::SUBSYSTEM_NQN[];


spdk::NvmfSubsystemAddListenerConnector::NvmfSubsystemAddListenerConnector() {
    model::NvmfSubsystem subsystem{};
    subsystem.set_nqn(SUBSYSTEM_NQN);
    subsystem.set_subtype("NVMe");
    nvmf_subsystem = subsystem;
}


std::string spdk::NvmfSubsystemAddListenerConnector::send_request(const std::string& message) {

    json::Json raw_request = json::Json::parse(message);
    json::Json params = raw_request["params"];
    auto id = raw_request["id"];

    json::Json response{};
    if (raw_request["method"] == Method::GET_NVMF_SUBSYSTEMS) {
        response.push_back(nvmf_subsystem);
        return JsonRpcResponse(response, id).to_json().dump();
    }
    else if (raw_request["method"] == Method::NVMF_SUBSYSTEM_ADD_LISTENER) {
        response = false;
        if (params.count(model::NvmfSubsystem::LISTEN_ADDRESS) &&
            params.count(model::NvmfSubsystem::NQN)) {

            if (params[model::NvmfSubsystem::NQN] == nvmf_subsystem[model::NvmfSubsystem::NQN] &&
                params[model::NvmfSubsystem::LISTEN_ADDRESS].count(model::ListenAddress::TRADDR) &&
                params[model::NvmfSubsystem::LISTEN_ADDRESS].count(model::ListenAddress::TRTYPE)) {

                std::vector<model::ListenAddress> addresses{};
                model::ListenAddress address{};
                address.set_adrfam(params[model::NvmfSubsystem::LISTEN_ADDRESS][model::ListenAddress::ADRFAM]
                    .get<JsonOptional<std::string>>());
                address.set_trsvcid(params[model::NvmfSubsystem::LISTEN_ADDRESS][model::ListenAddress::TRSVCID]
                    .get<JsonOptional<std::string>>());
                address.set_traddr(params[model::NvmfSubsystem::LISTEN_ADDRESS][model::ListenAddress::TRADDR]);
                address.set_trtype(params[model::NvmfSubsystem::LISTEN_ADDRESS][model::ListenAddress::TRTYPE]);

                if (address.get_trtype().empty() || !address.get_trsvcid() || address.get_traddr().empty() || !address.get_adrfam()) {
                    throw JsonRpcException(json_rpc::common::ERROR_RPC_INVALID_PARAMS, "Invalid parameter");
                }

                addresses.push_back(address);
                nvmf_subsystem[model::NvmfSubsystem::LISTEN_ADDRESSES] = addresses;

                response = true;
            }
        }

        return JsonRpcResponse(response, id).to_json().dump();
    }
    else {
        throw std::logic_error("Improper method name. Given method hasn't got any implementation.");
    }
}


