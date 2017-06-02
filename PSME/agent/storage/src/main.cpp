/*!
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
 * */

#include "agent-framework/eventing/events_queue.hpp"
#include "agent-framework/logger_loader.hpp"
#include "agent-framework/registration/amc_connection_manager.hpp"
#include "agent-framework/signal.hpp"
#include "agent-framework/version.hpp"
#include "agent-framework/state_machine/state_machine_thread.hpp"
#include "agent-framework/action/task_runner.hpp"

#include "agent-framework/command-ref/command_server.hpp"

#include "agent-framework/module/common_components.hpp"

#include "loader/storage_loader.hpp"
#include "configuration/configuration.hpp"
#include "configuration/configuration_validator.hpp"
#include "default_configuration.hpp"

#include "status/status_manager.hpp"
#include "status/state_machine_action.hpp"
#include "discovery/discovery_manager.hpp"
#include "hotswap/hotswap_watcher.hpp"
#include "ip/ip_watcher.hpp"

#include <jsonrpccpp/server/connectors/httpserver.h>
#include <csignal>

using namespace std;
using namespace logger_cpp;

using namespace agent::storage;
using namespace agent::storage::status;
using namespace agent::storage::loader;
using namespace agent::storage::discovery;

using namespace agent_framework;
using namespace agent_framework::action;
using namespace agent_framework::eventing;
using namespace agent_framework::generic;
using namespace agent_framework::module;
using namespace agent_framework::state_machine;

using configuration::Configuration;

using agent::generic::DEFAULT_CONFIGURATION;
using agent::generic::DEFAULT_VALIDATOR_JSON;
using agent::generic::DEFAULT_ENV_FILE;
using agent::generic::DEFAULT_FILE;

static constexpr unsigned int DEFAULT_SERVER_PORT = 7778;

static constexpr int AGENT_ERROR_VALIDATE_CONFIG = -1;
static constexpr int AGENT_ERROR_MODEL_CONFIG = -2;
static constexpr int AGENT_ERROR_AGENT_CONFIG = -10;

void add_state_machine_entries(StateMachineThread* machine_thread, StateMachineThreadAction& action);
const json::Value& init_configuration(int argc, const char** argv);
bool check_configuration(const json::Value& json);

/*!
 * @brief Generic Agent main method.
 * */
int main(int argc, const char* argv[]) {
    unsigned int server_port = DEFAULT_SERVER_PORT;
    string agent_type{};

    StateMachineThreadUniquePtr state_machine_thread_u_ptr = nullptr;

    /* Initialize configuration */
    const json::Value& configuration = ::init_configuration(argc, argv);
    if (!::check_configuration(configuration)) {
        return AGENT_ERROR_VALIDATE_CONFIG;
    }

    /* Initialize logger */
    LoggerLoader loader(configuration);
    LoggerFactory::instance().set_loggers(loader.load());
    LoggerFactory::set_main_logger_name("agent");
    log_info(GET_LOGGER("agent"), "Running PSME Storage...\n");

    /* Load module configuration */
    StorageLoader module_loader{};
    if (!module_loader.load(configuration)) {
        std::cerr << "Invalid modules configuration" << std::endl;
        return AGENT_ERROR_MODEL_CONFIG;
    }

    try {
        /* read server port */
        server_port = configuration["server"]["port"].as_uint();
    } catch (const json::Value::Exception& e) {
        log_error(GET_LOGGER("agent"), "Cannot read server port " << e.what());
    }

    try {
        /* read the agent type */
        agent_type = configuration["agent"]["capabilities"].
            as_array().front().as_string();
    } catch (const std::exception& e) {
        log_error(GET_LOGGER("agent"),
            "failed to get agent type: " << e.what());
        return AGENT_ERROR_AGENT_CONFIG;
    }

    /* Start discovery */
    DiscoveryManager discovery_manager{};
    StateMachineAction state_action{discovery_manager};

    try {
        /* Start state machine */
        state_machine_thread_u_ptr.reset(new StateMachineThread());
        ::add_state_machine_entries(state_machine_thread_u_ptr.get(), state_action);
        state_machine_thread_u_ptr->start();
    } catch (const exception& e) {
        log_error(GET_LOGGER("agent"), e.what());
    }

    /* Wait for discovery to complete */
    discovery_manager.wait_for_complete();
    log_debug(GET_LOGGER("agent"), "Done waiting for discovery complete");

    EventDispatcher event_dispatcher;
    event_dispatcher.start();

    AmcConnectionManager amc_connection(event_dispatcher);
    amc_connection.start();

    /* Initialize command server */
    jsonrpc::HttpServer http_server((int(server_port)));
    agent_framework::command_ref::CommandServer server(http_server);
    server.add(command_ref::Registry::get_instance()->get_commands());
    server.start();

    /* If manager has been found -> send event to REST server */
    auto& module = CommonComponents::get_instance()->get_module_manager();
    for (const auto& module_uuid : module.get_keys()) {
        EventData edat{};
        edat.set_component(module_uuid);
        edat.set_type(agent_framework::model::enums::Component::Manager);
        edat.set_notification(Notification::Add);
        EventsQueue::get_instance()->push_back(edat);
    }

    /* Start Hot-swap manager */
    HotswapWatcher hotswap_watcher{};
    hotswap_watcher.start();

    /* Start IP watcher */
    IpWatcher ip_watcher{};
    ip_watcher.start();

    /* Stop the program and wait for interrupt */
    wait_for_interrupt();

    log_info(GET_LOGGER("agent"), "Stopping PSME Storage...\n");

    /* Cleanup */
    ip_watcher.stop();
    hotswap_watcher.stop();
    server.stop();
    amc_connection.stop();
    event_dispatcher.stop();
    Configuration::cleanup();
    LoggerFactory::cleanup();

    TaskRunner::cleanup();

    return 0;
}

void add_state_machine_entries(StateMachineThread* machine_thread, StateMachineThreadAction& action) {
    auto& module = CommonComponents::get_instance()->get_module_manager();
    for (const auto& module_uuid : module.get_keys()) {
        auto status_manager = std::make_shared<agent::storage::status::StatusManager>();
        auto state_thread_entry = std::make_shared<StateThreadEntry>(module_uuid, status_manager);
        auto module_thread = std::make_shared<StateMachineModuleThread>(state_thread_entry, action);
        machine_thread->add_module_thread(module_thread);
    }
}

const json::Value& init_configuration(int argc, const char** argv) {
    log_info(GET_LOGGER("agent"),
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

bool check_configuration(const json::Value& json) {
    json::Value json_schema;
    if (configuration::string_to_json(DEFAULT_VALIDATOR_JSON, json_schema)) {
        log_info(GET_LOGGER("agent"), "JSON Schema load!");

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
