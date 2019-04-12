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

#include "agent-framework/eventing/utils.hpp"
#include "agent-framework/logger_loader.hpp"
#include "agent-framework/signal.hpp"
#include "agent-framework/version.hpp"
#include "agent-framework/action/task_runner.hpp"
#include "agent-framework/command/context_command_server.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/registration/amc_connection_manager.hpp"

#include "json-rpc/connectors/http_server_connector.hpp"
#include "configuration/configuration.hpp"
#include "configuration/configuration_validator.hpp"
#include "database/database.hpp"
#include "logger/logger.hpp"

#include "storage_agent_context.hpp"
#include "storage_config.hpp"
#include "default_configuration.hpp"

#include "loader/storage_loader.hpp"
#include "discovery/discovery_manager.hpp"

#ifdef TGT_STORAGE // TODO: NVMe+LVM Out of scope
#include "agent/watcher/hotswap_watcher.hpp"
#include "agent/watcher/ip_watcher.hpp"
#endif

#include <csignal>
#include <cstdint>
#include <string>



using namespace agent::storage;

using namespace agent_framework::eventing;
using namespace agent_framework::generic;

static constexpr unsigned int DEFAULT_SERVER_PORT = 7778;

static constexpr int AGENT_ERROR_VALIDATE_CONFIG = -1;
static constexpr int AGENT_ERROR_MODEL_CONFIG = -2;
static constexpr int DISCOVERY_FAILED = -3;
static constexpr int SERVER_START_FAILED = -4;
static constexpr int AGENT_ERROR_AGENT_CONFIG = -10;

const json::Json& init_configuration(int argc, const char** argv);

bool check_configuration(const json::Json& json);

int main(int argc, const char* argv[]) {
    std::uint16_t server_port = DEFAULT_SERVER_PORT;
    std::string agent_type{};

    /* Initialize configuration */
    const json::Json& configuration = ::init_configuration(argc, argv);
    if (!::check_configuration(configuration)) {
        return AGENT_ERROR_VALIDATE_CONFIG;
    }

    /* Initialize logger */
    logger_cpp::LoggerLoader loader(configuration);
    loader.load(logger_cpp::LoggerFactory::instance());
    log_info("storage-agent", "Running PSME Storage...");

    /* Load module configuration */
    loader::StorageLoader module_loader{};
    if (!module_loader.load(configuration)) {
        log_error("storage-agent", "Invalid modules configuration!");
        return AGENT_ERROR_MODEL_CONFIG;
    }

    try {
        server_port = configuration.value("server", json::Json::object()).value("port", std::uint16_t{});
    }
    catch (const std::exception& e) {
        log_error("storage-agent", "Cannot read server port " << e.what());
    }

    if (configuration.value("database", json::Json()).is_object() && configuration["database"].value("location", json::Json()).is_string()) {
        database::Database::set_default_location(configuration["database"].value("location", std::string{}));
    }

    try {
        if (configuration.at("agent").at("capabilities").size() == 0){
            log_error("storage-agent", "Agent should have at least one capability.");
            return AGENT_ERROR_AGENT_CONFIG;
        }
        configuration.at("agent").at("capabilities").at(0).get<std::string>();
    }
    catch (const std::exception& e) {
        log_error("storage-agent", "Failed to get agent type: " << e.what());
        return AGENT_ERROR_AGENT_CONFIG;
    }

    // Configure context containing all interfaces required by the agent
    // Note: that all interfaces should be thread-safe
    auto context = std::make_shared<AgentContext>();
    context->initialize(module_loader.get());

    try {
        discovery::DiscoveryManager discovery_manager{context};
        discovery_manager.discover();
    }
    catch (const std::exception& e) {
        log_error("storage-agent", e.what());
        return DISCOVERY_FAILED;
    }

    RegistrationData registration_data{configuration};

    EventDispatcher event_dispatcher{};
    event_dispatcher.start();

    AmcConnectionManager amc_connection(event_dispatcher, registration_data);
    amc_connection.start();

    /* Initialize command server */
    auto http_server_connector = new json_rpc::HttpServerConnector(server_port, registration_data.get_ipv4_address());
    json_rpc::AbstractServerConnectorPtr http_server(http_server_connector);
    agent_framework::command::ContextCommandServer<AgentContext> server(http_server, context);
    server.add(agent_framework::command::ContextRegistry<AgentContext>::get_instance()->get_commands());
    bool server_started = server.start();
    if (!server_started) {
        log_error("storage-agent", "Could not start JSON-RPC command server on port "
            << server_port << " restricted to " << registration_data.get_ipv4_address()
            << ". " << "Quitting now...");
        amc_connection.stop();
        event_dispatcher.stop();
        return SERVER_START_FAILED;
    }

    /* If manager has been found -> send event to REST server */
    send_add_notifications_for_each<agent_framework::model::Manager>();

// TODO: LVM+NVMe Out of scope
#ifdef TGT_STORAGE
    /* Start Hot-swap manager */
    agent::storage::watcher::HotswapWatcher hotswap_watcher{context};
    hotswap_watcher.start();

    /* Start IP watcher */
    agent::storage::watcher::IpWatcher ip_watcher{context};
    ip_watcher.start();
#endif

    /* Stop the program and wait for interrupt */
    wait_for_interrupt();

    log_info("storage-agent", "Stopping PSME Storage...");

    /* Cleanup */
#ifdef TGT_STORAGE
    ip_watcher.stop();
    hotswap_watcher.stop();
#endif
    server.stop();
    amc_connection.stop();
    event_dispatcher.stop();
    configuration::Configuration::cleanup();
    logger_cpp::LoggerFactory::cleanup();

    agent_framework::action::TaskRunner::cleanup();

    return 0;
}

const json::Json& init_configuration(int argc, const char** argv) {
    log_info("storage-agent", agent_framework::generic::Version::build_info());

    auto& basic_config = configuration::Configuration::get_instance();
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

bool check_configuration(const json::Json& json) {
    json::Json json_schema{};
    if (configuration::string_to_json(DEFAULT_VALIDATOR_JSON, json_schema)) {
        log_info("agent-storage", "JSON Schema load!");

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
