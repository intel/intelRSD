/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file command_server_example.cpp
 *
 * */

#include "json-rpc/connectors/http_server_connector.hpp"
#include "agent-framework/command/context_command_server.hpp"
#include "agent-framework/command/context_registry.hpp"

struct TestMethodRequest {
    static std::string get_command() {
        return "testMethod";
    }
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
            get_command(),
            jsonrpc::PARAMS_BY_NAME,
            nullptr};
        return procedure;
    }
    static TestMethodRequest from_json(const json::Json&) {
        return TestMethodRequest();
    }
};

struct TestMethodResponse {
    std::string text{};

    json::Json to_json() const {
        json::Json j = json::Json();
        j["jsonrpc_response"] = text;
        return j;
    }
};

class CustomContext {
public:
    std::string context{};
};

using TestMethod = agent_framework::command::ContextCommand<CustomContext, TestMethodRequest, TestMethodResponse>;

REGISTER_CONTEXT_COMMAND(CustomContext, TestMethod,
    [] (TestMethod::ContextPtr ctx, const TestMethod::Request&, TestMethod::Response& rsp) {
        rsp.text = "(" + ctx->context + ") Test method response!";
    }
);

using namespace json_rpc;
using namespace std;

static const size_t SERVER_PORT = 8888;

using CommandServer = agent_framework::command::ContextCommandServer<CustomContext>;
using Registry = agent_framework::command::ContextRegistry<CustomContext>;

int main() {

    std::shared_ptr<CustomContext> ctx = std::make_shared<CustomContext>();
    ctx->context = "CTX";
    std::shared_ptr<HttpServerConnector> connector(new HttpServerConnector(SERVER_PORT));
    std::shared_ptr<CommandServer> server(new CommandServer(connector, ctx));
    server->add(Registry::get_instance()->get_commands());

    server->start();

    cout << "To exit, press any key" << endl;
    cout << "Server is listening..." << endl;
    getchar();

    server->stop();

    return 0;
}
