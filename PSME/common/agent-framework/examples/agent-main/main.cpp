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
*/

#include "agent-framework/client/generic_client_thread.hpp"
#include "agent-framework/logger_ext.hpp"
#include "agent-framework/logger_loader.hpp"
#include "agent-framework/module/module.hpp"
#include "agent-framework/module/module_manager.hpp"
#include "agent-framework/registration/registration_data.hpp"
#include "agent-framework/registration/registration_manager.hpp"
#include "agent-framework/signal.hpp"
#include "agent-framework/state_machine/state_machine.hpp"
#include "agent-framework/state_machine/state_machine_thread.hpp"

#include "agent-framework/command/command.hpp"
#include "agent-framework/command/command_factory.hpp"
#include "agent-framework/command/command_json.hpp"
#include "agent-framework/command/command_json_server.hpp"

#include <jsonrpccpp/server/connectors/httpserver.h>

#include "logger/logger_factory.hpp"

#include <csignal>
#include <cstdio>
#include <memory>

using namespace std;
using namespace agent_framework;
using namespace agent_framework::generic;
using namespace logger_cpp;

using command::Command;
using command::CommandFactory;
using command::CommandJson;
using command::CommandJsonServer;

/* @TODO: Move this varibale to configuration class. */
const char GENERIC_AGENT[] = "Compute";

/* @TODO: Move this varibale to configuration class. */
const char GENERIC_JSON_COMMANDS_IMPLEMENTATION[] = "Dummy";

/*!
 * @brief Generic Agent main method.
 * */
int main(int argc, char* argv[]) {

    /* @TODO: move port to configuration class */
    if (2 != argc) {
        cout << "Usage: " << argv[0] << " PORT" << endl;
        cout << "PORT -  server port number to listen" << endl;
        return -1;
    }

    RegistrationManager reg_manager;
    RegistrationData reg_data;

    StateMachineThreadUniquePtr state_machine_thread_u_ptr = nullptr;
    GenericClientThreadUniquePtr client = nullptr;

    // Initialize logger
    log_info(LOGUSR, "Running Generic Agent...\n");

    // Register
    reg_manager.register_agent(reg_data);

    // Initialize command server
    /* @TODO: Move this varibale to configuration class. */
    int port = std::stoi(argv[1]);

    /* @TODO: Move this setting to configuration class. */
    auto& commands_factory = CommandFactory::get_instance();

    commands_factory.add_commands(GENERIC_AGENT,
            GENERIC_JSON_COMMANDS_IMPLEMENTATION);

    jsonrpc::HttpServer http_server(port);
    CommandJsonServer server(http_server);

    server.add(commands_factory.create());
    server.start();

    try {
        // Create modules
        ModuleManager::create();
        // Start state machine
        //state_machine_thread_u_ptr.reset(new StateMachineThread(ModuleManager::get_modules()));
        // Start RPC Client
        client.reset(new GenericClientThread(reg_data.get_app_rpc_server_ip(),
                    reg_data.get_app_rpc_server_port()));

        state_machine_thread_u_ptr->start();
    }
    catch (exception & e) {
        log_error(LOGUSR, e.what());
    }

    // Stop the program and wait for interrupt.
    wait_for_interrupt();

    server.stop();

    log_info(LOGUSR, "Stopping Generic Agent...\n");

    ModuleManager::cleanup();
    command::Command::Map::cleanup();
    command::CommandJson::Map::cleanup();

    LoggerFactory::cleanup();
    return 0;
}
