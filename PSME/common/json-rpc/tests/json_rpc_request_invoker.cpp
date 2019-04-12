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
 * @file tests/json_rpc_request_invoker.cpp
 */

#include "json-rpc/handlers/json_rpc_request_invoker.hpp"

#include "json-rpc/json_rpc_request.hpp"
#include "json-rpc/json_rpc_error.hpp"
#include "json-rpc/json_rpc_response.hpp"

#include "gtest/gtest.h"

using namespace json_rpc;

namespace {

/*!
 * This is a mock client connector that simulates a real JsonRpcServer.
 * Using it, it is possible to check if JsonRpcRequestInvoker properly builds requests, extracts responses
 * and reacts to errors (e.g. by building proper exceptions)
 */
class MockClientConnector : public AbstractClientConnector {
public:

    // takes a request and string result reference
    using GenericCallbackFn = std::function<void(const JsonRpcRequest&, std::string&)>;
    // takes method/params/result reference
    using MethodCallbackFn = std::function<void(const std::string&, const json::Json&, json::Json&)>;
    // takes method/params
    using NotificationCallbackFn = std::function<void(const std::string&, const json::Json&)>;

    bool check_if_called_and_reset() {
        bool tmp = m_send_request_called;
        m_send_request_called = false;
        return tmp;
    }

    virtual std::string send_request(const std::string& message) override {
        m_send_request_called = true;
        // parse string to json
        json::Json raw_request{};
        try {
            raw_request = json::Json::parse(message);
        }
        catch (...) {
            return JsonRpcError(JsonRpcException(common::ERROR_RPC_JSON_PARSE_ERROR), {}).to_json().dump();
        }

        // parse json to request
        JsonRpcRequest request = JsonRpcRequest::make_notification_request("");
        try {
            request = JsonRpcRequest::from_json(raw_request);
        }
        catch (...) {
            return JsonRpcError(JsonRpcException(common::ERROR_RPC_INVALID_REQUEST), {}).to_json().dump();
        }

        // dispatch request:
        // if generic handler was set - use it to return something
        // otherwise - use method/notification handler depending on the request
        // if no handlers were set - return an error
        if (m_generic_handler) {
            std::string result{};
            m_generic_handler(request, result);
            return result;
        }
        if (!request.is_notification() && m_method_handler) {
            auto id = request.get_id();
            try {
                json::Json result{};
                m_method_handler(request.get_method(), request.get_params(), result);
                return JsonRpcResponse(result, id).to_json().dump();
            }
            catch (const JsonRpcException& e) {
                return JsonRpcError(e, id).to_json().dump();
            }
        }
        if (request.is_notification() && m_notification_handler) {
            m_notification_handler(request.get_method(), request.get_params());
            return {};
        }
        return JsonRpcError(JsonRpcException(common::ERROR_RPC_INTERNAL_ERROR), {}).to_json().dump();
    }

    void set_generic_handler(GenericCallbackFn handler) {
        m_generic_handler = handler;
    }

    void set_notification_handler(NotificationCallbackFn handler) {
        m_notification_handler = handler;
    }

    void set_method_handler(MethodCallbackFn handler) {
        m_method_handler = handler;
    }

private:
    GenericCallbackFn m_generic_handler{};
    MethodCallbackFn m_method_handler{};
    NotificationCallbackFn m_notification_handler{};

    bool m_send_request_called{false};
};

// uses JsonRpcRequestInvoker (under test) to call specific method
json::Json call_method(std::shared_ptr<AbstractClientConnector> connector, const std::string& command,
        const json::Json& params) {

    JsonRpcRequestInvoker invoker;
    invoker.prepare_method(command, params);
    invoker.call(connector);
    return invoker.get_result();
}

// uses JsonRpcRequestInvoker (under test) to call specific notification
json::Json call_notification(std::shared_ptr<AbstractClientConnector> connector, const std::string& command,
        const json::Json& params) {

    JsonRpcRequestInvoker invoker;
    invoker.prepare_notification(command, params);
    invoker.call(connector);
    return invoker.get_result();
}


}

namespace testing {

/*
 * The following tests check if valid calls (notifications/methods) are correctly handled:
 * json-rpc requests are correctly built, no exceptions are thrown, responses are correctly
 * extracted from the json-rpc results.
 * In total 6 tests: notification/method calls with null/object/array params.
 */
TEST(JsonRpcRequestInvokerTest, ValidNotificationCallWithNoParams) {
    std::shared_ptr<MockClientConnector> mock{new MockClientConnector};
    json::Json result{};
    mock->set_notification_handler([] (const std::string& method, const json::Json& params) {
        ASSERT_EQ(method, "test");
        ASSERT_TRUE(params.is_null());
    });

    ASSERT_NO_THROW(result = call_notification(mock, "test", {}));
    ASSERT_TRUE(result.is_null());
    ASSERT_TRUE(mock->check_if_called_and_reset());
}

TEST(JsonRpcRequestInvokerTest, ValidNotificationCallWithObjectParams) {
    std::shared_ptr<MockClientConnector> mock{new MockClientConnector};
    json::Json result{};
    mock->set_notification_handler([] (const std::string& method, const json::Json& params) {
        ASSERT_EQ(method, "test");
        ASSERT_EQ(params.count("param"), 1);
        ASSERT_EQ(params["param"].get<std::string>(), "value");
        ASSERT_EQ(params.size(), 1);
    });

    ASSERT_NO_THROW(result = call_notification(mock, "test", { {"param", "value"} }));
    ASSERT_TRUE(result.is_null());
    ASSERT_TRUE(mock->check_if_called_and_reset());
}

TEST(JsonRpcRequestInvokerTest, ValidNotificationCallWithArrayParams) {
    std::shared_ptr<MockClientConnector> mock{new MockClientConnector};
    json::Json result{};
    mock->set_notification_handler([] (const std::string& method, const json::Json& params) {
        ASSERT_EQ(method, "test");
        ASSERT_TRUE(params.is_array());
        ASSERT_EQ(params.size(), 3);
        for (int i = 0; i < 3; ++i) {
            ASSERT_TRUE(i == params[i]);
        }
    });

    ASSERT_NO_THROW(result = call_notification(mock, "test", { 0, 1, 2 }));
    ASSERT_TRUE(result.is_null());
    ASSERT_TRUE(mock->check_if_called_and_reset());
}

TEST(JsonRpcRequestInvokerTest, ValidMethodCallWithNoParams) {
    std::shared_ptr<MockClientConnector> mock{new MockClientConnector};
    json::Json result{};
    mock->set_method_handler([] (const std::string& method, const json::Json& params, json::Json& res) {
        ASSERT_EQ(method, "test");
        ASSERT_TRUE(params.is_null());
        res = json::Json{};
    });

    ASSERT_NO_THROW(result = call_method(mock, "test", {}));
    ASSERT_TRUE(result.is_null());
    ASSERT_TRUE(mock->check_if_called_and_reset());
}

TEST(JsonRpcRequestInvokerTest, ValidMethodCallWithObjectParams) {
    std::shared_ptr<MockClientConnector> mock{new MockClientConnector};
    json::Json result{};
    mock->set_method_handler([] (const std::string& method, const json::Json& params, json::Json& res) {
        ASSERT_EQ(method, "test");
        ASSERT_EQ(params.count("param"), 1);
        ASSERT_EQ(params["param"].get<std::string>(), "value");
        ASSERT_EQ(params.size(), 1);
        res = json::Json{};
    });

    ASSERT_NO_THROW(result = call_method(mock, "test", { {"param", "value"} }));
    ASSERT_TRUE(result.is_null());
    ASSERT_TRUE(mock->check_if_called_and_reset());
}

TEST(JsonRpcRequestInvokerTest, ValidMethodCallWithArrayParams) {
    std::shared_ptr<MockClientConnector> mock{new MockClientConnector};
    json::Json result{};
    mock->set_method_handler([] (const std::string& method, const json::Json& params, json::Json& res) {
        ASSERT_EQ(method, "test");
        ASSERT_TRUE(params.is_array());
        ASSERT_EQ(params.size(), 3);
        for (int i = 0; i < 3; ++i) {
            ASSERT_TRUE(i == params[i]);
        }
        res = json::Json{};
    });

    ASSERT_NO_THROW(result = call_method(mock, "test", { 0, 1, 2 }));
    ASSERT_TRUE(result.is_null());
    ASSERT_TRUE(mock->check_if_called_and_reset());
}

/*
 * Check if various responses types are correctly extracted from responses
 * 6 subtests - 1 per each json type (null, boolean, string, number, array, object)
 */
TEST(JsonRpcRequestInvokerTest, ValidResultUnpacking) {
    std::shared_ptr<MockClientConnector> mock{new MockClientConnector};
    json::Json result{};

    // null
    mock->set_method_handler([] (const std::string&, const json::Json&, json::Json& res) {
        res = json::Json{};
    });
    ASSERT_NO_THROW(result = call_method(mock, "test", {}));
    ASSERT_TRUE(result.is_null());
    ASSERT_TRUE(mock->check_if_called_and_reset());

    // strings
    mock->set_method_handler([] (const std::string&, const json::Json&, json::Json& res) {
        res = "123";
    });
    result = call_method(mock, "test", {});
    ASSERT_TRUE(result.is_string());
    ASSERT_TRUE(result == "123");
    ASSERT_TRUE(mock->check_if_called_and_reset());

    // bool value
    mock->set_method_handler([] (const std::string&, const json::Json&, json::Json& res) {
        res = true;
    });
    ASSERT_NO_THROW(result = call_method(mock, "test", {}));
    ASSERT_TRUE(result.is_boolean());
    ASSERT_TRUE(result == true);
    ASSERT_TRUE(mock->check_if_called_and_reset());

    // numeric value
    mock->set_method_handler([] (const std::string&, const json::Json&, json::Json& res) {
        res = 24;
    });
    ASSERT_NO_THROW(result = call_method(mock, "test", {}));
    ASSERT_TRUE(result.is_number());
    ASSERT_TRUE(result == 24);
    ASSERT_TRUE(mock->check_if_called_and_reset());

    // array
    mock->set_method_handler([] (const std::string&, const json::Json&, json::Json& res) {
        res = {0, 1, 2, 3};
    });
    ASSERT_NO_THROW(result = call_method(mock, "test", {}));
    ASSERT_TRUE(result.is_array());
    ASSERT_EQ(result.size(), 4);
    for (unsigned i = 0; i < 4; ++i) {
        ASSERT_EQ(i, result[i]);
    }
    ASSERT_TRUE(mock->check_if_called_and_reset());

    // object
    mock->set_method_handler([] (const std::string&, const json::Json&, json::Json& res) {
        res = { {"field", "value"} };
    });
    ASSERT_NO_THROW(result = call_method(mock, "test", {}));
    ASSERT_TRUE(result.is_object());
    ASSERT_EQ(result.size(), 1);
    ASSERT_EQ(result.count("field"), 1);
    ASSERT_TRUE(result["field"] == "value");
    ASSERT_TRUE(mock->check_if_called_and_reset());
}

/*
 * Check if correct exception is thrown when JsonRpcServer responds with a non-json response
 */
TEST(JsonRpcRequestInvokerTest, ExceptionOnResponseParseError) {
    std::shared_ptr<MockClientConnector> mock{new MockClientConnector};
    mock->set_generic_handler([] (const JsonRpcRequest&, std::string& res) {
        res = "{\"id\":123, ";
    });

    try {
        call_method(mock, "test", {});
        FAIL() << "Expected exception";
    }
    catch (const JsonRpcException& e) {
        ASSERT_TRUE(mock->check_if_called_and_reset());
        ASSERT_EQ(e.get_code(), common::ERROR_RPC_JSON_PARSE_ERROR);
    }
    catch (...) {
        FAIL() << "Invalid exception type";
    }
}

/*
 * Check if correct exception is thrown when JsonRpcServer responds with an invalid response
 * (a json that is not a valid json-rpc response or json-rpc error object)
 */
TEST(JsonRpcRequestInvokerTest, ExceptionOnInvalidResponseFormat) {
    std::shared_ptr<MockClientConnector> mock{new MockClientConnector};
    mock->set_generic_handler([] (const JsonRpcRequest&, std::string& res) {
        res = json::Json{ {"id", 123}, {"res", {}} }.dump();
    });

    try {
        call_method(mock, "test", {});
        FAIL() << "Expected exception";
    }
    catch (const JsonRpcException& e) {
        ASSERT_TRUE(mock->check_if_called_and_reset());
        ASSERT_EQ(e.get_code(), common::ERROR_CLIENT_INVALID_RESPONSE);
    }
    catch (...) {
        FAIL() << "Invalid exception type";
    }
}

/*
 * Check if correct exception is thrown when JsonRpcServer responds with a wrong id
 */
TEST(JsonRpcRequestInvokerTest, ExceptionOnInvalidId) {
    std::shared_ptr<MockClientConnector> mock{new MockClientConnector};
    mock->set_generic_handler([] (const JsonRpcRequest&, std::string& res) {
        res = JsonRpcResponse(json::Json{}, 100).to_json().dump();
    });

    try {
        call_method(mock, "test", {});
        FAIL() << "Expected exception";
    }
    catch (const JsonRpcException& e) {
        ASSERT_TRUE(mock->check_if_called_and_reset());
        ASSERT_EQ(e.get_code(), common::ERROR_CLIENT_INVALID_RESPONSE);
    }
    catch (...) {
        FAIL() << "Invalid exception type";
    }
}

/*
 * Make sure that responses to notifications do break the invoker
 */
TEST(JsonRpcRequestInvokerTest, NoExceptionOnResponseToNotification) {
    std::shared_ptr<MockClientConnector> mock{new MockClientConnector};
    json::Json result{};
    mock->set_generic_handler([] (const JsonRpcRequest&, std::string& res) {
        res = "{}";
    });

    ASSERT_NO_THROW(result = call_notification(mock, "test", {}));
    ASSERT_TRUE(mock->check_if_called_and_reset());
    ASSERT_TRUE(result.is_null());
}

/*
 * Make sure that errors returned on notifications do break the invoker
 */
TEST(JsonRpcRequestInvokerTest, NoExceptionOnErrorsOnNotifications) {
    std::shared_ptr<MockClientConnector> mock{new MockClientConnector};
    json::Json result{};
    mock->set_generic_handler([] (const JsonRpcRequest& req, std::string& res) {
        res = JsonRpcError(137, "message", req.get_id()).to_json().dump();
    });

    ASSERT_NO_THROW(result = call_notification(mock, "test", {}));
    ASSERT_TRUE(mock->check_if_called_and_reset());
    ASSERT_TRUE(result.is_null());
}

/*
 * Check if json rpc errors are correctly translated to exceptions
 */
TEST(JsonRpcRequestInvokerTest, ErrorsAreCorrectlyTranslatedToExceptions) {
    std::shared_ptr<MockClientConnector> mock{new MockClientConnector};
    mock->set_generic_handler([] (const JsonRpcRequest& req, std::string& res) {
        res = JsonRpcError(137, "message", req.get_id(), {{"data", "value"}}).to_json().dump();
    });

    try {
        call_method(mock, "test", {});
        FAIL() << "Expected exception";
    }
    catch (const JsonRpcException& e) {
        ASSERT_EQ(e.get_code(), 137);
        ASSERT_EQ(e.get_message(), "message");
        ASSERT_TRUE(mock->check_if_called_and_reset());
        ASSERT_TRUE(e.get_data().is_object());
        ASSERT_EQ(e.get_data().size(), 1);
        ASSERT_EQ(e.get_data().count("data"), 1);
        ASSERT_TRUE(e.get_data()["data"].is_string());
        ASSERT_TRUE(e.get_data()["data"] == "value");
    }
    catch (...) {
        FAIL() << "Invalid exception type";
    }
}

}
