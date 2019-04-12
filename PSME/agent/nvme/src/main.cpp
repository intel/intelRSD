/*!
 * @brief Implementation of the main function.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file main.cpp
 */

#include "agent-framework/logger_loader.hpp"
#include "agent-framework/eventing/utils.hpp"
#include "agent-framework/registration/amc_connection_manager.hpp"
#include "agent-framework/signal.hpp"
#include "agent-framework/version.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/service_uuid.hpp"
#include "agent-framework/command/context_command_server.hpp"

#include "loader/loader.hpp"
#include "loader/config.hpp"
#include "tools/endpoint_creator.hpp"
#include "configuration/configuration.hpp"
#include "configuration/configuration_validator.hpp"
#include "default_configuration.hpp"
#include "discovery/discovery_manager.hpp"
#include "tree_stability/nvme_key_generator.hpp"
#include "telemetry/nvme_constants.hpp"
#include "telemetry/nvme_telemetry_service.hpp"
#include "tools/periodic_task.hpp"
#include "tools/default_database_factory.hpp"
#include "database/database.hpp"
#include "utils/transaction/transaction_handler_factory.hpp"

#include "tools/drive_discovery/drive_reader.hpp"
#include "tools/drive_discovery/drive_handler_factory.hpp"
#include "nvme/nvme_interface.hpp"
#include "nvme-target/sysfs_nvme_target_handler.hpp"
#include "partition/file_drive_interface.hpp"
#include "partition/thread_safe_drive_interface_proxy.hpp"
#include "sysfs/sysfs_interface.hpp"
#include "json-rpc/connectors/http_server_connector.hpp"

#include <csignal>



using namespace agent_framework;
using namespace agent_framework::generic;
using namespace logger_cpp;

using namespace agent::nvme;
using namespace agent::nvme::discovery;
using namespace agent::nvme::telemetry;
using namespace agent::nvme::loader;
using namespace agent::nvme::tools;

using configuration::Configuration;

using agent::generic::DEFAULT_CONFIGURATION;
using agent::generic::DEFAULT_VALIDATOR_JSON;
using agent::generic::DEFAULT_ENV_FILE;
using agent::generic::DEFAULT_FILE;

static constexpr unsigned int DEFAULT_SERVER_PORT = 7782;
static constexpr int CONFIGURATION_VALIDATION_ERROR_CODE = -1;
static constexpr int INVALID_MODULES_CONFIGURATION_ERROR_CODE = -2;


const json::Json& init_configuration(int argc, const char** argv);


bool check_configuration(const json::Json& json);


/*!
 * @brief PSME NVMe Agent main method.
 * */
int main(int argc, const char* argv[]) {
    std::uint16_t server_port = DEFAULT_SERVER_PORT;

    // initialize EndpointCreator object
    NvmeConfig::EndpointCreatorPtr endpoint_creator{new EndpointCreator()};
    NvmeConfig::get_instance()->set_endpoint_creator(endpoint_creator);

    /* Initialize configuration */
    const json::Json& configuration = ::init_configuration(argc, argv);
    if (!::check_configuration(configuration)) {
        return CONFIGURATION_VALIDATION_ERROR_CODE;
    }

    /* Initialize logger */
    LoggerLoader loader(configuration);
    loader.load(LoggerFactory::instance());
    log_info("nvme-agent", "Running PSME NVMe Agent...");

    /* Load module configuration */
    loader::NvmeLoader module_loader{};
    if (!module_loader.load(configuration)) {
        log_error("nvme-agent", "Invalid modules configuration");
        return INVALID_MODULES_CONFIGURATION_ERROR_CODE;
    }

    try {
        server_port = configuration.value("server", json::Json::object()).value("port", std::uint16_t{});
    }
    catch (const std::exception& e) {
        log_error("nvme-agent", "Cannot read server port " << e.what());
    }

    if (configuration.value("database", json::Json::object()).value("location", json::Json()).is_string()) {
        database::Database::set_default_location(configuration["database"]["location"].get<std::string>());
    }

    EventDispatcher event_dispatcher;
    event_dispatcher.start();

    RegistrationData registration_data{configuration};

    AmcConnectionManager amc_connection(event_dispatcher, registration_data);
    amc_connection.start();

    // Context containing all interfaces required by the agent, note that all interfaces should be thread-safe
    auto context = std::make_shared<NvmeAgentContext>();
    // NvmeInterface is thread-safe by the way NVMe works
    context->nvme_interface = std::make_shared<::nvme::NvmeInterface>();
    // FileDriveInterface is not thread safe, so Thread Safe proxy is used
    context->drive_interface = std::make_shared<::partition::ThreadSafeDriveInterfaceProxy>(
        std::make_shared<::partition::FileDriveInterface>());
    // DriveReader is stateless + it uses SysfsInterface which is thread-safe as much as the file operations in the OS
    context->drive_reader = std::make_shared<DriveReader>(std::make_shared<::sysfs::SysfsInterface>());
    // DriveHandlerFactory is thread safe as long as provided NvmeInterface, DriveInterface implementations
    // are thread-safe. To make it work correctly all built DriveHandler classes also have to be thread-safe
    context->drive_handler_factory = std::make_shared<DriveHandlerFactory>(
        context->nvme_interface, context->drive_interface);
    // The factory itself is thread safe but the factories themselves don't have to be thread safe
    context->transaction_handler_factory = std::make_shared<::utils::transaction::TransactionHandlerFactory>();
    // The is stateless and thread safe as long as the sysfs interface is thread safe
    context->nvme_target_handler = std::make_shared<nvme_target::SysfsNvmeTargetHandler>(std::make_shared<sysfs::SysfsInterface>());
    // The factory is stateless but the databases don't have to be thread safe
    context->db_factory = std::make_shared<DefaultDatabaseFactory>();

    // initialize key generator
    NvmeKeyGenerator::set_agent_id(module::ServiceUuid::get_instance()->get_service_uuid());
    try {
        /* Start discovery manager */
        DiscoveryManager discovery_manager{};
        discovery_manager.discover(context);
    }
    catch (std::exception& e) {
        log_error("nvme-agent", e.what());
    }

    /* Initialize command server */
    auto http_server_connector = new json_rpc::HttpServerConnector(server_port, registration_data.get_ipv4_address());
    json_rpc::AbstractServerConnectorPtr http_server(http_server_connector);
    agent_framework::command::ContextCommandServer<NvmeAgentContext> server(http_server, context);
    server.add(command::ContextRegistry<NvmeAgentContext>::get_instance()->get_commands());
    bool server_started = server.start();
    if (!server_started) {
        log_error("nvme-agent", "Could not start JSON-RPC command server on port "
            << server_port << " restricted to " << registration_data.get_ipv4_address()
            << ". " << "Quitting now...");
        amc_connection.stop();
        event_dispatcher.stop();
        return -3;
    }

    /* Send add event */
    agent_framework::eventing::send_add_notifications_for_each<agent_framework::model::Manager>();

    /* Initialize Telemetry */
    TelemetryService ts;
    auto initialize = [&ts, &context] {
        ts.init(context);
    };
    auto update_metrics = [&ts, &context] {
        ts.update_metrics(context);
    };

    // start periodic telemetry
    tools::PeriodicTask telemetry_monitor{Constants::TELEMETRY_DELAY, update_metrics, initialize};
    telemetry_monitor.start();

    /* Stop the program and wait for interrupt */
    wait_for_interrupt();

    log_info("nvme-agent", "Stopping PSME NVMe Agent...");

    /* Cleanup */
    server.stop();
    amc_connection.stop();
    event_dispatcher.stop();
    telemetry_monitor.stop();
    Configuration::cleanup();
    LoggerFactory::cleanup();

    return 0;
}


const json::Json& init_configuration(int argc, const char** argv) {
    log_info("nvme-agent", agent_framework::generic::Version::build_info());
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


bool check_configuration(const json::Json& json) {
    json::Json json_schema = json::Json();
    if (configuration::string_to_json(DEFAULT_VALIDATOR_JSON, json_schema)) {
        log_info("nvme-agent", "JSON Schema load!");

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
