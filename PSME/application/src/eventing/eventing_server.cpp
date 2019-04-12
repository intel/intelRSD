/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 *
 * */

#include "psme/eventing/eventing_server.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace psme::app::eventing;
using namespace json_rpc;
using namespace agent_framework::command;

EventingServer::EventingServer(const json::Json& config) :
        m_connector(
            new HttpServerConnector(
                config.value("eventing", json::Json::object()).value("port", std::uint16_t{})
            )
        ),
        m_server(new CommandServer(m_connector)) {
    // this registers commands from the registry in the 'local' translation unit
    m_server->add(agent_framework::command::Registry::get_instance()->get_commands());
}

EventingServer::~EventingServer() {
    stop();
}
void EventingServer::start() {
    m_server->start();
}

void EventingServer::stop() {
    m_server->stop();
}
