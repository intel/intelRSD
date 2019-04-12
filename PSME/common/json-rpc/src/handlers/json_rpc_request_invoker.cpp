/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file handlers/json_rpc_request_invoker.cpp
 */

#include "json-rpc/handlers/json_rpc_request_invoker.hpp"

#include "json-rpc/json_rpc_error.hpp"
#include "json-rpc/json_rpc_response.hpp"

using namespace json_rpc;

void JsonRpcRequestInvoker::prepare_method(const std::string& method, const json::Json& params) {
    m_is_notification = false;
    if (params.is_null()) {
        m_request = JsonRpcRequest::make_method_request(method, json::Json(m_last_id));
    }
    else {
        m_request = JsonRpcRequest::make_method_request(method, json::Json(m_last_id), params);
    }

}

void JsonRpcRequestInvoker::prepare_notification(const std::string& method, const json::Json& params) {
    m_is_notification = true;
    if (params.is_null()) {
        m_request = JsonRpcRequest::make_notification_request(method);
    }
    else {
        m_request = JsonRpcRequest::make_notification_request(method, params);
    }
}

void JsonRpcRequestInvoker::call(AbstractClientConnectorPtr connector) {
    std::string raw_response = connector->send_request(m_request.to_json().dump());
    if (m_is_notification) {
        // no response for notifications
        m_result = json::Json{};
        return;
    }

    // process response for methods
    json::Json response{};
    try {
        response = json::Json::parse(raw_response);
    }
    catch (const std::exception& e) {
        // this is an invalid json, throw parsing error
        throw JsonRpcException(common::ERROR_RPC_JSON_PARSE_ERROR, e.what());
    }

    // here we know response is a valid json
    JsonRpcResponse rpc_response(json::Json{}, json::Json{});
    try {
        rpc_response = JsonRpcResponse::from_json(response);
    }
    catch (const JsonRpcException&) {
        // this is not a valid response - check if this is an error

        JsonRpcError rpc_error{0, "", {}};
        try {
            rpc_error = JsonRpcError::from_json(response);
        }
        catch (const JsonRpcException&) {
            // it is not a valid error - show
            throw JsonRpcException(common::ERROR_CLIENT_INVALID_RESPONSE,
                std::string{"Returned JSON is not not a valid response or error: "} + raw_response);
        }
        // if we are here - this was a valid error, make exception out of it
        throw JsonRpcException(rpc_error);
    }
    // response is correctly parsed - check id
    if (rpc_response.get_id() != m_request.get_id()) {
        throw JsonRpcException(common::ERROR_CLIENT_INVALID_RESPONSE,
            "Id returned by the server is invalid");
    }
    m_result = rpc_response.get_result();
}
