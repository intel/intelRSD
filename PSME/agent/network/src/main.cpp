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
 * */

#include "network_config.hpp"

#include "agent-framework/logger_loader.hpp"
#include "logger/logger_factory.hpp"

#include "agent-framework/registration/amc_connection_manager.hpp"
#include "agent-framework/signal.hpp"
#include "agent-framework/version.hpp"
#include "agent-framework/module/common_components.hpp"

#include "agent-framework/command/command_server.hpp"

#include "loader/network_loader.hpp"
#include "configuration/configuration.hpp"
#include "configuration/configuration_validator.hpp"
#include "default_configuration.hpp"
#include "discovery/discovery_manager.hpp"
#include "database/database.hpp"

#include "hal/switch_agent.hpp"

#include "json-rpc/connectors/http_server_connector.hpp"

#include <csignal>

using namespace std;
using namespace agent_framework;
using namespace agent_framework::generic;
using namespace logger_cpp;

using namespace agent::network;
using namespace agent::network::discovery;

using configuration::Configuration;

using agent::generic::DEFAULT_CONFIGURATION;
using agent::generic::DEFAULT_VALIDATOR_JSON;
using agent::generic::DEFAULT_ENV_FILE;
using agent::generic::DEFAULT_FILE;

static constexpr unsigned int DEFAULT_SERVER_PORT = 7779;
static constexpr int CONFIGURATION_VALIDATION_ERROR_CODE = -1;
static constexpr int INVALID_MODULES_CONFIGURATION_ERROR_CODE = -2;

const json::Json& init_configuration(int argc, const char** argv);
bool check_configuration(const json::Json& json);

/*!
 * @brief PSME Network Agent main method.
 * */
int main(int argc, const char* argv[]) {
    std::uint16_t server_port = DEFAULT_SERVER_PORT;

    /* Initialize configuration */
    const json::Json& configuration = ::init_configuration(argc, argv);
    if (!::check_configuration(configuration)) {
        return CONFIGURATION_VALIDATION_ERROR_CODE;
    }

    /* Initialize logger */
    LoggerLoader loader(configuration);
    loader.load(LoggerFactory::instance());
    log_info("network-agent", "Running PSME Network Agent...");

    /* Load module configuration */
    ::agent::network::loader::NetworkLoader module_loader{};
    if (!module_loader.load(configuration)) {
        log_error("network-agent", "Invalid modules configuration");
        return INVALID_MODULES_CONFIGURATION_ERROR_CODE;
    }

    try {
        server_port = configuration["server"]["port"].get<std::uint16_t>();
    }
    catch (const std::exception& e) {
        log_error("network-agent", "Cannot read server port " << e.what());
    }

    if (configuration.value("database", json::Json::object()).value("location", json::Json()).is_string()) {
        database::Database::set_default_location(configuration["database"]["location"].get<std::string>());
    }

    RegistrationData registration_data{configuration};

    EventDispatcher event_dispatcher;
    event_dispatcher.start();

    AmcConnectionManager amc_connection(event_dispatcher, registration_data);
    amc_connection.start();

    /* Start underlying switch agent before the discovery starts */
    hal::SwitchAgent::get_instance()->start();
    /* wait for the initialization */
    hal::SwitchAgent::get_instance()->wait_until_initialized();

    try {
        /* Start discovery manager */
        const auto manager_uuids = agent_framework::module::get_manager<agent_framework::model::Manager>().get_keys();
        for (const auto& uuid : manager_uuids) {
            DiscoveryManager discovery_manager{};
            discovery_manager.discovery(uuid);
        }
    }
    catch (const std::exception &e) {
        log_error("network-agent", e.what());
    }

    /* Initialize command server */
    auto http_server_connector = new json_rpc::HttpServerConnector(server_port, registration_data.get_ipv4_address());
    json_rpc::AbstractServerConnectorPtr http_server(http_server_connector);
    agent_framework::command::CommandServer server(http_server);
    server.add(command::Registry::get_instance()->get_commands());
    bool server_started = server.start();
    if (!server_started) {
        log_error("network-agent", "Could not start JSON-RPC command server on port "
            << server_port << " restricted to " << registration_data.get_ipv4_address()
            << ". " << "Quitting now...");
        amc_connection.stop();
        event_dispatcher.stop();
        return -3;
    }

    /* Stop the program and wait for interrupt */
    wait_for_interrupt();

    log_info("network-agent", "Stopping PSME Network Agent...");

    /* Cleanup */
    server.stop();
    /* stop underlying switch agent */
    hal::SwitchAgent::get_instance()->stop();
    amc_connection.stop();
    event_dispatcher.stop();
    Configuration::cleanup();
    LoggerFactory::cleanup();

    return 0;
}

const json::Json& init_configuration(int argc, const char** argv) {
    log_info("network-agent",
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
    /* FIXME default configuration for network: command line args are not available in Arista! */
    basic_config.add_file("/etc/psme/network-config.json");
    basic_config.load_key_file();
    return basic_config.to_json();
}

bool check_configuration(const json::Json& json) {
    json::Json json_schema = json::Json();
    if (configuration::string_to_json(DEFAULT_VALIDATOR_JSON, json_schema)) {
        log_info("network-agent", "JSON Schema load!");

        configuration::SchemaErrors errors;
        configuration::SchemaValidator validator;
        configuration::SchemaReader reader;
        reader.load_schema(json_schema, validator);
        validator.validate(json, errors);

        if (!errors.is_valid()) {
            std::cerr << "Configuration invalid: " << errors.to_string() <<
                                                                std::endl;
            return false;
        }
    }
    return true;
}
