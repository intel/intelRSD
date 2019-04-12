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
 * @file command_server.cpp
 *
 * @brief Command Server implementation for RPC C++ library
 * */

#include "agent-framework/command/command_server.hpp"

using namespace agent_framework::command;
using namespace json_rpc;

CommandServer::CommandServer(json_rpc::AbstractServerConnectorPtr connector) :
        m_connector(connector),
        m_handler(new JsonRpcRequestHandler()) {

    m_connector->set_url_handler("/", m_handler);
    m_handler->set_info_logger([] (const std::string& msg) {
        log_debug("command-server", msg);
    });
    m_handler->set_error_logger([] (const std::string& msg) {
        log_error("command-server", msg);
    });
}

CommandServer::~CommandServer() {

}

void CommandServer::add(const command::Registry::Commands& commands) {

    using std::placeholders::_1;

    std::vector<std::string> registered_commands{};

    for (const auto& command : commands) {
        const auto& command_name = command->get_procedure().get_procedure_name();

        if (std::find(registered_commands.begin(), registered_commands.end(), command_name) != registered_commands.end()) {
            log_error("command-server", "Trying to register " << command_name
                << " while this command was already registered! Ignoring it...");
            continue;
        }
        registered_commands.push_back(command_name);

        m_handler->set_method_handler(command_name,
            std::bind(CommandServer::method_wrapper, command, _1));
        m_handler->set_notification_handler(command_name,
            std::bind(CommandServer::notification_wrapper, command, _1));
    }
}
