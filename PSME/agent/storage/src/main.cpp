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

#include "agent-framework/eventing/event_data.hpp"
#include "agent-framework/eventing/events_queue.hpp"
#include "agent-framework/logger_loader.hpp"
#include "logger/logger_factory.hpp"
#include "agent-framework/module/module.hpp"
#include "agent-framework/module/module_manager.hpp"
#include "agent-framework/registration/amc_connection_manager.hpp"
#include "agent-framework/signal.hpp"
#include "agent-framework/version.hpp"
#include "agent-framework/state_machine/state_machine_thread.hpp"
#include "agent-framework/action/task_runner.hpp"
#include "agent-framework/action/task_status_manager.hpp"

#include "agent-framework/command/command.hpp"
#include "agent-framework/command/command_factory.hpp"
#include "agent-framework/command/command_json.hpp"
#include "agent-framework/command/command_json_server.hpp"

#include "loader/storage_loader.hpp"
#include "configuration/configuration.hpp"
#include "configuration/configuration_validator.hpp"
#include "default_configuration.hpp"

#include "status/status_manager.hpp"
#include "status/state_machine_action.hpp"
#include "status/storage_services_ip_thread.hpp"
#include "discovery/discovery_manager.hpp"
#include "hotswap/hotswap_watcher.hpp"

#include <jsonrpccpp/server/connectors/httpserver.h>

#include <csignal>
#include <cstdio>
#include <iostream>
#include <memory>

using namespace std;
using namespace agent_framework;
using namespace agent_framework::generic;
using namespace agent_framework::state_machine;
using namespace agent::storage::status;
using namespace logger_cpp;

using command::Command;
using command::CommandFactory;
using command::CommandJson;
using command::CommandJsonServer;

using configuration::Configuration;

using agent::generic::DEFAULT_CONFIGURATION;
using agent::generic::DEFAULT_VALIDATOR_JSON;
using agent::generic::DEFAULT_ENV_FILE;
using agent::generic::DEFAULT_FILE;


static constexpr unsigned int DEFAULT_SERVER_PORT = 7778;

void add_state_machine_entries(StateMachineThread* machine_thread);

/*!
 * @brief Generic Agent main method.
 * */
int main(int argc, char* argv[]) {
    unsigned int server_port = DEFAULT_SERVER_PORT;

    StateMachineThreadUniquePtr state_machine_thread_u_ptr = nullptr;
    StorageServicesIpUpdateThreadUniquePtr storage_services_ip_update_u_ptr = nullptr;

    /* Initialize configuration */
    log_info(GET_LOGGER("storage-agent"),
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

    //TODO Will be move to common main file
    json::Value json_schema;
    if (configuration::string_to_json(DEFAULT_VALIDATOR_JSON,
            json_schema)) {
        log_info(GET_LOGGER("storage-agent"), "JSON Schema load!");

        configuration::SchemaErrors errors;
        configuration::SchemaValidator validator;
        configuration::SchemaReader reader;
        reader.load_schema(json_schema, validator);
        validator.validate(configuration, errors);

        if (!errors.is_valid()) {
            std::cerr << "Configuration invalid: " << errors.to_string() << std::endl;
            return -1;
        }
    }

    /* Initialize logger */
    LoggerLoader loader(configuration);
    LoggerFactory::instance().set_loggers(loader.load());
    LoggerFactory::set_main_logger_name("agent");
    log_info(GET_LOGGER("storage-agent"), "Running PSME Storage...\n");

    /* Load module configuration */
    ::agent::storage::loader::StorageLoader module_loader{};
    if (!module_loader.load(configuration)) {
        std::cerr << "Invalid modules configuration" << std::endl;
        return -2;
    }

    try {
        server_port = configuration["server"]["port"].as_uint();
    } catch (const json::Value::Exception& e) {
        log_error(GET_LOGGER("storage-agent"), "Cannot read server port " << e.what());
    }

    /* Initialize command server */
    auto& commands_factory = CommandFactory::get_instance();
    commands_factory.add_commands(configuration);
    auto commands_initialization =
            commands_factory.create_initialization();
    auto commands = commands_factory.create();
    jsonrpc::HttpServer http_server((int(server_port)));
    CommandJsonServer server(http_server);
    server.add(commands);

    /* Start discovery */
    agent::storage::discovery::DiscoveryManager discovery_manager;
    ::agent::storage::StateMachineAction state_action{discovery_manager};

    try {
        /* Create modules */
        ModuleManager::create();
        /* Start state machine */
        state_machine_thread_u_ptr.reset(new StateMachineThread(state_action));
        ::add_state_machine_entries(state_machine_thread_u_ptr.get());
        state_machine_thread_u_ptr->start();

        storage_services_ip_update_u_ptr.reset(new StorageServicesIpUpdateThread());
    } catch (exception & e) {
        log_error(GET_LOGGER("storage-agent"), e.what());
    }

    /* Wait for discovery to complete */
    discovery_manager.wait_for_discovery_complete();

    EventDispatcher event_dispatcher;
    event_dispatcher.start();

    AmcConnectionManager amc_connection(event_dispatcher);
    amc_connection.start();

    /* Start json-rpc command server */
    server.start();

    /* If manager has been found -> send event to REST server */
    if (!ModuleManager::get_modules().empty()) {
        ::agent_framework::eventing::EventData edat;
        edat.set_component(ModuleManager::get_modules().front()->get_name());
        edat.set_type(::agent_framework::model::enums::Component::Manager);
        edat.set_notification(::agent_framework::eventing::Notification::Add);
        ::agent_framework::eventing::EventsQueue::get_instance()->push_back(edat);
    }

    agent::storage::HotswapWatcher hotswap_watcher;
    hotswap_watcher.start();

    storage_services_ip_update_u_ptr->start();

    /* Stop the program and wait for interrupt */
    wait_for_interrupt();

    log_info(GET_LOGGER("storage-agent"), "Stopping PSME Storage...\n");

    /* Cleanup */
    hotswap_watcher.stop();
    server.stop();
    amc_connection.stop();
    event_dispatcher.stop();
    commands_initialization.clear();
    ModuleManager::cleanup();
    command::Command::Map::cleanup();
    command::CommandJson::Map::cleanup();
    command::CommandFactory::cleanup();
    Configuration::cleanup();
    LoggerFactory::cleanup();

    agent_framework::action::TaskRunner::cleanup();
    agent_framework::action::TaskStatusManager::cleanup();

    return 0;
}

void add_state_machine_entries(StateMachineThread* machine_thread) {
    const auto& modules = ModuleManager::get_modules();
    for (const auto& module : modules) {
        machine_thread->add_entry(
                std::make_shared<StateThreadEntry>(module->get_name(),
                std::make_shared<::agent::storage::status::StatusManager>()));
    }
}
