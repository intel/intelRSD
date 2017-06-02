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
 * @file command_json_server.cpp
 *
 * @brief Command JSON server implementation
 * */

#include "psme/command/command_json_server.hpp"
#include "psme/rest/server/error/server_exception.hpp"
#include "agent-framework/exceptions/error_codes.hpp"
#include "logger/logger_factory.hpp"

using namespace jsonrpc;
using namespace agent_framework;
using namespace psme::command;

CommandJsonServer::CommandJsonServer(AbstractServerConnector& connector,
        serverVersion_t type) :
            m_connection(connector),
            m_handler(RequestHandlerFactory::createProtocolHandler(type, *this)),
            m_methods({}) {
    connector.SetHandler(m_handler.get());
}

void CommandJsonServer::start() {
    m_connection.StartListening();
    log_debug(GET_LOGGER("command"), "Command JSON Server started\n");
}

void CommandJsonServer::stop() {
    m_connection.StopListening();
    log_debug(GET_LOGGER("command"), "Command Server stopped\n");
}

void CommandJsonServer::HandleMethodCall(Procedure& proc,
        const Json::Value& input, Json::Value& output) {

    try {
        m_methods[proc.GetProcedureName()](input, output);
    }
    catch (const JsonRpcException&) {
        throw;
    }
    catch (const std::exception& e) {
        throw JsonRpcException(static_cast<int>(exceptions::ErrorCode::UNKNOWN_ERROR),
                               e.what());
    }
    catch (...) {
        throw JsonRpcException(static_cast<int>(exceptions::ErrorCode::UNKNOWN_ERROR),
                               "Unknown exception occurred in JSON command method.");
    }
}

void CommandJsonServer::HandleNotificationCall(Procedure& proc,
        const Json::Value& input) {

    try {
        Json::Value dummy;
        m_methods[proc.GetProcedureName()](input, dummy);

        log_debug(GET_LOGGER("command"),
                  "Notification call: " << proc.GetProcedureName() <<
                  "\nRequest: " << input.toStyledString() <<
                  "\nResponse: " << dummy.toStyledString());
    }
    catch (const JsonRpcException&) {
        throw;
    }
    catch (const std::exception& e) {
        throw JsonRpcException(static_cast<int>(exceptions::ErrorCode::UNKNOWN_ERROR),
                               e.what());
    }
    catch (...) {
        throw JsonRpcException(static_cast<int>(exceptions::ErrorCode::UNKNOWN_ERROR),
                               "Unknown exception occurred in JSON command method.");
    }
}

void CommandJsonServer::add(const Procedure& proc,
                            const method_function_t& method) {

    if (RPC_METHOD != proc.GetProcedureType()) {
        return;
    }

    auto& method_name = proc.GetProcedureName();

    if (m_methods.end() != m_methods.find(method_name)) {
        return;
    }

    m_handler->AddProcedure(proc);
    m_methods[method_name] = method;
}

void CommandJsonServer::add(const Procedure& proc,
                            const notification_function_t& notification) {

    if(RPC_NOTIFICATION != proc.GetProcedureType()) {
        return;
    }

    auto& method_name = proc.GetProcedureName();

    if (m_methods.end() != m_methods.find(method_name)) {
        return;
    }

    using std::placeholders::_1;

    m_handler->AddProcedure(proc);
    m_methods[method_name] = std::bind(notification, _1);
}

void CommandJsonServer::add(const CommandJson::Map::command_map_t& command_map) {
    using std::placeholders::_1;
    using std::placeholders::_2;

    CommandJson* command = nullptr;

    for (const auto& command_pair : command_map) {
        command = command_pair.second.get();
        add(command->get_procedure(), static_cast<method_function_t>(
                    std::bind(&CommandJson::method, command, _1, _2)));
        add(command->get_procedure(), static_cast<notification_function_t>(
                    std::bind(&CommandJson::notification, command, _1)));
    }
}

CommandJsonServer::~CommandJsonServer() { }
