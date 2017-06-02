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
*/

#include "agent-framework/client/generic_client_thread.hpp"

using namespace std;
using namespace agent_framework::generic;

GenericClientThread::~GenericClientThread() {
    m_is_running = false;
    m_thread.join();
}

void GenericClientThread::m_task() {

    std::string url{};

    log_info(GET_LOGGER("rpc-client"), "Starting RPC Client thread...");

    url = "http://" + m_ip + ":" + std::to_string(m_port);

    jsonrpc::HttpClient httpclient(url);
    GenericClient client(httpclient);

    log_debug(GET_LOGGER("rpc-client"), "RPC Client has been initialized.");

    while(m_is_running) {
        log_debug(GET_LOGGER("rpc-client"), "Waiting for events...");

        /* Implementation of this loop has to be condition_variable based.
         * The thread interates as long as message queue is not empty.
         * Sleep blow is used only for stubbing the loop.
         * */

        this_thread::sleep_for(chrono::seconds(1));
    }

    log_debug(GET_LOGGER("rpc-client"), "RPC Client thread is stopped.");
}


