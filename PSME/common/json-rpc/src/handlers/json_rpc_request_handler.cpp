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
 * @file handlers/json_rpc_request_handler.cpp
 */

#include "json-rpc/handlers/json_rpc_request_handler.hpp"

#include "json-rpc/json_rpc_error.hpp"
#include "json-rpc/json_rpc_request.hpp"
#include "json-rpc/json_rpc_response.hpp"

using namespace json_rpc;

JsonRpcRequestHandler::JsonRpcRequestHandler() {}

JsonRpcRequestHandler::~JsonRpcRequestHandler() {}

bool JsonRpcRequestHandler::handle_single_request(const json::Json& json_request, json::Json& json_response) {

    bool send_response = true;
    json::Json id{};
    try {
        // parse request
        JsonRpcRequest request = JsonRpcRequest::from_json(json_request);
        id = request.get_id();

        // request is correct, dispatch it to the handler
        send_response = !request.is_notification();
        const auto& method = request.get_method();

        // notification -> find handler, execute, return no response
        if (request.is_notification()) {
            NotificationHandler handler = get_notification_handler(method);
            show_info("RPC Notification call: " + method);
            handler(request.get_params());
        }
        // method -> find handler, execute, return response
        else {
            MethodHandler handler = get_method_handler(method);
            show_info("RPC Method call: " + method);
            auto result = handler(request.get_params());
            json_response = JsonRpcResponse(result, id).to_json();
        }

    }
    catch (const JsonRpcException& e) {
        json_response = JsonRpcError(e, id).to_json();
        show_error(std::string{"RPC Error : "} + e.what());
    }
    catch (const std::exception& e) {
        json_response = JsonRpcError(common::ERROR_UNKNOWN, std::string{"Unexpected exception: "} + e.what(), id).to_json();
        show_error(std::string{"RPC Error : "} + e.what());
    }
    catch (...) {
        json_response = JsonRpcError(common::ERROR_UNKNOWN, "Unexpected exception", id).to_json();
        show_error("RPC Error : unexpected exception");
    }
    return send_response;
}

std::string JsonRpcRequestHandler::handle(const std::string& input_request) {
    json::Json request{};
    try {
        request = json::Json::parse(input_request);
    }
    catch (const std::exception& e) {
        // response for invalid json is always an error
        auto json_e = JsonRpcException(common::ERROR_RPC_JSON_PARSE_ERROR, e.what());
        auto error = JsonRpcError(json_e, json::Json{}).to_json().dump();
        show_error("RPC Error " + error);
        return error;
    }

    if (request.is_array()) {
        // json is an array = batch request, we have to return an array, if request is empty - return error
        if (request.empty()) {
            return JsonRpcError(JsonRpcException(common::ERROR_RPC_INVALID_REQUEST), json::Json{}).to_json().dump();
        }
        json::Json response = json::Json::array();
        for (const auto& sub_request : request) {
            // handle each request
            json::Json sub_response{};
            if (handle_single_request(sub_request, sub_response)) {
                // response is validate_type
                response.emplace_back(std::move(sub_response));
            }
        }
        if (response.size() == 0) {
            // no responses were added, return nothing
            return std::string{};
        }
        return response.dump();
    }
    else {
        // single request
        json::Json response{};
        if (handle_single_request(request, response)) {
            return response.dump();
        }
        return std::string{};
    }
}
