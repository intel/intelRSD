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
 * @file common.cpp
 */

#include "json-rpc/common.hpp"

using namespace json_rpc;
using namespace json_rpc::common;

namespace {

static std::map<int, std::string> json_rpc_error_messages = {
    {ERROR_RPC_JSON_PARSE_ERROR, "JSON_PARSE_ERROR: The JSON-Object is not JSON-Valid"},
    {ERROR_RPC_METHOD_NOT_FOUND, "METHOD_NOT_FOUND: The method being requested is not available on this server"},
    {ERROR_RPC_INVALID_REQUEST, "INVALID_JSON_REQUEST: The JSON sent is not a valid JSON-RPC Request object"},
    {ERROR_RPC_INVALID_PARAMS, "INVALID_PARAMS: Invalid method parameters (invalid name and/or type) recognised"},
    {ERROR_RPC_INTERNAL_ERROR, "INTERNAL_ERROR: "},
    {ERROR_CLIENT_CONNECTOR, "Client connector error"},
    {ERROR_SERVER_CONNECTOR, "Server connector error"},
    {ERROR_CLIENT_INVALID_RESPONSE, "The response is invalid"},
    {ERROR_UNKNOWN, "Unknown exception occurred in JSON command method"}
};

}

std::string json_rpc::common::get_error_message(int error_code) {
    try {
        return ::json_rpc_error_messages.at(error_code);
    }
    catch (std::out_of_range&) {
        return std::string("Unknown value: " + std::to_string(error_code));
    }
}
