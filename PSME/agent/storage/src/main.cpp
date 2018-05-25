/*!
 * @copyright
 * Copyright (c) 2015-2018 Intel Corporation
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

#include "agent-framework/eventing/utils.hpp"
#include "agent-framework/logger_loader.hpp"
#include "agent-framework/registration/amc_connection_manager.hpp"
#include "agent-framework/signal.hpp"
#include "agent-framework/version.hpp"
#include "agent-framework/action/task_runner.hpp"
#include "agent-framework/command/command_server.hpp"
#include "agent-framework/module/common_components.hpp"

#include "loader/storage_loader.hpp"
#include "configuration/configuration.hpp"
#include "configuration/configuration_validator.hpp"
#include "default_configuration.hpp"
#include "database/database.hpp"

#include "discovery/discovery_manager.hpp"
#include "discovery/lvm_discoverer.hpp"
#include "watcher/hotswap_watcher.hpp"
#include "watcher/ip_watcher.hpp"

#include "json-rpc/connectors/http_server_connector.hpp"

#include <csignal>

using namespace logger_cpp;

using namespace agent::storage;
using namespace agent::storage::loader;
using namespace agent::storage::discovery;
using namespace agent::storage::watcher;

using namespace agent_framework;
using namespace agent_framework::action;
using namespace agent_framework::eventing;
using namespace agent_framework::generic;
using namespace agent_framework::module;

using configuration::Configuration;

using agent::generic::DEFAULT_CONFIGURATION;
using agent::generic::DEFAULT_VALIDATOR_JSON;
using agent::generic::DEFAULT_ENV_FILE;
using agent::generic::DEFAULT_FILE;

static constexpr unsigned int DEFAULT_SERVER_PORT = 7778;

static constexpr int AGENT_ERROR_VALIDATE_CONFIG = -1;
static constexpr int AGENT_ERROR_MODEL_CONFIG = -2;
static constexpr int AGENT_ERROR_AGENT_CONFIG = -10;

const json::Value& init_configuration(int argc, const char** argv);
bool check_configuration(const json::Value& json);

/*!
 * @brief Generic Agent main method.
 * */
int main(int argc, const char* argv[]) {
    std::uint16_t server_port = DEFAULT_SERVER_PORT;
    std::string agent_type{};

    /* Initialize configuration */
    const json::Value& configuration = ::init_configuration(argc, argv);
    if (!::check_configuration(configuration)) {
        return AGENT_ERROR_VALIDATE_CONFIG;
    }

    /* Initialize logger */
    LoggerLoader loader(configuration);
    loader.load(LoggerFactory::instance());
    log_info("storage-agent", "Running PSME Storage...\n");

    /* Load module configuration */
    StorageLoader module_loader{};
    if (!module_loader.load(configuration)) {
        log_error("storage-agent", "Invalid modules configuration");
        return AGENT_ERROR_MODEL_CONFIG;
    }

    try {
        server_port = static_cast<std::uint16_t>(configuration["server"]["port"].as_uint());
    }
    catch (const json::Value::Exception& e) {
        log_error("storage-agent", "Cannot read server port " << e.what());
    }

    if (configuration["database"].is_object() && configuration["database"]["location"].is_string()) {
        database::Database::set_default_location(configuration["database"]["location"].as_string());
    }

    try {
        agent_type = configuration["agent"]["capabilities"].as_array().at(0).as_string();
    }
    catch (const std::out_of_range& e) {
        log_error("storage-agent", "Agent should have at least one capability: " << e.what());
        return AGENT_ERROR_AGENT_CONFIG;
    }
    catch (const std::exception& e) {
        log_error("storage-agent", "Failed to get agent type: " << e.what());
        return AGENT_ERROR_AGENT_CONFIG;
    }

    StorageDiscoverer::SPtr discoverer{std::make_shared<LvmDiscoverer>(LvmDiscoverer())};
    try {
        DiscoveryManager discovery_manager{discoverer};
        discovery_manager.discover();
    }
    catch (const std::exception& e) {
        log_error("storage-agent", e.what());
    }

    RegistrationData registration_data{configuration};

    EventDispatcher event_dispatcher;
    event_dispatcher.start();

    AmcConnectionManager amc_connection(event_dispatcher, registration_data);
    amc_connection.start();


    /* Initialize command server */
    auto http_server_connector = new json_rpc::HttpServerConnector(server_port, registration_data.get_ipv4_address());
    json_rpc::AbstractServerConnectorPtr http_server(http_server_connector);
    agent_framework::command::CommandServer server(http_server);
    server.add(command::Registry::get_instance()->get_commands());

    bool server_started = server.start();
    if (!server_started) {
        log_error("storage-agent", "Could not start JSON-RPC command server on port "
            << server_port << " restricted to " << registration_data.get_ipv4_address()
            << ". " << "Quitting now...");
        amc_connection.stop();
        event_dispatcher.stop();
        return -3;
    }

    /* If manager has been found -> send event to REST server */
    agent_framework::eventing::send_add_notifications_for_each<agent_framework::model::Manager>();

    /* Start Hot-swap manager */
    HotswapWatcher hotswap_watcher{};
    hotswap_watcher.start();

    /* Start IP watcher */
    IpWatcher ip_watcher{discoverer};
    ip_watcher.start();

    /* Stop the program and wait for interrupt */
    wait_for_interrupt();

    log_info("storage-agent", "Stopping PSME Storage...");

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

const json::Value& init_configuration(int argc, const char** argv) {
    log_info("agent", agent_framework::generic::Version::build_info());
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
        log_info("agent", "JSON Schema load!");

        configuration::SchemaErrors errors;
        configuration::SchemaValidator validator;
        configuration::SchemaReader reader;
        reader.load_schema(json_schema, validator);

        validator.validate(json, errors);
        if (!errors.is_valid()) {
            std::cerr << "Configuration invalid: " << errors.to_string() << std::endl;
            return false;
        }
    }
    return true;
}
