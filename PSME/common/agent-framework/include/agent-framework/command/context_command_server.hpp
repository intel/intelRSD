/*!
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
 * @file context_command_server.hpp
 *
 * @brief Command Server implementation for RPC C++ library
 * */

#pragma once

#include "json-rpc/connectors/abstract_server_connector.hpp"
#include "json-rpc/handlers/json_rpc_request_handler.hpp"
#include "agent-framework/command/context_registry.hpp"

#include <map>
#include <functional>

namespace agent_framework {
namespace command {

/*
 * JSON RPC command server that is able to pass used-defined contexts to the
 * procedures handling JSON RPC commands
 */
template <typename CONTEXT>
class ContextCommandServer final {
public:

    using MethodFunction = typename json_rpc::JsonRpcRequestHandler::MethodHandler;
    using NotificationFunction = typename json_rpc::JsonRpcRequestHandler::NotificationHandler;

    /*!
     * @brief Constructs a contextual command Server
     * @param connector Server connectors
     * @param context Context that will be passed to all commands
     */
    ContextCommandServer(json_rpc::AbstractServerConnectorPtr connector, std::shared_ptr<CONTEXT> context)
            : m_connector(connector), m_handler(new json_rpc::JsonRpcRequestHandler()), m_context(context) {

        m_connector->set_url_handler("/", m_handler);
        m_handler->set_info_logger([] (const std::string& msg) {
            log_debug("command-server", msg);
        });
        m_handler->set_error_logger([] (const std::string& msg) {
            log_error("command-server", msg);
        });
    }

    ContextCommandServer(const ContextCommandServer&) = delete;
    ContextCommandServer(ContextCommandServer&&) = delete;
    ContextCommandServer& operator=(const ContextCommandServer&) = delete;
    ContextCommandServer& operator=(ContextCommandServer&&) = delete;

    /*! Start command JSON server */
    bool start() {
        return m_connector->start_listening();
    }

    /*! Stop command JSON server (on default) */
    void stop() {
        m_connector->stop_listening();
    }

    /*! Adds all commands registered in the registry */
    void add(const typename command::ContextRegistry<CONTEXT>::Commands& commands) {

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
                std::bind(ContextCommandServer<CONTEXT>::method_wrapper, m_context, command, _1));
            m_handler->set_notification_handler(command_name,
                std::bind(ContextCommandServer<CONTEXT>::notification_wrapper, m_context, command, _1));
        }
    }

private:

    static json::Json method_wrapper(std::shared_ptr<CONTEXT> ctx,
            std::shared_ptr<ContextCommandBase<CONTEXT>> command, const json::Json& input) {

        command->get_procedure().validate(input);
        json::Json output{};
        command->method(ctx, input, output);
        return output;
    }

    static void notification_wrapper(std::shared_ptr<CONTEXT> ctx,
            std::shared_ptr<ContextCommandBase<CONTEXT>> command, const json::Json& input) {

        command->get_procedure().validate(input);
        command->notification(ctx, input);
    }

    json_rpc::AbstractServerConnectorPtr m_connector{};
    std::shared_ptr<json_rpc::JsonRpcRequestHandler> m_handler{};
    std::shared_ptr<CONTEXT> m_context{};
};

}
}
