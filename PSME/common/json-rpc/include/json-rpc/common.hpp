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
 * @file common.hpp
 */

#pragma once

#include "json-wrapper/json-wrapper.hpp"
#include "json-rpc/json_rpc_exception.hpp"

#include <string>

namespace json_rpc {
namespace common {

/* Official JSON-RPC 2.0 Errors */
constexpr int ERROR_RPC_JSON_PARSE_ERROR = -32700;
constexpr int ERROR_RPC_METHOD_NOT_FOUND = -32601;
constexpr int ERROR_RPC_INVALID_REQUEST  = -32600;
constexpr int ERROR_RPC_INVALID_PARAMS   = -32602;
constexpr int ERROR_RPC_INTERNAL_ERROR   = -32603;

/* Library Errors */
constexpr int ERROR_CLIENT_CONNECTOR        = -32003;
constexpr int ERROR_SERVER_CONNECTOR        = -32002;
constexpr int ERROR_CLIENT_INVALID_RESPONSE = -32001;

constexpr int ERROR_UNKNOWN = -1;

/*!
 * @brief Gets the error message based on its code
 * @param error_code Error code
 * @return Message describing the error
 */
std::string get_error_message(int error_code);

constexpr char KEY_JSONRPC[] = "jsonrpc";
constexpr char KEY_JSONRPC_VALUE[] = "2.0";
constexpr char KEY_ID[] = "id";
constexpr char KEY_PARAMS[] = "params";
constexpr char KEY_METHOD[] = "method";
constexpr char KEY_RESULT[] = "result";
constexpr char KEY_ERROR[] = "error";
constexpr char KEY_CODE[] = "code";
constexpr char KEY_MESSAGE[] = "message";
constexpr char KEY_DATA[] = "data";

}
}
