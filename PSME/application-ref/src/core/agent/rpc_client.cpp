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
 */

#include "psme/core/agent/rpc_client.hpp"
#include "agent-framework/exceptions/error_codes.hpp"
#include "agent-framework/exceptions/exception.hpp"

using namespace psme::core::agent;

Json::Value RpcClient::CallMethod(const std::string& name,
                                  const Json::Value& parameter) {

    try {
        return jsonrpc::Client::CallMethod(name, parameter);
    }
    catch (const jsonrpc::JsonRpcException& e) {
        using namespace agent_framework::exceptions;

        const auto error_code = e.GetCode();
        const auto& message = e.GetMessage();

        switch (static_cast<ErrorCode>(error_code)) {
            case ErrorCode::INVALID_PARAMETER : {
                throw InvalidParameters(message);
            }
            case ErrorCode::INVALID_ARGUMENT : {
                throw InvalidParameters(message);
            }
            case ErrorCode::NOT_IMPLEMENTED : {
                throw NotImplemented(message);
            }
            case ErrorCode::NOT_FOUND : {
                throw NotFound(message);
            }
            case ErrorCode::INVALID_UUID : {
                throw InvalidUuid(message);
            }
            case ErrorCode::ISCSI : {
                throw IscsiError(message);
            }
            case ErrorCode::LVM : {
                throw LvmError(message);
            }
            case ErrorCode::FM10000 : {
                throw Fm10000Error(message);
            }
            case ErrorCode::UNSUPPORTED_VALUE : {
                throw UnsupportedValue(message);
            }
            case ErrorCode::UNSUPPORTED_PARAMETER : {
                throw UnsupportedParameter(message);
            }
            case ErrorCode::CERTIFICATE : {
                throw CertificateError(message);
            }
            case ErrorCode::UNKNOWN_ERROR :
            default :
                throw;
        }
    }
}

RpcClient::~RpcClient() { }

