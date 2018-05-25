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

#include "logger/logger_factory.hpp"
#include "agent-framework/logger_loader.hpp"

#include "agent-framework/module/compute_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/registration/amc_connection_manager.hpp"
#include "agent-framework/signal.hpp"
#include "agent-framework/version.hpp"

#include "agent-framework/command/command_server.hpp"

#include "asset_configuration/asset_configuration.hpp"
#include "loader/compute_loader.hpp"

#include "configuration/configuration.hpp"
#include "configuration/configuration_validator.hpp"
#include "default_configuration.hpp"

#include "agent-framework/eventing/utils.hpp"

#include "json-rpc/connectors/http_server_connector.hpp"

#include <csignal>

using namespace agent_framework;
using namespace agent_framework::generic;
using namespace logger_cpp;
using namespace configuration;

using agent::generic::DEFAULT_CONFIGURATION;
using agent::generic::DEFAULT_VALIDATOR_JSON;
using agent::generic::DEFAULT_ENV_FILE;
using agent::generic::DEFAULT_FILE;

static constexpr unsigned int DEFAULT_SERVER_PORT = 7777;
const json::Value& init_configuration(int argc, const char** argv);
bool check_configuration(const json::Value& json);

int main(int argc, const char *argv[]) {
    std::uint16_t server_port = DEFAULT_SERVER_PORT;

    /* Initialize configuration */
    const json::Value& configuration = ::init_configuration(argc, argv);
    if (!::check_configuration(configuration)) {
        return -1;
    }

    /* Initialize logger */
    LoggerLoader loader(configuration);
    loader.load(LoggerFactory::instance());
    log_info("compute-agent", "Running Generic Agent...\n");

    /*  Reading asset configuration from xml*/
    auto& asset = agent::AssetConfiguration::get_instance();
    try {
        asset.read_configuration(configuration);
    } catch (const json::Value::Exception& e) {
        log_error("compute-agent",
                "Cannot read asset configuration " << e.what());
    } catch (const xmlpp::exception& e) {
        log_error("compute-agent",
                "Cannot read asset configuration " << e.what());
        agent::AssetConfiguration::cleanup();
        Configuration::cleanup();
        LoggerFactory::cleanup();
        return -1;
    }

    ::agent::compute::loader::ComputeLoader module_loader{};
    if (!module_loader.load(asset.get_drawer())) {
        std::cerr << "Invalid asset configuration" << std::endl;
        return -2;
    }

    try {
        server_port = static_cast<std::uint16_t>(configuration["server"]["port"].as_uint());
    } catch (const json::Value::Exception& e) {
        log_error("compute-agent",
                "Cannot read server port " << e.what());
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
    server.start();

    agent_framework::eventing::send_add_notifications_for_each<agent_framework::model::Manager>();

    /* Stop the program and wait for interrupt */
    wait_for_interrupt();

    log_info("compute-agent", "Stopping Generic Agent...\n");

    /* Cleanup */
    server.stop();
    amc_connection.stop();
    event_dispatcher.stop();
    agent::AssetConfiguration::cleanup();
    Configuration::cleanup();
    LoggerFactory::cleanup();

    return 0;
}

const json::Value& init_configuration(int argc, const char** argv) {
    log_info("compute-agent",
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
        log_info("compute-agent", "JSON Schema load!");

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
