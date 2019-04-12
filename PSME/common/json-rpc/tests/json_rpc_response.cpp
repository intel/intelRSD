/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file tests/json_rpc_response.cpp
 */

#include "json-rpc/json_rpc_response.hpp"

#include "gtest/gtest.h"

using namespace json_rpc;

namespace {

void positive_response_test(JsonRpcResponse& er, const json::Json& input, const std::string& desc) {
    try {
        er = JsonRpcResponse::from_json(input);
    }
    catch (const JsonRpcException& e) {
        FAIL() << "JsonRpcException caught during serialization of: " << input.dump()
               << ", what: " << e.what() << ", test = " << desc;
    }
    catch (const std::exception& e) {
        FAIL() << "Unknown exception caught during serialization of: " << input.dump()
               << ", what: " << e.what() << ", test = " << desc;
    }
    catch (...) {
        FAIL() << "Unknown exception caught during serialization of: " << input.dump()
              << ", test = " << desc;
    }
}

void negative_response_test(const json::Json& input, const std::string& desc) {
    try {
        JsonRpcResponse::from_json(input);
    }
    catch (const JsonRpcException& e) {
        ASSERT_EQ(common::ERROR_CLIENT_INVALID_RESPONSE, e.get_code()) << "Invalid exception type, test = "
            << desc;
        return;
    }
    catch (const std::exception& e) {
        FAIL() << "Unknown exception caught during serialization of: " << input.dump()
               << ", what: " << e.what() << ", test = " << desc;
    }
    catch (...) {
        FAIL() << "Unknown exception caught during serialization of: " << input.dump()
               << ", test = " << desc;
    }
    FAIL() << "Invalid json passed validation: " << input.dump() << ", test = " << desc;
}

}

namespace testing {

TEST(JsonRpcResponseTest, NegativeSerialization) {
    negative_response_test({}, "Empty json test");

    JsonRpcResponse rsp = JsonRpcResponse(json::Json{}, "");
    json::Json good_json = {
        {"jsonrpc", "2.0"},
        {"id", "id_value"},
        {"result", "123"}
    };
    positive_response_test(rsp, good_json, "pre negative tests check");

    auto test_json = good_json;
    test_json.erase("jsonrpc");
    negative_response_test(test_json, "jsonrpc is required");
    test_json = good_json;
    test_json["jsonrpc"] = 2;
    negative_response_test(test_json, "jsonrpc must be a string");
    test_json = good_json;
    test_json["jsonrpc"] = "11";
    negative_response_test(test_json, "jsonrpc must be equal to \"2.0\"");
    test_json = good_json;
    test_json.erase("id");
    negative_response_test(test_json, "id field is required");
    test_json = good_json;
    test_json["id"] = true;
    negative_response_test(test_json, "id field must be null, string or int");
    test_json = good_json;
    test_json.erase("result");
    negative_response_test(test_json, "result field is required");
    test_json = good_json;
    test_json["add"] = 123;
    negative_response_test(test_json, "unexpected fields in the json");
    test_json = good_json;
    test_json["error"] = 123;
    negative_response_test(test_json, "error field cannot be present");
}

TEST(JsonRpcResponseTest, PositiveSerializationValues) {
    JsonRpcResponse rsp = JsonRpcResponse(json::Json(), 3);
    json::Json test_json = {
        {"jsonrpc", "2.0"},
        {"id", 123},
        {"result", "result_value"}
    };
    positive_response_test(rsp, test_json, "valid json");
    ASSERT_TRUE(rsp.get_id().is_number());
    ASSERT_EQ(rsp.get_id(), 123);
    ASSERT_EQ(rsp.get_result(), "result_value");
}

TEST(JsonRpcResponseTest, PositiveSerialization) {
    JsonRpcResponse rsp = JsonRpcResponse(json::Json(), 3);
    json::Json good_json = {
        {"jsonrpc", "2.0"},
        {"id", "id_value"},
        {"result", nullptr}
    };
    positive_response_test(rsp, good_json, "Pre positive checks test");
    auto test_json = good_json;
    test_json["id"] = 123;
    positive_response_test(rsp, test_json, "id may be a number");
    test_json = good_json;
    test_json["id"] = nullptr;
    positive_response_test(rsp, test_json, "id may be null");
    test_json = good_json;
    test_json["result"] = nullptr;
    positive_response_test(rsp, test_json, "data may be null");
    test_json = good_json;
    test_json["result"] = true;
    positive_response_test(rsp, test_json, "data may be bool");
    test_json = good_json;
    test_json["result"] = 1342.2;
    positive_response_test(rsp, test_json, "data may be numeric");
    test_json = good_json;
    test_json["result"] = "string value";
    positive_response_test(rsp, test_json, "data may be string");
    test_json = good_json;
    test_json["result"] = json::Json::object();
    test_json["result"]["test"] = 1;
    positive_response_test(rsp, test_json, "data may be an object");
    test_json = good_json;
    test_json["result"] = json::Json::array();
    test_json["result"].emplace_back(10);
    positive_response_test(rsp, test_json, "data may be an array");
}

}
