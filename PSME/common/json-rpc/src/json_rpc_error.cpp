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
 * @file json_rpc_error.cpp
 */

#include "json-rpc/json_rpc_error.hpp"
#include "json-rpc/json_rpc_exception.hpp"

using namespace json_rpc;
using namespace json_rpc::common;

namespace {

void check(bool condition, const std::string& message) {
    if (!condition) {
        throw JsonRpcException(ERROR_CLIENT_INVALID_RESPONSE, message);
    }
}

}

void JsonRpcError::throw_if_invalid_json(const json::Json& json) {

    check(json.is_object(), "JSON is not an object");
    check(json.count(KEY_JSONRPC), "Required 'jsonrpc' field is missing");
    check(json[KEY_JSONRPC].is_string(), "'jsonrpc' field is not a string");
    check(json[KEY_JSONRPC] == KEY_JSONRPC_VALUE, "'jsonrpc' field is not equal to '2.0'");
    check(json.count(KEY_ID), "Required 'id' field is missing");
    check(json[KEY_ID].is_string() || json[KEY_ID].is_number() || json[KEY_ID].is_null(),
        "'id' field is not a string, number or null");
    check(json.count(KEY_ERROR), "Required 'error' field is missing");
    check(json[KEY_ERROR].is_object(), "'error' field is not an object");
    check(json[KEY_ERROR].count(KEY_CODE), "Required 'error/code' field is missing");
    check(json[KEY_ERROR][KEY_CODE].is_number_integer(), "'error/code' field is not an integer");
    check(json[KEY_ERROR].count(KEY_MESSAGE), "Required 'error/message' field is missing");
    check(json[KEY_ERROR][KEY_MESSAGE].is_string(), "'error/message' field is not a string");

    unsigned error_expected_size = 2 + (json[KEY_ERROR].count(KEY_DATA) ? 1 : 0);
    unsigned json_expected_size = 3;

    check(json[KEY_ERROR].size() == error_expected_size, "Unexpected fields found in the 'error' object");
    check(json.size() == json_expected_size, "Unexpected fields found in the response JSON object");
}

JsonRpcError JsonRpcError::from_json(const json::Json& json) {

    throw_if_invalid_json(json);

    JsonRpcError ret{};
    ret.m_id = json[KEY_ID];
    ret.m_code = json[KEY_ERROR][KEY_CODE];
    ret.m_message = json[KEY_ERROR][KEY_MESSAGE];
    ret.m_data = json[KEY_ERROR].value(KEY_DATA, json::Json{});
    return ret;
}

json::Json JsonRpcError::to_json() const {
    json::Json ret{};
    ret[KEY_JSONRPC] = KEY_JSONRPC_VALUE;
    ret[KEY_ID] = m_id;
    ret[KEY_ERROR][KEY_CODE] = m_code;
    ret[KEY_ERROR][KEY_MESSAGE] = m_message;
    if (m_data_present) {
        ret[KEY_ERROR][KEY_DATA] = m_data;
    }
    return ret;
}
