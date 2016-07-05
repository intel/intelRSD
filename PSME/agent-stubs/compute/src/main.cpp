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

#include "agent-framework/logger_loader.hpp"
#include "logger/logger_factory.hpp"
#include "agent-framework/module-ref/compute_manager.hpp"
#include "agent-framework/registration/amc_connection_manager.hpp"
#include "agent-framework/signal.hpp"
#include "agent-framework/version.hpp"

#include "agent-framework/command/command.hpp"
#include "agent-framework/command/command_factory.hpp"
#include "agent-framework/command/command_json.hpp"
#include "agent-framework/command/command_json_server.hpp"

#include "status/status_manager.hpp"
#include "status/state_machine_action.hpp"

#include "configuration/configuration.hpp"
#include "default_configuration.hpp"

#include "agent-framework/eventing/events_queue.hpp"

#include "generate_data.hpp"

#include <jsonrpccpp/server/connectors/httpserver.h>

#include <csignal>
#include <cstdio>
#include <memory>

using namespace std;
using namespace agent_framework;
using namespace agent_framework::generic;
using namespace logger_cpp;
using namespace configuration;

using command::Command;
using command::CommandFactory;
using command::CommandJson;
using command::CommandJsonServer;

using agent::generic::DEFAULT_CONFIGURATION;
using agent::generic::DEFAULT_ENV_FILE;
using agent::generic::DEFAULT_FILE;

static constexpr unsigned int DEFAULT_SERVER_PORT = 7777;
static constexpr const char COMMANDS_IMPLEMENTATION[] = "Stubs";

/*!
 * @brief Generic Agent main method.
 * */
int main(int argc, char* argv[]) {
    unsigned int server_port = DEFAULT_SERVER_PORT;

    /* Initialize configuration */
    log_info(GET_LOGGER("compute-agent"),
        agent_framework::generic::Version::build_info());
    auto& basic_config = Configuration::get_instance();
    basic_config.set_default_configuration(DEFAULT_CONFIGURATION);
    basic_config.set_default_file(DEFAULT_FILE);
    basic_config.set_default_env_file(DEFAULT_ENV_FILE);
    /* @TODO This should be replaced with nice arguments parsing class */
    while (argc > 1) {
        basic_config.add_file(argv[argc - 1]);
        --argc;
    }
    basic_config.load_key_file();
    const json::Value& configuration = basic_config.to_json();

    /* Initialize logger */
    LoggerLoader loader(configuration);
    LoggerFactory::instance().set_loggers(loader.load());
    LoggerFactory::set_main_logger_name("agent");
    log_info(GET_LOGGER("compute-agent"), "Running Generic Agent...\n");

    try {
        server_port = configuration["server"]["port"].as_uint();
    } catch (const json::Value::Exception& e) {
        log_error(GET_LOGGER("compute-agent"),
                "Cannot read server port " << e.what());
    }

    EventDispatcher event_dispatcher;
    event_dispatcher.start();

    AmcConnectionManager amc_connection(event_dispatcher);
    amc_connection.start();

    /* Initialize command server */
    auto& commands_factory = CommandFactory::get_instance();
    try {
        server_port = configuration["server"]["port"].as_uint();
        auto agent_type = configuration["agent"]["capabilities"].as_array();
        commands_factory.add_commands(agent_type.front().as_string(), COMMANDS_IMPLEMENTATION);
    }
    catch (const json::Value::Exception& e) {
        log_error(GET_LOGGER("agent"), "Invalid agent configuration: " << e.what());
        return -10;
    }
    auto commands_initialization =
        commands_factory.create_initialization();
    auto commands = commands_factory.create();

    jsonrpc::HttpServer http_server((int(server_port)));
    CommandJsonServer server(http_server);

    server.add(commands);
    server.start();

    ::agent::compute::generate_data();

    for (const auto& elem : ::agent_framework::module::ComputeManager::get_instance()->
            get_module_manager().get_keys()) {
        ::agent_framework::eventing::EventData edat;
        edat.set_component(elem);
        edat.set_type(::agent_framework::model::enums::Component::Manager);
        edat.set_notification(::agent_framework::eventing::Notification::Add);
        ::agent_framework::eventing::EventsQueue::get_instance()->push_back(edat);
    }


    /* Stop the program and wait for interrupt */
    wait_for_interrupt();

    log_info(GET_LOGGER("compute-agent"), "Stopping Generic Agent...\n");

    /* Cleanup */
    server.stop();
    amc_connection.stop();
    event_dispatcher.stop();
    commands_initialization.clear();
    command::Command::Map::cleanup();
    command::CommandJson::Map::cleanup();
    command::CommandFactory::cleanup();
    Configuration::cleanup();
    LoggerFactory::cleanup();

    return 0;
}
