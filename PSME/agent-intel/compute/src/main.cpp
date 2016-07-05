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
#include "agent-framework/registration/amc_connection_manager.hpp"
#include "agent-framework/signal.hpp"
#include "agent-framework/version.hpp"
#include "agent-framework/state_machine/state_machine_thread.hpp"
#include "agent-framework/module-ref/compute_manager.hpp"

#include "agent-framework/eventing/event_data.hpp"
#include "agent-framework/eventing/events_queue.hpp"

#include "agent-framework/command/command.hpp"
#include "agent-framework/command/command_factory.hpp"
#include "agent-framework/command/command_json.hpp"
#include "agent-framework/command/command_json_server.hpp"

#include "loader/compute_loader.hpp"
#include "discovery/discovery_manager.hpp"
#include "status/status_manager.hpp"
#include "status/state_machine_action.hpp"

#include "configuration/configuration.hpp"
#include "configuration/configuration_validator.hpp"
#include "default_configuration.hpp"

#include <jsonrpccpp/server/connectors/httpserver.h>

#include <csignal>
#include <cstdio>
#include <memory>
#include <iostream>

using namespace std;
using namespace agent::compute::status;
using namespace agent_framework;
using namespace agent_framework::generic;
using namespace agent_framework::state_machine;
using namespace logger_cpp;
using namespace configuration;

using command::Command;
using command::CommandFactory;
using command::CommandJson;
using command::CommandJsonServer;
using eventing::EventData;
using eventing::EventsQueue;

using agent::generic::DEFAULT_CONFIGURATION;
using agent::generic::DEFAULT_VALIDATOR_JSON;
using agent::generic::DEFAULT_ENV_FILE;
using agent::generic::DEFAULT_FILE;

using ComputeComponents = agent_framework::module::ComputeManager;

static constexpr unsigned int DEFAULT_SERVER_PORT = 7777;
static constexpr const char COMMANDS_IMPLEMENTATION[] = "Intel";

void add_state_machine_entries(StateMachineThread* machine_thread);
const json::Value& init_configuration(int argc, const char** argv);
bool check_configuration(const json::Value& json);

/*!
 * @brief PSME Intel Compute Agent main method.
 * */
int main(int argc, const char* argv[]) {
    using agent::compute::StateMachineAction;
    using StateMachineActionUnique = unique_ptr<StateMachineAction>;

    unsigned int server_port = DEFAULT_SERVER_PORT;

    StateMachineThreadUniquePtr state_machine_thread_u_ptr = nullptr;
    StateMachineActionUnique state_action = nullptr;

    /* Initialize configuration */
    const json::Value& configuration = ::init_configuration(argc, argv);
    if (!::check_configuration(configuration)) {
        return -1;
    }

    ::agent::compute::loader::ComputeLoader module_loader{};
    if (!module_loader.load(configuration)) {
        std::cerr << "Invalid modules configuration" << std::endl;
        return -2;
    }

    /* Initialize logger */
    LoggerLoader loader(configuration);
    LoggerFactory::instance().set_loggers(loader.load());
    LoggerFactory::set_main_logger_name("agent");
    log_info(GET_LOGGER("compute-agent"), "Running Intel PSME Compute Agent.\n");

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
        log_error(GET_LOGGER("compute-agent"), "Invalid agent configuration: " << e.what());
        return -10;
    }
    auto commands_initialization =
        commands_factory.create_initialization();
    auto commands = commands_factory.create();

    jsonrpc::HttpServer http_server((int(server_port)));
    CommandJsonServer server(http_server);

    server.add(commands);
    server.start();

    try {
        state_action.reset(new StateMachineAction());

        /* Start state machine */
        state_machine_thread_u_ptr.reset(new StateMachineThread(*state_action));

        ::add_state_machine_entries(state_machine_thread_u_ptr.get());
        /* Start RPC Client */

        state_machine_thread_u_ptr->start();
    }
    catch (const std::exception& e) {
        log_error(GET_LOGGER("compute-agent"), e.what());
        return -10;
    }

    auto cc = ComputeComponents::get_instance();
    auto managers_uuids_vec = cc->get_module_manager().get_keys();

    for (const auto& manager_uuid: managers_uuids_vec) {
        EventData event_data{};
        event_data.set_component(manager_uuid);
        event_data.set_type(agent_framework::model::enums::Component::Manager);
        event_data.set_notification(eventing::Notification::Add);
        EventsQueue::get_instance()->push_back(event_data);
    }

    /* Stop the program and wait for interrupt */
    wait_for_interrupt();

    log_info(GET_LOGGER("compute-agent"), "Stopping Intel PSME Compute Agent.\n");

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

const json::Value& init_configuration(int argc, const char** argv) {
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
    return basic_config.to_json();
}

void add_state_machine_entries(StateMachineThread* machine_thread) {
    auto keys = ComputeComponents::get_instance()->
            get_module_manager().get_keys();
    for (const auto& key : keys) {
        auto entry = ComputeComponents::get_instance()->
                get_module_manager().get_entry(key);
        machine_thread->add_entry(std::make_shared<StateThreadEntry>(key,
                std::make_shared<::agent::compute::status::StatusManager>
                (entry.get_slot(),
                entry.get_connection_data().get_ip_address())));
    }
}
bool check_configuration(const json::Value& json) {
    json::Value json_schema;
    if (configuration::string_to_json(DEFAULT_VALIDATOR_JSON, json_schema)) {
        log_info(GET_LOGGER("compute-agent"), "JSON Schema load!");

        configuration::SchemaErrors errors;
        configuration::SchemaValidator validator;
        configuration::SchemaReader reader;
        reader.load_schema(json_schema, validator);

        validator.validate(json, errors);
        if (!errors.is_valid()) {
            std::cerr << "Configuration invalid: " <<
                         errors.to_string() << std::endl;
            return false;
        }
    }
    return true;
}
