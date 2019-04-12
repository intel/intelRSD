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
 * @file command_server.hpp
 *
 * @brief Command Server implementation for RPC C++ library
 * */

#pragma once

#include "json-rpc/connectors/abstract_server_connector.hpp"
#include "json-rpc/handlers/json_rpc_request_handler.hpp"
#include "agent-framework/command/registry.hpp"

#include <map>
#include <functional>

namespace agent_framework {
namespace command {

class CommandServer {
public:

    using MethodFunction = typename json_rpc::JsonRpcRequestHandler::MethodHandler;
    using NotificationFunction = typename json_rpc::JsonRpcRequestHandler::NotificationHandler;

    CommandServer(json_rpc::AbstractServerConnectorPtr connector);

    CommandServer(const CommandServer&) = delete;
    CommandServer(CommandServer&&) = delete;
    CommandServer& operator=(const CommandServer&) = delete;
    CommandServer& operator=(CommandServer&&) = delete;

    /*! Start command JSON server */
    bool start() {
        return m_connector->start_listening();
    }

    /*! Stop command JSON server (on default) */
    void stop() {
        m_connector->stop_listening();
    }

    /*! Destructor */
    virtual ~CommandServer();

    /*! Adds all commands registered in the registry */
    void add(const typename command::Registry::Commands& commands);

private:

    static json::Json method_wrapper(std::shared_ptr<CommandBase> command, const json::Json& input) {

        command->get_procedure().validate(input);
        json::Json output{};
        command->method(input, output);
        return output;
    }

    static void notification_wrapper(std::shared_ptr<CommandBase> command, const json::Json& input) {

        command->get_procedure().validate(input);
        command->notification(input);
    }

    json_rpc::AbstractServerConnectorPtr m_connector{};
    std::shared_ptr<json_rpc::JsonRpcRequestHandler> m_handler{};
};

} /* namespace command */
} /* namespace agent_framework */
