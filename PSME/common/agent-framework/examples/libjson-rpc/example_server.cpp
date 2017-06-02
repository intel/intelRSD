/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @section DESCRIPTION
 *
 * @file example_server.cpp
 *
 * @brief Json-RPC server example implementation
 * */

#include "abstractstubserver.h"
#include <jsonrpccpp/server/connectors/httpserver.h>

#include <cstdio>
#include <iostream>

using namespace jsonrpc;
using namespace std;

static const size_t DEFAULT_SERVER_PORT = 8888;

class MyStubServer: public AbstractStubServer {
public:
    MyStubServer(AbstractServerConnector& connector);

    virtual void notifyServer();
    virtual std::string sayHello(const std::string& name);
};

MyStubServer::MyStubServer(AbstractServerConnector& connector) :
    AbstractStubServer(connector) {

}

AbstractStubServer::~AbstractStubServer() {}

void MyStubServer::notifyServer() {
    cout << "Server got notified" << endl;
}

string MyStubServer::sayHello(const string& name) {
    return "Hello " + name;
}

int main(int argc, char* argv[]) {

    int port = DEFAULT_SERVER_PORT;

    if (2 != argc) {
        cout << "Usage: " << argv[0] << " PORT" << endl;
        cout << "PORT -  server port number to listen" << endl;
        return -1;
    }

    port = std::stoi(argv[1]);

    HttpServer http_server(port);
    MyStubServer s(http_server);

    s.StartListening();

    cout << "To exit, press any key" << endl;
    cout << "Server is listening..." << endl;
    getchar();

    s.StopListening();

    return 0;
}
