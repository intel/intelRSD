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
 * @file tests/json_rpc_request_handler.cpp
 */

#include "json-rpc/handlers/json_rpc_request_handler.hpp"
#include "json-rpc/connectors/abstract_server_connector.hpp"

#include "json-rpc/json_rpc_request.hpp"
#include "json-rpc/json_rpc_error.hpp"
#include "json-rpc/json_rpc_response.hpp"

#include "gtest/gtest.h"

#include <functional>

using namespace json_rpc;

namespace {

/*!
 * This is a mock server connector that simulates a real server.
 * Using it, it is possible to check if JsonRpcRequestHandler properly handler json-rpc calls
 * and reacts to exceptions (e.g. by building proper errors)
 */
class MockServerConnector : public AbstractServerConnector {
public:

    using MethodHandler = JsonRpcRequestHandler::MethodHandler;
    using NotificationHandler = JsonRpcRequestHandler::NotificationHandler;

    MockServerConnector() {}
    ~MockServerConnector() {}

    /* Not used, but required by the interface */
    virtual bool start_listening() override { return true; }
    virtual void stop_listening() override {}
    virtual void set_url_handler(const std::string&, AbstractRequestHandlerPtr) override { }
    virtual AbstractRequestHandlerPtr get_url_handler(const std::string&) const override { return nullptr; }

    void add_method(const std::string& name, const MethodHandler& handler) {
        m_handler.set_method_handler(name, handler);
    }

    void add_notification(const std::string& name, const NotificationHandler& handler) {
        m_handler.set_notification_handler(name, handler);
    }

    std::string call(const JsonRpcRequest& req) {
        return m_handler.handle(req.to_json().dump());
    }

    std::string call(const std::string& req) {
        return m_handler.handle(req);
    }

private:

    JsonRpcRequestHandler m_handler{};
};

JsonRpcResponse parse_response(const std::string& in) {
    return JsonRpcResponse::from_json(json::Json::parse(in));
}

void call_assert_exception_and_test(MockServerConnector& server, const std::string& request, long expected_code,
        const json::Json& expected_id) {
    std::string str_response{};
    str_response = server.call(request);
    JsonRpcError error(-1, {}, {});
    try {
        error = JsonRpcError::from_json(json::Json::parse(str_response));
    }
    catch (...) {
        FAIL() << "Unexpected exception, with request: " << request << ", response = " << str_response;
    }
    ASSERT_EQ(error.get_code(), expected_code) << "Failed code test with request: " << request
        << ", response: " << str_response;
    ASSERT_TRUE(error.get_id() == expected_id) << "Failed id test with request: " << request
        << ", response: " << str_response;
}

}

namespace testing {

/*
 * Checks if correct methods are called when needed. Methods/notifications are tested with all possible
 * parameter types: no params, objects and arrays.
 */
TEST(JsonRpcRequestHandlerTest, CheckValidCalls) {
    std::vector<int> call_flags{0, 0, 0, 0};
    MockServerConnector server{};
    server.add_method("method_1", [&call_flags](const json::Json&) -> json::Json {
        call_flags[0]++;
        return {};
    });
    server.add_method("method_2", [&call_flags](const json::Json&) -> json::Json {
        call_flags[1]++;
        return {};
    });
    server.add_notification("notify_1", [&call_flags](const json::Json&) {
        call_flags[2]++;
    });
    server.add_notification("notify_2", [&call_flags](const json::Json&) {
        call_flags[3]++;
    });

    JsonRpcRequest method_1n = JsonRpcRequest::make_method_request("method_1", 0);
    JsonRpcRequest method_1 = JsonRpcRequest::make_method_request("method_1", 0, {1, 2, 3, 4});
    JsonRpcRequest method_2 = JsonRpcRequest::make_method_request("method_2", 0, {{"data"}, {"value"}});
    JsonRpcRequest notify_1n = JsonRpcRequest::make_notification_request("notify_1");
    JsonRpcRequest notify_1 = JsonRpcRequest::make_notification_request("notify_1", {1, 2, 3, 4});
    JsonRpcRequest notify_2 = JsonRpcRequest::make_notification_request("notify_2", {{"data"}, {"value"}});

    ASSERT_EQ(call_flags, std::vector<int>({0, 0, 0, 0}));
    server.call(method_1n);
    ASSERT_EQ(call_flags, std::vector<int>({1, 0, 0, 0}));
    server.call(method_2);
    ASSERT_EQ(call_flags, std::vector<int>({1, 1, 0, 0}));
    server.call(method_1);
    ASSERT_EQ(call_flags, std::vector<int>({2, 1, 0, 0}));
    server.call(notify_2);
    ASSERT_EQ(call_flags, std::vector<int>({2, 1, 0, 1}));
    server.call(notify_2);
    ASSERT_EQ(call_flags, std::vector<int>({2, 1, 0, 2}));
    server.call(notify_1n);
    ASSERT_EQ(call_flags, std::vector<int>({2, 1, 1, 2}));
    server.call(notify_1);
    ASSERT_EQ(call_flags, std::vector<int>({2, 1, 2, 2}));
    server.call(method_2);
    ASSERT_EQ(call_flags, std::vector<int>({2, 2, 2, 2}));
    server.call(method_1);
    ASSERT_EQ(call_flags, std::vector<int>({3, 2, 2, 2}));
}

/*
 * Checks if server packs/unpacks requests/responses/ids correctly, nothing is returned on notifications etc.
 */
TEST(JsonRpcRequestHandlerTest, CheckRequestResponsePackingUnpacking) {
    int calls_passed = 0;
    MockServerConnector server{};
    server.add_method("m_null", [&calls_passed](const json::Json& in) -> json::Json {
        if (in.is_null()) {
            calls_passed++;
        }
        return json::Json("return_value");
    });
    server.add_method("m_obj", [&calls_passed](const json::Json& in) -> json::Json {
        if (in.is_object() && in.size() == 2
            && in.count("p1") && in["p1"].is_string() && in["p1"] == "v1"
            && in.count("p2") && in["p2"].is_number() && in["p2"] == 10) {

            calls_passed++;
        }
        return {{"string", "value"}, {"bool", true}, {"number", 22}};
    });
    server.add_method("m_arr", [&calls_passed](const json::Json& in) -> json::Json {
        if (in.is_array() && in.size() == 2
            && in[0].is_string() && in[0] == "v1"
            && in[1].is_number() && in[1] == 10) {

            calls_passed++;
        }
        return {"value2", false, 33};
    });
    server.add_notification("n_null", [&calls_passed](const json::Json& in) {
        if (in.is_null()) {
            calls_passed++;
        }
    });
    server.add_notification("n_obj", [&calls_passed](const json::Json& in) {
        if (in.is_object() && in.size() == 2
            && in.count("p1") && in["p1"].is_string() && in["p1"] == "v1"
            && in.count("p2") && in["p2"].is_number() && in["p2"] == 10) {

            calls_passed++;
        }
    });
    server.add_notification("n_arr", [&calls_passed](const json::Json& in) {
        if (in.is_array() && in.size() == 2
            && in[0].is_string() && in[0] == "v1"
            && in[1].is_number() && in[1] == 10) {

            calls_passed++;
        }
    });

    std::string str_response{};
    JsonRpcResponse response({}, {});
    ASSERT_EQ(calls_passed, 0);

    str_response = server.call(JsonRpcRequest::make_method_request("m_null", 220));
    ASSERT_EQ(calls_passed, 1);
    ASSERT_NO_THROW(response = parse_response(str_response));
    ASSERT_EQ(response.get_id(), 220);
    ASSERT_TRUE(response.get_result().is_string());
    ASSERT_TRUE(response.get_result() == "return_value");

    str_response = server.call(JsonRpcRequest::make_method_request("m_obj", "220", { {"p1", "v1"}, {"p2", 10} }));
    ASSERT_EQ(calls_passed, 2);
    ASSERT_NO_THROW(response = parse_response(str_response));
    ASSERT_TRUE(response.get_id() == "220");
    ASSERT_EQ(response.get_result().size(), 3);
    ASSERT_EQ(response.get_result().count("string"), 1);
    ASSERT_TRUE(response.get_result()["string"].is_string());
    ASSERT_TRUE(response.get_result()["string"] == "value");
    ASSERT_EQ(response.get_result().count("bool"), 1);
    ASSERT_TRUE(response.get_result()["bool"].is_boolean());
    ASSERT_EQ(response.get_result()["bool"], true);
    ASSERT_EQ(response.get_result().count("number"), 1);
    ASSERT_TRUE(response.get_result()["number"].is_number());
    ASSERT_EQ(response.get_result()["number"], 22);

    str_response = server.call(JsonRpcRequest::make_method_request("m_arr", nullptr, {"v1", 10}));
    ASSERT_EQ(calls_passed, 3);
    ASSERT_NO_THROW(response = parse_response(str_response));
    ASSERT_EQ(response.get_id(), nullptr);
    ASSERT_EQ(response.get_result().size(), 3);
    ASSERT_TRUE(response.get_result()[0].is_string());
    ASSERT_TRUE(response.get_result()[1].is_boolean());
    ASSERT_TRUE(response.get_result()[2].is_number());
    ASSERT_TRUE(response.get_result()[0] == "value2");
    ASSERT_EQ(response.get_result()[1], false);
    ASSERT_EQ(response.get_result()[2], 33);

    str_response = server.call(JsonRpcRequest::make_notification_request("n_null"));
    ASSERT_EQ(calls_passed, 4);
    ASSERT_TRUE(str_response.empty());

    str_response = server.call(JsonRpcRequest::make_notification_request("n_obj", { {"p1", "v1"}, {"p2", 10} }));
    ASSERT_EQ(calls_passed, 5);
    ASSERT_TRUE(str_response.empty());

    str_response = server.call(JsonRpcRequest::make_notification_request("n_arr", {"v1", 10}));
    ASSERT_EQ(calls_passed, 6);
    ASSERT_TRUE(str_response.empty());
}

/*
 * Checks if server returns invalid method errors
 */
TEST(JsonRpcRequestHandlerTest, CheckInvalidMethodErrorGeneration) {
    int calls_passed = 0;
    MockServerConnector server{};
    server.add_method("test_method", [&calls_passed](const json::Json&) -> json::Json {
        calls_passed++;
        return {};
    });
    server.add_notification("test_notify", [&calls_passed](const json::Json&) {
        calls_passed++;
    });

    ASSERT_EQ(calls_passed, 0);

    // valid method call
    server.call(JsonRpcRequest::make_method_request("test_method", 1));
    ASSERT_EQ(calls_passed, 1);

    // valid notification call
    server.call(JsonRpcRequest::make_notification_request("test_notify"));
    ASSERT_EQ(calls_passed, 2);

    // invalid method call - error
    call_assert_exception_and_test(server, JsonRpcRequest::make_method_request("invalid_method", 2).to_json().dump(),
         common::ERROR_RPC_METHOD_NOT_FOUND, json::Json(2));
    ASSERT_EQ(calls_passed, 2);

    // invalid notification call - no response
    std::string str_response_2{};
    str_response_2 = server.call(JsonRpcRequest::make_method_request("invalid_method", 2));
    ASSERT_EQ(calls_passed, 2);
}

/*
 * Checks if server returns parse errors on invalid jsons
 */
TEST(JsonRpcRequestHandlerTest, CheckParseErrorGeneration) {
    int calls_passed = 0;
    MockServerConnector server{};
    server.add_method("test_method", [&calls_passed](const json::Json&) -> json::Json {
        calls_passed++;
        return {};
    });
    server.add_notification("test_notify", [&calls_passed](const json::Json&) {
        calls_passed++;
    });

    ASSERT_EQ(calls_passed, 0);

    // valid method call
    server.call(JsonRpcRequest::make_method_request("test_method", 1));
    ASSERT_EQ(calls_passed, 1);

    // valid notification call
    server.call(JsonRpcRequest::make_notification_request("test_notify"));
    ASSERT_EQ(calls_passed, 2);

    // invalid method call - error
    call_assert_exception_and_test(server, "{\"jsonrpc\":\"2.0\", }", common::ERROR_RPC_JSON_PARSE_ERROR, json::Json{});
    ASSERT_EQ(calls_passed, 2);
}

/*
 * Checks if server returns invalid request errors
 */
TEST(JsonRpcRequestHandlerTest, CheckInvalidRequestErrorGeneration) {
    int calls_passed = 0;
    MockServerConnector server{};
    server.add_method("test_method", [&calls_passed](const json::Json&) -> json::Json {
        calls_passed++;
        return {};
    });
    server.add_notification("test_notify", [&calls_passed](const json::Json&) {
        calls_passed++;
    });

    ASSERT_EQ(calls_passed, 0);

    // valid method call
    server.call(JsonRpcRequest::make_method_request("test_method", 1));
    ASSERT_EQ(calls_passed, 1);

    // valid notification call
    server.call(JsonRpcRequest::make_notification_request("test_notify"));
    ASSERT_EQ(calls_passed, 2);

    // invalid request - invalid id type
    call_assert_exception_and_test(server, "{\"jsonrpc\":\"2.0\", \"id\":{}, \"method\":\"test_method\" }",
         common::ERROR_RPC_INVALID_REQUEST, json::Json{});
    ASSERT_EQ(calls_passed, 2);

    // invalid request - invalid params type
    call_assert_exception_and_test(server, "{\"jsonrpc\":\"2.0\", \"id\":1, \"params\":123, \"method\":\"test_method\" }",
         common::ERROR_RPC_INVALID_REQUEST, json::Json{});
    ASSERT_EQ(calls_passed, 2);

    // invalid request - invalid params in notification - since it cannot be parsed as a notification request,
    // error is returned anyway
    call_assert_exception_and_test(server, "{\"jsonrpc\":\"2.0\", \"params\":123, \"method\":\"test_notification\"}",
         common::ERROR_RPC_INVALID_REQUEST, json::Json{});
    ASSERT_EQ(calls_passed, 2);
}

/*
 * Checks if ids are correct in errors
 */
TEST(JsonRpcRequestHandlerTest, ValidateIdsOnErrors) {
    int calls_passed = 0;
    MockServerConnector server{};
    server.add_method("test_method", [&calls_passed](const json::Json&) -> json::Json {
        calls_passed++;
        throw JsonRpcException(-100, "Test");
    });
    server.add_notification("test_notify", [&calls_passed](const json::Json&) {
        calls_passed++;
        throw JsonRpcException(-200, "Test");
    });

    ASSERT_EQ(calls_passed, 0);

    call_assert_exception_and_test(server, "{\"jsonrpc\":\"2.0\", \"id\":22, \"method\":\"test_method\" }", -100,
        json::Json(22));
    ASSERT_EQ(calls_passed, 1);

    // for notification no exception is expected
    std::string response{};
    ASSERT_NO_THROW(response = server.call("{\"jsonrpc\":\"2.0\", \"method\":\"test_notify\"}"));
    ASSERT_EQ(calls_passed, 2);
    ASSERT_TRUE(response.empty());
}

/*
 * Test batch requests with two method calls and one notification called
 */
TEST(JsonRpcRequestHandlerTest, BatchRequestTests) {
    MockServerConnector server{};
    server.add_method("method1", [](const json::Json&) -> json::Json {
        return 11;
    });
    server.add_method("method2", [](const json::Json&) -> json::Json {
        return 22;
    });
    bool notification_called = false;
    server.add_notification("test_notify", [&notification_called](const json::Json&) {
        notification_called = true;
    });

    std::string str_response = server.call(R"(
        [
            {"jsonrpc": "2.0", "id": 1, "method": "method1"},
            {"jsonrpc": "2.0", "id": 2, "method": "method2"},
            {"jsonrpc": "2.0", "method": "test_notify"}
        ]
    )");
    try {
        bool method_1_response_found = false;
        bool method_2_response_found = false;
        auto json_response = json::Json::parse(str_response);
        ASSERT_TRUE(json_response.is_array());
        ASSERT_TRUE(json_response.size() == 2);
        for (auto it = json_response.begin(); it != json_response.end(); ++it) {
            auto response = JsonRpcResponse::from_json(*it);
            if (response.get_id().is_number() && response.get_id() == 1) {
                method_1_response_found = (response.get_result().is_number() && response.get_result() == 11);
            }
            if (response.get_id().is_number() && response.get_id() == 2) {
                method_2_response_found = (response.get_result().is_number() && response.get_result() == 22);
            }
        }
        ASSERT_TRUE(method_1_response_found);
        ASSERT_TRUE(method_2_response_found);
    }
    catch (...) {
        FAIL() << "Unexpected exceptions";
    }
    ASSERT_TRUE(notification_called);
}

/*
 * Test error generation on batch requests that are invalid json
 */
TEST(JsonRpcRequestHandlerTest, InvalidJsonBatchRequests) {
    MockServerConnector server{};

    std::string request = R"(
        [
            {"jsonrpc": "2.0", "id": 1, "method": "method1"},
            {"jsonrpc": "2.0", "method": "test_notify"
        ]
    )";

    // we expect a single error with no id
    call_assert_exception_and_test(server, request, common::ERROR_RPC_JSON_PARSE_ERROR, json::Json{});
}

/*
 * Check invalid request errors on empty array batch calls
 */
TEST(JsonRpcRequestHandlerTest, InvalidRequestErrorsOnEmptyBatchCalls) {
    MockServerConnector server{};

    std::string request = "[]";

    // we expect a single error with no id
    call_assert_exception_and_test(server, request, common::ERROR_RPC_INVALID_REQUEST, json::Json{});
}

/*
 * Check responses on notification-only batch calls
 */
TEST(JsonRpcRequestHandlerTest, CheckNoResponsesOnNotificationOnlyBatchRequests) {
    MockServerConnector server{};
    int num_called = 0;
    server.add_notification("test_notify", [&num_called](const json::Json&) {
        num_called++;
    });

    std::string request = R"(
        [
            {"jsonrpc": "2.0", "method": "test_notify"},
            {"jsonrpc": "2.0", "method": "test_notify"},
            {"jsonrpc": "2.0", "method": "test_notify"}
        ]
    )";

    std::string response = server.call(request);
    ASSERT_TRUE(response.empty());
    ASSERT_TRUE(3 == num_called);
}

/*
 * Check batch calls with valid and invalid responses
 */
TEST(JsonRpcRequestHandlerTest, CheckComplexRequests) {
    MockServerConnector server{};
    int num_called = 0;
    server.add_method("add", [](const json::Json& in) -> json::Json {
        if (!in.is_array() || in.size() != 2) {
            throw JsonRpcException(common::ERROR_RPC_INVALID_REQUEST);
        }
        return in[0].get<uint32_t>() + in[1].get<uint32_t>();
    });
    server.add_method("mul", [](const json::Json& in) -> json::Json {
        if (!in.is_array() || in.size() != 2) {
            throw JsonRpcException(common::ERROR_RPC_INVALID_REQUEST);
        }
        return in[0].get<uint32_t>() * in[1].get<uint32_t>();
    });
    server.add_notification("notify", [&num_called](const json::Json&) {
        num_called++;
    });

    std::string request = R"(
        [
            {"jsonrpc": "2.0", "id": 0, "method": "add", "params": [3, 10]},
            {"jsonrpc": "2.0", "id": 1, "method": "mul", "params": [7, 9]},
            {"jsonrpc": "2.0", "id": 2, "method": "add", "params": [1, 2]},
            {"jsonrpc": "2.0", "id": 3, "method": "invalid_method"},
            {"jsonrpc": "2.0", "id": 4, "method": "add"},
            {"jsonrpc": "2.0", "method": "notify"},
            {"jsonrpc": "2.0", "method": "notify2"}
        ]
    )";

    std::vector<bool> response_present = {false, false, false, false, false};
    ASSERT_TRUE(0 == num_called);
    std::string str_response = server.call(request);
    json::Json json_response{};
    ASSERT_TRUE(1 == num_called);
    ASSERT_NO_THROW(json_response = json::Json::parse(str_response));
    ASSERT_TRUE(json_response.is_array());
    ASSERT_TRUE(json_response.size() == 5);

    // check responses
    for (const auto& single_response : json_response) {
        try {
            auto response = JsonRpcResponse::from_json(single_response);
            if (!response.get_id().is_number()) {
                FAIL() << "Id is not a number in response: " << single_response.dump();
            }
            if (0u == response.get_id()) {
                ASSERT_TRUE(response.get_result().is_number());
                ASSERT_TRUE(response.get_result() == 13);
                response_present[0] = true;
            }
            else if (1u == response.get_id()) {
                ASSERT_TRUE(response.get_result().is_number());
                ASSERT_TRUE(response.get_result() == 63);
                response_present[1] = true;
            }
            else if (2u == response.get_id()) {
                ASSERT_TRUE(response.get_result().is_number());
                ASSERT_TRUE(response.get_result() == 3);
                response_present[2] = true;
            }
            else {
                FAIL() << "Unexpected response: " << single_response.dump();
            }
        }
        catch (...) {
            try {
                auto error = JsonRpcError::from_json(single_response);
                if (!error.get_id().is_number()) {
                    FAIL() << "Id is not a number in error: " << single_response.dump();
                }
                else if (3u == error.get_id()) {
                    ASSERT_TRUE(error.get_code() == common::ERROR_RPC_METHOD_NOT_FOUND);
                    response_present[3] = true;
                }
                else if (4u == error.get_id()) {
                    ASSERT_TRUE(error.get_code() == common::ERROR_RPC_INVALID_REQUEST);
                    response_present[4] = true;
                }
                else {
                    FAIL() << "Unexpected error: " << single_response.dump();
                }
            }
            catch (...) {
                FAIL() << "Invalid response: " << single_response.dump();
            }
        }
    }
    for (auto i = 0u; i < 5; ++i) {
        ASSERT_TRUE(response_present[i]);
    }

}

}
