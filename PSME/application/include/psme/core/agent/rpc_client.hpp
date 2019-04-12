/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * */

#pragma once

#include "json-rpc/connectors/abstract_client_connector.hpp"
#include "json-rpc/handlers/json_rpc_request_invoker.hpp"
#include "json-wrapper/json-wrapper.hpp"



namespace psme {
namespace core {
namespace agent {

/*! Implementation of JSON RPC Client which can handle framework exceptions. */
class RpcClient {
public:
    RpcClient(json_rpc::AbstractClientConnectorPtr conn, json_rpc::JsonRpcRequestInvokerPtr inv):
        m_connector(conn), m_invoker(inv) {}


    json::Json CallMethod(const std::string& name, const json::Json& parameter);


    virtual ~RpcClient();


private:

    json_rpc::AbstractClientConnectorPtr m_connector;
    json_rpc::JsonRpcRequestInvokerPtr m_invoker;

};

}
}
}
