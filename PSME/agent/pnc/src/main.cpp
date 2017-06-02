/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016-2017 Intel Corporation
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

#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/pnc_components.hpp"
#include "agent-framework/registration/amc_connection_manager.hpp"
#include "agent-framework/signal.hpp"
#include "agent-framework/version.hpp"

#include "agent-framework/eventing/events_queue.hpp"
#include "agent-framework/command-ref/command_server.hpp"

#include "discovery/discovery_manager.hpp"
#include "loader/pnc_loader.hpp"
#include "tree_stability/pnc_tree_stabilizer.hpp"
#include "configuration/configuration.hpp"
#include "configuration/configuration_validator.hpp"
#include "default_configuration.hpp"
#include "port_monitor_thread.hpp"

#include <jsonrpccpp/server/connectors/httpserver.h>

#include <csignal>

using namespace std;
using namespace agent_framework;
using namespace agent_framework::generic;
using namespace agent::pnc::discovery;
using namespace logger_cpp;
using namespace configuration;
using namespace agent::pnc;
using namespace agent::pnc::tools;

using agent::generic::DEFAULT_CONFIGURATION;
using agent::generic::DEFAULT_VALIDATOR_JSON;
using agent::generic::DEFAULT_ENV_FILE;
using agent::generic::DEFAULT_FILE;

static constexpr unsigned int DEFAULT_SERVER_PORT = 7781;

static constexpr unsigned int PORT_MONITOR_THREAD_INTERVAL_SECONDS = 10;
static constexpr unsigned int DISCOVERY_SLEEP_TIME_SECONDS = 1;

const json::Value& init_configuration(int argc, const char** argv);
bool check_configuration(const json::Value& json);

/*!
 * @brief Generic Agent main method.
 * */
int main(int argc, const char* argv[]) {
    unsigned int server_port = DEFAULT_SERVER_PORT;

    /* Initialize configuration */
    log_info(GET_LOGGER("agent"),
        agent_framework::generic::Version::build_info());

    /* Initialize configuration */
    const json::Value& configuration = ::init_configuration(argc, argv);
    if (!::check_configuration(configuration)) {
        Configuration::cleanup();
        LoggerFactory::cleanup();
        return 2;
    }

    ::agent::pnc::loader::PncLoader module_loader{};
    if (!module_loader.load(configuration)) {
        log_error(GET_LOGGER("pnc-agent"), "Invalid modules configuration");
        Configuration::cleanup();
        LoggerFactory::cleanup();
        return 6;
    }

    /* Initialize logger */
    LoggerLoader loader(configuration);
    LoggerFactory::instance().set_loggers(loader.load());
    LoggerFactory::set_main_logger_name("pnc-agent");
    log_info(GET_LOGGER("pnc-agent"), "Running PSME Pnc...");

    try {
        server_port = configuration["server"]["port"].as_uint();
    } catch (const json::Value::Exception& e) {
        log_error(GET_LOGGER("pnc-agent"),
                "Cannot read server port " << e.what());
    }

    EventDispatcher event_dispatcher;
    event_dispatcher.start();

    auto tools = Toolset::get();
    DiscoveryManager dm = DiscoveryManager::create(tools);
    try {
        dm.discovery("");
        // stabilize switch ports -> needed for correct port state manager operation
        for (const auto& elem : ::agent_framework::module::CommonComponents::get_instance()->
                get_module_manager().get_keys()) {
            ::agent::pnc::PncTreeStabilizer().stabilize(elem);
        }
    }
    catch (PncDiscoveryExceptionSwitchNotFound&) {
        log_error(GET_LOGGER("pnc-agent"), "No PCIe switches found, exiting.");
        event_dispatcher.stop();
        Configuration::cleanup();
        LoggerFactory::cleanup();
        return 1;
    }
    catch (const std::exception& e) {
        log_error(GET_LOGGER("pnc-discovery"), "Discovery FAILED: " << e.what());
    }

    /* Start Port Monitor Thread */
    PortMonitorThread::PortMonitorThreadUniquePtr port_monitor_thread;
    auto switches = ::agent_framework::module::PncComponents::get_instance()->get_switch_manager().get_keys();
    if (switches.size() > 0) {
        port_monitor_thread.reset(new PortMonitorThread(switches.front(), tools, PORT_MONITOR_THREAD_INTERVAL_SECONDS));
        port_monitor_thread->start();
    }

    /* Wait for initial discovery to finish */
    while (!port_monitor_thread->is_discovery_finished()) {
        std::this_thread::sleep_for(std::chrono::seconds(::DISCOVERY_SLEEP_TIME_SECONDS));
    }

    /* Create command server */
    jsonrpc::HttpServer http_server((int(server_port)));
    agent_framework::command_ref::CommandServer server(http_server);
    server.add(command_ref::Registry::get_instance()->get_commands());
    server.start();

    /* all events up to this point were ignored */
    AmcConnectionManager amc_connection(event_dispatcher);
    amc_connection.start();

    /* Send add event */
    for (const auto& elem : ::agent_framework::module::CommonComponents::get_instance()->
            get_module_manager().get_keys()) {
        const std::string module_persistent_uuid = ::agent::pnc::PncTreeStabilizer().stabilize(elem);
        tools.model_tool->send_event("", module_persistent_uuid,
            ::agent_framework::model::enums::Component::Manager,
            ::agent_framework::eventing::Notification::Add);
    }

    /* Stop the program and wait for interrupt */
    wait_for_interrupt();

    log_info(GET_LOGGER("pnc-agent"), "Stopping PSME Pnc Agent...\n");

    /* Cleanup */
    server.stop();
    amc_connection.stop();
    event_dispatcher.stop();
    Configuration::cleanup();
    LoggerFactory::cleanup();

    return 0;
}

const json::Value& init_configuration(int argc, const char** argv) {
    log_info(GET_LOGGER("pnc-agent"),
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
        log_info(GET_LOGGER("pnc-agent"), "JSON Schema load!");

        configuration::SchemaErrors errors;
        configuration::SchemaValidator validator;
        configuration::SchemaReader reader;
        reader.load_schema(json_schema, validator);

        validator.validate(json, errors);
        if (!errors.is_valid()) {
            log_error(GET_LOGGER("pnc-agent"), "Configuration invalid: " << errors.to_string());
            return false;
        }
    }
    return true;
}
