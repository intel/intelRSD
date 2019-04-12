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
 */

#include "psme/core/agent/rpc_client.hpp"
#include "psme/rest/server/error/error_factory.hpp"



using namespace psme::core::agent;
using namespace psme::rest::error;
using namespace agent_framework::exceptions;

json::Json RpcClient::CallMethod(const std::string& name, const json::Json& parameter) {
    try {
        m_invoker->prepare_method(name, parameter);
        m_invoker->call(m_connector);
        return m_invoker->get_result();
    }
    catch (const json_rpc::JsonRpcException& ex) {
        const auto error_code = static_cast<ErrorCode>(ex.get_code());
        log_error("rest", "RPC connector threw exception: >>"
            << ex.what() << "<< for method " << name << ".");

        // JSON RPC InvalidParameter exception is similar to InvalidField exception
        if (is_communication_error(error_code)) {
            // Special case:
            // Create internal server error if JSON fields are incorrect
            // We do not want to show on REST API internal communication data such as GAMI fields.
            auto error = ErrorFactory::create_internal_error("There were communication errors with underlying Agent.");
            throw ServerException(error);
        }
        else if (json_rpc::common::ERROR_CLIENT_CONNECTOR == ex.get_code()) {
            // Rethrow this kind of exception to handle Agent disconnection.
            throw;
        }
        else {
            // Throw REST server exception from GAMI exception
            auto error = ErrorFactory::create_error_from_gami_exception(
                GamiException{error_code, ex.get_message(), ex.get_data()});
            throw ServerException(error);
        }
    }
}


RpcClient::~RpcClient() {}
