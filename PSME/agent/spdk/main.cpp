/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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
 * @file main.cpp
 */

#include "agent-framework/logger_loader.hpp"
#include "agent-framework/version.hpp"
#include "agent-framework/eventing/utils.hpp"
#include "agent-framework/registration/amc_connection_manager.hpp"
#include "agent-framework/signal.hpp"
#include "agent-framework/action/task_runner.hpp"
#include "agent-framework/command/context_command_server.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/database/database_entities.hpp"

#include "json-rpc/connectors/http_server_connector.hpp"
#include "json-rpc/connectors/unix_domain_socket_client_connector.hpp"
#include "configuration/configuration.hpp"
#include "configuration/configuration_validator.hpp"
#include "database/database.hpp"
#include "interface-reader/udev_interface_reader.hpp"
#include "generic/worker_thread.hpp"
#include "utils/transaction/transaction_handler_factory.hpp"
#include "sysfs/sysfs_interface.hpp"

#include "default_configuration.hpp"
#include "spdk_agent_context.hpp"
#include "database_keys.hpp"

#include "discovery/periodic_discovery_manager.hpp"
#include "discovery/discovery_manager.hpp"



using namespace agent::spdk;
using namespace agent_framework::action;
using namespace agent_framework::generic;

static constexpr std::uint16_t DEFAULT_SERVER_PORT = 7784;
static constexpr std::int32_t AGENT_ERROR_VALIDATE_CONFIG = -1;
static constexpr std::int32_t AGENT_ERROR_MODEL_CONFIG = -2;
static constexpr std::int32_t AGENT_ERROR_AGENT_CONFIG = -10;
static constexpr std::int32_t SERVER_START_FAILED = -4;


const json::Json& init_configuration(int argc, const char** argv);


bool check_configuration(const json::Json& json);


/*!
 * @brief Generic Agent main method.
 * */
int main(int argc, const char* argv[]) {
    std::uint16_t server_port = DEFAULT_SERVER_PORT;

    /* Initialize configuration */
    const json::Json& configuration = ::init_configuration(argc, argv);
    if (!::check_configuration(configuration)) {
        return AGENT_ERROR_VALIDATE_CONFIG;
    }

    /* Initialize logger */
    logger_cpp::LoggerLoader loader(configuration);
    loader.load(logger_cpp::LoggerFactory::instance());
    log_info("spdk-agent", "Running PSME SPDK Agent...");

    /* Load module configuration */
    loader::SpdkLoader module_loader{};
    if (!module_loader.load(configuration)) {
        log_error("spdk-agent", "Invalid modules configuration!");
        return AGENT_ERROR_MODEL_CONFIG;
    }

    try {
        server_port = configuration.value("server", json::Json::object())["port"];
    }
    catch (const std::exception& e) {
        log_error("spdk-agent", "Cannot read server port: " << e.what());
    }

    if (configuration.value("database", json::Json()).is_object() &&
        configuration["database"].value("location", json::Json()).is_string()) {
        database::Database::set_default_location(configuration["database"].value("location", std::string{}));
    }

    auto context = std::make_shared<AgentContext>();
    context->configuration = module_loader.get();
    context->sysfs_interface = std::make_shared<sysfs::SysfsInterface>();
    context->transaction_handler_factory = std::make_shared<::utils::transaction::TransactionHandlerFactory>();
    context->interface_reader =
        std::make_shared<interface_reader::UdevInterfaceReader>(context->configuration->get_nic_drivers());

    auto connector =
        std::make_shared<json_rpc::UnixDomainSocketClientConnector>(context->configuration->get_spdk_socket());
    auto invoker = std::make_shared<json_rpc::JsonRpcRequestInvoker>();
    context->spdk_api = std::make_shared<::spdk::SpdkApi>(connector, invoker);

    try {
        discovery::DiscoveryManager discovery_manager{context};
        discovery_manager.discover();
    }
    catch (const std::exception& e) {
        log_error("spdk-agent", "Initial discovery error: " << e.what());
    }

    RegistrationData registration_data{configuration};

    EventDispatcher event_dispatcher;
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
        log_error("spdk-agent", "Could not start JSON-RPC command server on port "
            << server_port << " restricted to " << registration_data.get_ipv4_address()
            << ". " << "Quitting now...");
        amc_connection.stop();
        event_dispatcher.stop();
        return SERVER_START_FAILED;
    }

    /* If manager has been found -> send event to REST server */
    agent_framework::eventing::send_add_notifications_for_each<agent_framework::model::Manager>();

    // Declare periodic discovery task
    discovery::PeriodicDiscoveryManager periodic_discovery_manager{context};
    ::generic::WorkerThread periodic_discovery{};

    auto polling_interval_seconds = context->configuration->get_spdk_polling_interval();
    if (polling_interval_seconds != std::chrono::seconds{0}) {
        auto periodic_discovery_task = [&periodic_discovery_manager] {
            try {
                periodic_discovery_manager.discover();
            }
            catch (const std::exception& exception) {
                log_error("spdk-discovery", "SPDK periodic discovery failed: " << exception.what());
            }
        };

        // Start periodic discovery
        periodic_discovery.fixed_delay_task("SPDK-Periodic-Discovery",
                                            polling_interval_seconds,
                                            polling_interval_seconds,
                                            std::move(periodic_discovery_task));
    }

    // FIXME: Workaround for SPDK deletion snapshot limitation
    const std::chrono::minutes garbage_collector_interval{1};
    ::generic::WorkerThread garbage_collector{};

    auto garbage_collector_task = [context]() {
        auto volumes_to_remove = context->hidden_volumes;
        for (const auto& hidden_volume : volumes_to_remove) {
            try {
                {
                    std::lock_guard<std::mutex> lock{context->spdk_mutex};
                    auto db = agent_framework::database::VolumeEntity(hidden_volume);
                    auto lvol_uuid = db.get(VOLUME_SPDK_UUID_PROPERTY);
                    context->spdk_api->destroy_lvol_bdev(lvol_uuid);
                    context->hidden_volumes.erase(hidden_volume);
                }
                agent_framework::database::VolumeEntity::remove(hidden_volume);

                log_info("garbage-collector", "Hidden volume " << hidden_volume << " successfully deleted.");
            }
            catch (const std::exception& exception) {
                log_debug("garbage-collector",
                    "Cannot remove volume " << hidden_volume << ": " << exception.what());
            }
        }
    };

    garbage_collector.fixed_delay_task("SPDK-Garbage-Collector",
                                       garbage_collector_interval,
                                       garbage_collector_interval,
                                       std::move(garbage_collector_task));

    /* Stop the program and wait for interrupt */
    wait_for_interrupt();

    log_info("spdk-agent", "Stopping PSME SPDK Storage Agent...");

    /* Cleanup */
    garbage_collector.stop();
    periodic_discovery.stop();
    server.stop();
    amc_connection.stop();
    event_dispatcher.stop();
    configuration::Configuration::cleanup();
    logger_cpp::LoggerFactory::cleanup();

    TaskRunner::cleanup();

    return 0;
}


const json::Json& init_configuration(int argc, const char** argv) {
    log_info("spdk-agent", Version::build_info());
    auto& basic_config = configuration::Configuration::get_instance();
    basic_config.set_default_configuration(DEFAULT_CONFIGURATION);
    basic_config.set_default_file(DEFAULT_FILE);
    basic_config.set_default_env_file(DEFAULT_ENV_FILE);
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
        log_info("spdk-agent", "JSON Schema load!");

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
