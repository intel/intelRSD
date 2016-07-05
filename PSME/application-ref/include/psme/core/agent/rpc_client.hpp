/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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

#ifndef PSME_CORE_AGENT_RPC_CLIENT_HPP
#define PSME_CORE_AGENT_RPC_CLIENT_HPP

#include <jsonrpccpp/client.h>

namespace psme {
namespace core {
namespace agent {

/*! Implementation of JSON RPC Client which can handle framework exceptions. */
class RpcClient : jsonrpc::Client {
public:
    RpcClient(jsonrpc::IClientConnector& conn) :
            jsonrpc::Client(conn) { }

    Json::Value CallMethod(const std::string& name,
                           const Json::Value& parameter);

    virtual ~RpcClient();

private:

};

}
}
}

#endif // PSME_CORE_AGENT_RPC_CLIENT_HPP
