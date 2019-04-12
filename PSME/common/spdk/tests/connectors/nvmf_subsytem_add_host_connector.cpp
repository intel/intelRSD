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
 * @file nvmf_subsystem_add_host_connector.cpp
 */


#include "spdk/spdk_api.hpp"
#include "nvmf_subsytem_add_host_connector.hpp"



using namespace spdk;
using namespace json_rpc;

constexpr const char spdk::NvmfSubsystemAddHostConnector::NQN[];
constexpr const char spdk::NvmfSubsystemAddHostConnector::HOST_NQN[];


spdk::NvmfSubsystemAddHostConnector::NvmfSubsystemAddHostConnector() {
    model::NvmfSubsystem subsystem{};
    subsystem.set_nqn(NQN);
    subsystem.set_subtype("NVMe");
    nvmf_subsystem = subsystem;
}


std::string spdk::NvmfSubsystemAddHostConnector::send_request(const std::string& message) {

    json::Json raw_request = json::Json::parse(message);

    auto id = raw_request["id"];

    json::Json params = raw_request["params"];

    json::Json response{};

    if (raw_request["method"] == Method::GET_NVMF_SUBSYSTEMS) {

        response.push_back(nvmf_subsystem);
        return JsonRpcResponse(response, id).to_json().dump();
    }
    else if (raw_request["method"] == Method::NVMF_SUBSYSTEM_ADD_HOST) {

        response = false;
        if (params.count(model::NvmfSubsystem::HOST) && (params.count(model::NvmfSubsystem::NQN))) {
            if (params[model::NvmfSubsystem::NQN] == nvmf_subsystem[model::NvmfSubsystem::NQN]) {

                std::vector<model::Host> hosts{};
                model::Host host{};
                host.set_nqn(params[model::NvmfSubsystem::HOST]);
                hosts.push_back(host);
                nvmf_subsystem[model::NvmfSubsystem::HOSTS] = hosts;

                response = true;
            }
        }

        return JsonRpcResponse(response, id).to_json().dump();
    }
    else {
        throw std::logic_error("Improper method name. Given method hasn't got any implementation.");
    }
}


