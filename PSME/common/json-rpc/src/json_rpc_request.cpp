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
 * @file json_rpc_request.cpp
 */

#include "json-rpc/json_rpc_request.hpp"

using namespace json_rpc;
using namespace json_rpc::common;

namespace {

void check(bool condition, const std::string& message) {
    if (!condition) {
        throw JsonRpcException(ERROR_RPC_INVALID_REQUEST, message);
    }
}

}

void JsonRpcRequest::throw_if_invalid_json(const json::Json& json) {

    check(json.is_object(), "JSON is not an object");
    check(json.count(KEY_JSONRPC), "Required 'jsonrpc' field is missing");
    check(json[KEY_JSONRPC].is_string(), "'jsonrpc' field is not a string");
    check(json[KEY_JSONRPC] == KEY_JSONRPC_VALUE, "'jsonrpc' field is not equal to '2.0'");

    check(json.count(KEY_METHOD), "Required 'method' field is missing");
    check(json[KEY_METHOD].is_string(), "'method' field is not a string");

    unsigned expected_size = 2;
    // optional field
    if (json.count(KEY_ID)) {
        ++expected_size;
        check(json[KEY_ID].is_string() || json[KEY_ID].is_number() || json[KEY_ID].is_null(),
            "'id' field is not a string, number or null");
    }
    if (json.count(KEY_PARAMS)) {
        ++expected_size;
        check(json[KEY_PARAMS].is_structured(), "'params' is not an array or object");
    }
    check(json.size() == expected_size, "Unexpected fields found in the request JSON object");
}

JsonRpcRequest JsonRpcRequest::from_json(const json::Json& json) {

    throw_if_invalid_json(json);

    JsonRpcRequest ret{};
    // 'id' field is optional
    if (!json.count(KEY_ID)) {
        ret.m_id_present = false;
    }
    // 'params' field is optional, but if present, must be structured
    if (!json.count(KEY_PARAMS)) {
        ret.m_params_present = false;
    }

    ret.m_method = json[KEY_METHOD];
    ret.m_id = json.value(KEY_ID, json::Json{});
    ret.m_params = json.value(KEY_PARAMS, json::Json{});
    return ret;
}

json::Json JsonRpcRequest::to_json() const {
    json::Json ret{};
    ret[KEY_JSONRPC] = KEY_JSONRPC_VALUE;
    ret[KEY_METHOD] = m_method;
    if (m_id_present) {
        ret[KEY_ID] = m_id;
    }
    if (m_params_present) {
        ret[KEY_PARAMS] = m_params;
    }
    return ret;
}
