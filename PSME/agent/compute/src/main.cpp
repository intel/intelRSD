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

#include "agent-framework/registration/amc_connection_manager.hpp"
#include "agent-framework/signal.hpp"
#include "agent-framework/version.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/model/attributes/event_data.hpp"
#include "agent-framework/eventing/events_queue.hpp"
#include "agent-framework/logger_loader.hpp"
#include "agent-framework/command/command_server.hpp"

#include "logger/logger_factory.hpp"
#include "database/database.hpp"

#include "loader/compute_loader.hpp"
#include "configuration/configuration.hpp"
#include "configuration/configuration_validator.hpp"
#include "default_configuration.hpp"

#include "json-rpc/connectors/http_server_connector.hpp"

#include "status/bmc.hpp"
#include "status/gpio/gpio_manager.hpp"
#include "ipmi/manager/ipmitool/management_controller.hpp"
#include "ipmi/command/generic/get_device_id.hpp"

#include <csignal>


using namespace agent::compute::status;
using namespace agent_framework;
using namespace agent_framework::generic;
using namespace logger_cpp;
using namespace configuration;

using model::attribute::EventData;
using eventing::EventsQueue;

using agent::generic::DEFAULT_CONFIGURATION;
using agent::generic::DEFAULT_VALIDATOR_JSON;
using agent::generic::DEFAULT_ENV_FILE;
using agent::generic::DEFAULT_FILE;

using agent_framework::module::CommonComponents;

namespace {

#define ENABLE_CONFIGURATION_ENCRYPTION // Enable by default.

#ifdef ENABLE_CONFIGURATION_ENCRYPTION


inline std::string decrypt_value(const std::string& value) {
    return configuration::Configuration::get_instance().decrypt_value(value);
}


#else
inline std::string decrypt_value(const std::string& value) {
        return value;
    }
#endif

}

static constexpr unsigned int DEFAULT_SERVER_PORT = 7777;


const json::Json& init_configuration(int argc, const char** argv);


bool check_configuration(const json::Json& json);


BmcCollection load_bmcs(const json::Json& configuration);


/*!
 * @brief PSME SDV Compute Agent main method.
 * */
int main(int argc, const char* argv[]) {
    std::uint16_t server_port = DEFAULT_SERVER_PORT;

    /* Initialize configuration */
    const json::Json& configuration = ::init_configuration(argc, argv);
    if (!::check_configuration(configuration)) {
        return -1;
    }

    agent::compute::loader::ComputeLoader module_loader{};
    if (!module_loader.load(configuration)) {
        log_error("compute-agent", "Invalid modules configuration");
        return -2;
    }

    /* Initialize logger */
    LoggerLoader loader(configuration);
    loader.load(LoggerFactory::instance());
    log_info("compute-agent", "Running SDV PSME Compute Agent.");

    try {
        server_port = configuration.value("server", json::Json::object()).value("port", std::uint16_t{});
    }
    catch (const std::exception& e) {
        log_error("compute-agent", "Cannot read server port: " << e.what());
    }

    if (configuration.value("database", json::Json()).is_object() && configuration.value("database", json::Json::object()).value("location", json::Json()).is_string()) {
        database::Database::set_default_location(configuration["database"]["location"].get<std::string>());
    }

    RegistrationData registration_data{configuration};

    EventDispatcher event_dispatcher{};
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
        log_error("compute-agent", "Could not start JSON-RPC command server on port "
            << server_port << " restricted to " << registration_data.get_ipv4_address()
            << ". " << "Quitting now...");
        amc_connection.stop();
        event_dispatcher.stop();
        return -3;
    }

    try {
        auto bmcs = load_bmcs(configuration);
        for (auto& bmc : bmcs) {
            bmc->start();
        }
        /* Stop the program and wait for interrupt */
        wait_for_interrupt();
        log_info("compute-agent", "Stopping SDV PSME Compute Agent.");
        for (auto& bmc: bmcs) {
            bmc->stop();
        }
    }
    catch (const std::exception& e) {
        log_error("compute-agent", e.what());
        return -10;
    }

    /* Cleanup */
    server.stop();
    amc_connection.stop();
    event_dispatcher.stop();
    Configuration::cleanup();
    LoggerFactory::cleanup();

    return 0;
}


const json::Json& init_configuration(int argc, const char** argv) {
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


bool check_configuration(const json::Json& json) {
    json::Json json_schema = json::Json();
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


BmcCollection load_bmcs(const json::Json& config) {
    BmcCollection bmcs{};
    Bmc::Duration state_update_interval = GpioManager::get_instance()->get_minimal_update_interval();
    for (const auto& manager : config["managers"]) {
        auto slot = manager.value("slot", std::uint16_t{});
        auto read_presence_fn = [slot]() {
            log_debug("compute-agent", "Getting sled presence on slot: " << slot);
            return GpioManager::get_instance()->is_present(uint8_t(slot));
        };

        ConnectionData connection{};
        connection.set_ip_address(manager["ipv4"].get<std::string>());
        connection.set_username(decrypt_value(manager["username"].get<std::string>()));
        connection.set_password(decrypt_value(manager["password"].get<std::string>()));
        connection.set_port(manager["port"].get<std::uint16_t>());
        ipmi::manager::ipmitool::ManagementController mc{
            connection.get_ip_address(), connection.get_port(),
            connection.get_username(), connection.get_password()
        };

        auto read_online_state_fn = [mc]() mutable {
            try {
                ipmi::command::generic::response::GetDeviceId device_rsp{};
                mc.send(ipmi::command::generic::request::GetDeviceId{}, device_rsp);
                log_debug("compute-agent", "BMC " << mc.get_info() << " is online...");
                return true;
            }
            catch (std::exception& e) {
                log_debug("compute-agent", "BMC " << mc.get_info() << " reading state error: " << e.what());
            }
            return false;
        };
        bmcs.push_back(Bmc::Ptr(new agent::compute::Bmc(connection, state_update_interval, read_presence_fn, read_online_state_fn)));
    }
    if (bmcs.empty()) {
        throw std::runtime_error("Invalid modules configuration");
    }
    return bmcs;
}
