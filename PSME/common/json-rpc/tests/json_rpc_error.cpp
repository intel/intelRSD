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
 * @file tests/json_rpc_error.cpp
 */

#include "json-rpc/json_rpc_error.hpp"

#include "gtest/gtest.h"

using namespace json_rpc;

namespace {

void positive_error_test(JsonRpcError& er, const json::Json& input, const std::string& desc) {
    try {
        er = JsonRpcError::from_json(input);
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

void negative_error_test(const json::Json& input, const std::string& desc) {
    try {
        JsonRpcError::from_json(input);
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

TEST(JsonRpcErrorTest, NegativeSerialization) {
    negative_error_test({}, "Empty json test");

    JsonRpcError error = JsonRpcError(0, "", json::Json());
    json::Json good_json = {
        {"jsonrpc", "2.0"},
        {"id", "id_value"},
        {"error", {
            {"code", 137},
            {"message", "message_value"},
            {"data", "data_value"}
        }}
    };
    positive_error_test(error, good_json, "pre negative tests check");

    auto test_json = good_json;
    test_json.erase("jsonrpc");
    negative_error_test(test_json, "jsonrpc is required");
    test_json = good_json;
    test_json["jsonrpc"] = 2;
    negative_error_test(test_json, "jsonrpc must be a string");
    test_json = good_json;
    test_json["jsonrpc"] = "11";
    negative_error_test(test_json, "jsonrpc must be equal to \"2.0\"");
    test_json = good_json;
    test_json.erase("id");
    negative_error_test(test_json, "id field is required");
    test_json = good_json;
    test_json["id"] = true;
    negative_error_test(test_json, "id field must be null, string or int");
    test_json = good_json;
    test_json["error"].erase("code");
    negative_error_test(test_json, "code field is required");
    test_json = good_json;
    test_json["error"]["code"] = "test";
    negative_error_test(test_json, "code field must be a number");
    test_json = good_json;
    test_json["error"]["code"] = 123.1;
    negative_error_test(test_json, "code field must be integer");
    test_json = good_json;
    test_json["error"].erase("message");
    negative_error_test(test_json, "message field is required");
    test_json = good_json;
    test_json["error"]["message"] = 123;
    negative_error_test(test_json, "message field must be a string");
    test_json = good_json;
    test_json["error"]["add"] = 123;
    negative_error_test(test_json, "unexpected fields in error object");
    test_json = good_json;
    test_json["error"].erase("data");
    test_json["error"]["add"] = 123;
    negative_error_test(test_json, "unexpected fields in error object (no data)");
    test_json = good_json;
    test_json["add"] = 123;
    negative_error_test(test_json, "unexpected fields in the json");
    test_json = good_json;
    test_json["result"] = 123;
    negative_error_test(test_json, "result field cannot be present");
}

TEST(JsonRpcErrorTest, PositiveSerializationValues) {
    JsonRpcError error = JsonRpcError(0, "", json::Json());
    json::Json test_json = {
        {"jsonrpc", "2.0"},
        {"id", "id_value"},
        {"error", {
            {"code", 137},
            {"message", "message_value"},
            {"data", "data_value"}
        }}
    };
    positive_error_test(error, test_json, "valid json");
    ASSERT_EQ(error.get_code(), 137);
    ASSERT_EQ(error.get_message(), "message_value");
    ASSERT_TRUE(error.get_id().is_string());
    ASSERT_EQ(error.get_id(), "id_value");
    ASSERT_TRUE(error.get_data().is_string());
    ASSERT_EQ(error.get_data(), "data_value");
}

TEST(JsonRpcErrorTest, PositiveSerialization) {
    JsonRpcError error = JsonRpcError(0, "", json::Json());
    json::Json good_json = {
        {"jsonrpc", "2.0"},
        {"id", "id_value"},
        {"error", {
            {"code", 137},
            {"message", "message_value"},
            {"data", "data_value"}
        }}
    };
    positive_error_test(error, good_json, "Pre positive checks test");
    auto test_json = good_json;
    test_json["id"] = 123;
    positive_error_test(error, test_json, "id may be a number");
    test_json = good_json;
    test_json["id"] = nullptr;
    positive_error_test(error, test_json, "id may be null");
    test_json = good_json;
    test_json["error"].erase("data");
    positive_error_test(error, test_json, "data is optional");
    test_json = good_json;
    test_json["error"]["data"] = nullptr;
    positive_error_test(error, test_json, "data may be null");
    test_json = good_json;
    test_json["error"]["data"] = true;
    positive_error_test(error, test_json, "data may be bool");
    test_json = good_json;
    test_json["error"]["data"] = 123.2;
    positive_error_test(error, test_json, "data may be numeric");
    test_json = good_json;
    test_json["error"]["data"] = "123";
    positive_error_test(error, test_json, "data may be string");
    test_json = good_json;
    test_json["error"]["data"] = json::Json::object();
    test_json["error"]["data"]["test"] = 1;
    positive_error_test(error, test_json, "data may be an object");
    test_json = good_json;
    test_json["error"]["data"] = json::Json::array();
    test_json["error"]["data"].emplace_back(10);
    positive_error_test(error, test_json, "data may be an array");
}

TEST(JsonRpcErrorTest, CorrectConvertionToAnException) {
    JsonRpcError error = JsonRpcError(0, "", json::Json());
    json::Json test_json = {
        {"jsonrpc", "2.0"},
        {"id", "id_value"},
        {"error", {
            {"code", common::ERROR_RPC_JSON_PARSE_ERROR},
            {"message", "message_value"},
            {"data", "data_value"}
        }}
    };
    positive_error_test(error, test_json, "valid json");

    JsonRpcException exception = JsonRpcException(error);
    ASSERT_EQ(exception.get_code(), common::ERROR_RPC_JSON_PARSE_ERROR);
    ASSERT_EQ(exception.get_message(), error.get_message());
    ASSERT_EQ(exception.get_data(), "data_value");
}

TEST(JsonRpcErrorTest, CorrectConvertionFromAnException) {
    JsonRpcException exception(common::ERROR_RPC_JSON_PARSE_ERROR, "message", "data");
    JsonRpcError error = JsonRpcError(exception, "123");
    ASSERT_EQ(error.get_code(), exception.get_code());
    ASSERT_EQ(error.get_data(), exception.get_data());
    ASSERT_EQ(error.get_message(), exception.get_message());
    ASSERT_EQ(error.get_id(), "123");
}

TEST(JsonRpcErrorTest, CorrectDoubleConvertion) {
    JsonRpcException exception(common::ERROR_RPC_JSON_PARSE_ERROR, "message", "data");
    JsonRpcError error = JsonRpcError(exception, "123");
    JsonRpcException new_exception = JsonRpcException(error);
    ASSERT_EQ(exception.get_code(), new_exception.get_code());
    ASSERT_EQ(exception.get_message(), new_exception.get_message());
    ASSERT_EQ(exception.get_data(), new_exception.get_data());
}

}
