/*!
 * @header{License}
 * @copyright Copyright (c) 2017 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @header{Filesystem}
 * @file rmm/main.cpp
 */

#include "discovery/discovery_manager.hpp"

#include "agent-framework/module/common_components.hpp"
#include "agent-framework/registration/amc_connection_manager.hpp"
#include "agent-framework/signal.hpp"
#include "agent-framework/version.hpp"

#include "loader/rmm_loader.hpp"
#include "configuration/configuration.hpp"
#include "configuration/configuration_validator.hpp"
#include "default_configuration.hpp"
#include "discovery_thread.hpp"
#include "tree_stability/rmm_tree_stabilizer.hpp"

#include "agent-framework/eventing/utils.hpp"
#include "agent-framework/command/command_server.hpp"
#include "database/database.hpp"

#include "json-rpc/connectors/http_server_connector.hpp"

#include <csignal>
#include <cstdio>
#include <memory>

using namespace agent_framework;
using namespace agent_framework::generic;
using namespace logger_cpp;
using namespace configuration;
using namespace agent::rmm::loader;

using agent::generic::DEFAULT_CONFIGURATION;
using agent::generic::DEFAULT_VALIDATOR_JSON;
using agent::generic::DEFAULT_ENV_FILE;
using agent::generic::DEFAULT_FILE;

static constexpr unsigned int DEFAULT_SERVER_PORT = 7791;

static constexpr unsigned int MONITOR_THREAD_INTERVAL_SECONDS = 10;
static constexpr unsigned int DISCOVERY_SLEEP_TIME_SECONDS = 1;

const json::Value& init_configuration(int argc, const char** argv);
bool check_configuration(const json::Value& json);

/*!
 * @brief Generic Agent main method.
 * */
int main(int argc, const char* argv[]) {
    std::uint16_t server_port = DEFAULT_SERVER_PORT;

    /* Initialize configuration */
    const json::Value& configuration = ::init_configuration(argc, argv);
    if (!::check_configuration(configuration)) {
        Configuration::cleanup();
        LoggerFactory::cleanup();
        return 2;
    }

    /* Initialize logger */
    agent::rmm::loader::RmmLoader rmm_loader{};
    if (!rmm_loader.load(configuration)) {
        log_error(GET_LOGGER("rmm-agent"), "Invalid modules configuration");
        Configuration::cleanup();
        LoggerFactory::cleanup();
        return 6;
    }

    LoggerLoader loader(configuration);
    LoggerFactory::instance().set_loggers(loader.load());
    LoggerFactory::set_main_logger_name("agent");
    log_info(GET_LOGGER("agent"), "Running Rmm...\n");

    server_port = static_cast<std::uint16_t>(configuration["server"]["port"].as_uint());
    database::Database::set_default(configuration["database"]["location"].as_string());

    RegistrationData registration_data{configuration};

    EventDispatcher event_dispatcher;
    event_dispatcher.start();

    /* Start DiscoveryThread */
    agent::rmm::DiscoveryThread::DiscoveryThreadUniquePtr discovery_thread;
    discovery_thread.reset(new agent::rmm::DiscoveryThread(MONITOR_THREAD_INTERVAL_SECONDS));
    discovery_thread->start();
    /* Wait for initial discovery to finish */
    while (!discovery_thread->is_discovery_finished()) {
        std::this_thread::sleep_for(std::chrono::seconds(DISCOVERY_SLEEP_TIME_SECONDS));
    }
    log_debug(GET_LOGGER("agent"), "Initial discovery finished!");

    AmcConnectionManager amc_connection(event_dispatcher, registration_data);
    amc_connection.start();

    /* Create command server */
    auto http_server_connector = new json_rpc::HttpServerConnector(server_port, registration_data.get_ipv4_address());
    json_rpc::AbstractServerConnectorPtr http_server(http_server_connector);
    agent_framework::command::CommandServer server(http_server);
    server.add(command::Registry::get_instance()->get_commands());
    bool server_started = server.start();
    if (!server_started) {
        log_error("compute-agent", "Could not start JSON-RPC command server on port "
            << server_port << " restricted to " << registration_data.get_ipv4_address()
            << ". " << "Quitting now...");
        amc_connection.stop();
        event_dispatcher.stop();
        return -3;
    }

    agent_framework::eventing::send_add_notifications_for_each<agent_framework::model::Manager>();

    /* Stop the program and wait for interrupt */
    wait_for_interrupt();

    log_info(GET_LOGGER("agent"), "Stopping Rmm Agent...\n");

    /* Cleanup */
    server.stop();
    amc_connection.stop();
    event_dispatcher.stop();
    Configuration::cleanup();
    LoggerFactory::cleanup();

    return 0;
}

const json::Value& init_configuration(int argc, const char** argv) {
    log_info(GET_LOGGER("rmm-agent"), agent_framework::generic::Version::build_info());
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
        log_info(GET_LOGGER("rmm-agent"), "JSON Schema load!");

        configuration::SchemaErrors errors;
        configuration::SchemaValidator validator;
        configuration::SchemaReader reader;
        reader.load_schema(json_schema, validator);

        validator.validate(json, errors);
        if (!errors.is_valid()) {
            log_error(GET_LOGGER("rmm-agent"), "Configuration invalid: " << errors.to_string());
            return false;
        }
    }
    return true;
}
