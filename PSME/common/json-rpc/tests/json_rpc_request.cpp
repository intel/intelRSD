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
 * @file tests/json_rpc_request.cpp
 */

#include "json-rpc/json_rpc_request.hpp"

#include "gtest/gtest.h"

using namespace json_rpc;

namespace {

void positive_request_test(JsonRpcRequest& req, const json::Json& input, const std::string& desc) {
    try {
        req = JsonRpcRequest::from_json(input);
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

void negative_request_test(const json::Json& input, const std::string& desc) {
    try {
        JsonRpcRequest::from_json(input);
    }
    catch (const JsonRpcException& e) {
        ASSERT_EQ(common::ERROR_RPC_INVALID_REQUEST, e.get_code()) << "Invalid exception type, test = "
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

TEST(JsonRpcRequestTest, NegativeSerialization) {
    negative_request_test({}, "Empty json test");

    JsonRpcRequest request = JsonRpcRequest::make_notification_request("test");
    json::Json good_json = {
        {"jsonrpc", "2.0"},
        {"method", "testMethod"}
    };
    positive_request_test(request, good_json, "pre negative tests check");

    auto test_json = good_json;
    test_json.erase("jsonrpc");
    negative_request_test(test_json, "jsonrpc is required");
    test_json = good_json;
    test_json["jsonrpc"] = 2;
    negative_request_test(test_json, "jsonrpc must be a string");
    test_json = good_json;
    test_json["jsonrpc"] = "11";
    negative_request_test(test_json, "jsonrpc must be equal to \"2.0\"");
    test_json = good_json;
    test_json["id"] = true;
    negative_request_test(test_json, "if present, id field must be null, string or int");
    test_json = good_json;
    test_json["id"] = json::Json::array();
    negative_request_test(test_json, "if present, id field must be null, string or int");
    test_json = good_json;
    test_json["id"] = json::Json::object();
    negative_request_test(test_json, "if present, id field must be null, string or int");
    test_json = good_json;
    test_json["params"] = true;
    negative_request_test(test_json, "if present, params field must be an array or an object");
    test_json = good_json;
    test_json["params"] = nullptr;
    negative_request_test(test_json, "if present, params field must be an array or an object");
    test_json = good_json;
    test_json["params"] = "invalid_params";
    negative_request_test(test_json, "if present, params field must be an array or an object");
    test_json = good_json;
    test_json["params"] = 234;
    negative_request_test(test_json, "if present, params field must be an array or an object");
    test_json = good_json;
    test_json.erase("method");
    negative_request_test(test_json, "method field is required");
    test_json = good_json;
    test_json["method"] = 123;
    negative_request_test(test_json, "method field must be a string");
    test_json = good_json;
    test_json["test"] = 123;
    negative_request_test(test_json, "unexpected fields in request object (no id, no params)");
    test_json = good_json;
    test_json["id"] = 22;
    test_json["test"] = 123;
    negative_request_test(test_json, "unexpected fields in request object (no params)");
    test_json = good_json;
    test_json["params"] = 22;
    test_json["test"] = 123;
    negative_request_test(test_json, "unexpected fields in request object (no id)");
    test_json = good_json;
    test_json["id"] = 33;
    test_json["params"] = 22;
    test_json["test"] = 123;
    negative_request_test(test_json, "unexpected fields in request object (full)");
}

TEST(JsonRpcRequestTest, PositiveSerializationValues) {
    JsonRpcRequest req = JsonRpcRequest::make_notification_request("test_method");
    json::Json test_json = {
        {"jsonrpc", "2.0"},
        {"id", 123},
        {"method", "method_value"},
        {"params", {
            {"test_param", "test_value"}
        }}
    };
    positive_request_test(req, test_json, "valid json");
    ASSERT_TRUE(req.get_id().is_number());
    ASSERT_EQ(req.get_id(), 123);
    ASSERT_EQ(req.get_method(), "method_value");
    ASSERT_TRUE(req.get_params().is_object());
    ASSERT_EQ(req.get_params()["test_param"], "test_value");
}

TEST(JsonRpcRequestTest, PositiveSerialization) {
    JsonRpcRequest req = JsonRpcRequest::make_notification_request("test_method");
    json::Json good_json = {
        {"jsonrpc", "2.0"},
        {"method", "test_method"}
    };
    positive_request_test(req, good_json, "Pre positive checks test");
    auto test_json = good_json;
    test_json["id"] = 123;
    positive_request_test(req, test_json, "id may be a number");
    test_json = good_json;
    test_json["id"] = "test_id";
    positive_request_test(req, test_json, "id may be string");
    test_json = good_json;
    test_json["id"] = nullptr;
    positive_request_test(req, test_json, "id may be null");
    test_json = good_json;
    test_json["params"] = json::Json::object();
    test_json["params"]["test"] = 10;
    positive_request_test(req, test_json, "params may be an object");
    test_json = good_json;
    test_json["params"] = json::Json::array();
    test_json["params"].emplace_back(10);
    positive_request_test(req, test_json, "params may be an array");
}

}
