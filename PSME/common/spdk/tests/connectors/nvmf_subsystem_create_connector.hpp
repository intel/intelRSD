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
 * @file nvmf_subsystem_create_connector.hpp
 */

#include "spdk/model/nvmf_subsystem.hpp"
#include "json-rpc/json_rpc_response.hpp"
#include "json-rpc/connectors/abstract_client_connector.hpp"
#include "json-rpc/handlers/json_rpc_request_invoker.hpp"

#include "spdk/model/lvol_store.hpp"



namespace spdk {

class NvmfSubsystemCreateConnector : public json_rpc::AbstractClientConnector {
private:

    json::Json nvmf_subsystem;

public:
    static constexpr const char NQN[] = "nqn.2014-08.org.nvmexpress:uuid:913fc008-b67a-447f-b2c4-c73543638c31";
    static constexpr const char INVALID_NQN[] = "invalid_nqn";

    NvmfSubsystemCreateConnector();
    virtual std::string send_request(const std::string& message) override;

};

}