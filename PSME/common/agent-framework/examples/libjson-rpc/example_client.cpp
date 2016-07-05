/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * @file example_client.cpp
 *
 * @brief Json-RPC client example implementation
 * */

#include <iostream>

#include "stubclient.h"
#include <jsonrpccpp/client/connectors/httpclient.h>

using namespace jsonrpc;
using namespace std;

static const size_t TEST_COUNT = 100;

StubClient::~StubClient() {}

int main(int argc, char* argv[]) {

    if (2 != argc) {
        cout << "Usage: " << argv[0] << " URL" << endl;
        cout << "URL -  url address like: http://localhost:4999" << endl;
        return -1;
    }

    HttpClient http_client(argv[1]);
    StubClient c(http_client);

    try {
        for (size_t i = 0; i < TEST_COUNT; i++) {
            cout << c.sayHello("Peter Knafl") << endl;
            c.notifyServer();
        }
    } catch (JsonRpcException e) {
        cerr << "Json-RPC Error: " << e.what() << endl;
    }

    return 0;
}
