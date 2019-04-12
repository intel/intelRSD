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
#include "agent-framework/module/compute_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/eventing/utils.hpp"
#include "agent-framework/command/command_server.hpp"
#include "agent-framework/logger_loader.hpp"

#include "loader/chassis_loader.hpp"
#include "database/database.hpp"

#include "configuration/configuration.hpp"
#include "configuration/configuration_validator.hpp"
#include "default_configuration.hpp"

#include "ipmb/service.hpp"
#include "ipmb/mux.hpp"

#include "status/bmc.hpp"
#include "ipmb/gpio.hpp"
#include "ipmi/management_controller.hpp"
#include "ipmi/command/generic/get_device_id.hpp"
#include "tree_stability/chassis_tree_stabilizer.hpp"

#include "json-rpc/connectors/http_server_connector.hpp"
#include <csignal>

using namespace agent_framework;
using namespace agent_framework::generic;
using namespace logger_cpp;
using namespace configuration;
using namespace agent::chassis;
using namespace agent::chassis::ipmb;
using namespace agent_framework::eventing;

using agent::generic::DEFAULT_CONFIGURATION;
using agent::generic::DEFAULT_VALIDATOR_JSON;
using agent::generic::DEFAULT_ENV_FILE;
using agent::generic::DEFAULT_FILE;

using agent_framework::module::CommonComponents;

static constexpr unsigned int DEFAULT_SERVER_PORT = 7780;

const json::Json& init_configuration(int argc, const char** argv);
bool check_configuration(const json::Json& json);

BmcCollection load_bmcs(const json::Json& configuration);

/*!
 * @brief Generic Agent main method.
 * */
int main(int argc, const char* argv[]) {
    std::uint16_t server_port = DEFAULT_SERVER_PORT;

    /* Initialize configuration */
    const json::Json& configuration = ::init_configuration(argc, argv);
    if (!::check_configuration(configuration)) {
        return -1;
    }

    /* Initialize logger */
    LoggerLoader loader(configuration);
    loader.load(LoggerFactory::instance());
    log_info("chassis-agent", "Running PSME Chassis SDV...");

    if (configuration.value("database", json::Json()).is_object() && configuration.value("database", json::Json::object()).value("location", json::Json()).is_string()) {
        database::Database::set_default_location(configuration["database"]["location"].get<std::string>());
    }

    try {
        server_port = configuration.value("server", json::Json::object()).value("port", std::uint16_t{});
    }
    catch (const std::exception& e) {
        log_error("chassis-agent", "Cannot read server port " << e.what());
    }

    agent::chassis::loader::ChassisLoader module_loader{};
    if (!module_loader.load(configuration)) {
        log_error("chassis-agent", "Invalid modules configuration");
        return -2;
    }

    BmcCollection bmcs{};
    try {
        bmcs = load_bmcs(configuration);
        for (auto& bmc : bmcs) {
            bmc->start();
        }
    }
    catch (const std::exception& e) {
        log_error("chassis-agent", e.what());
        return -1;
    }

    ipmb::Service ipmb_service;
    ipmb_service.start();

    EventDispatcher event_dispatcher;
    event_dispatcher.start();

    RegistrationData registration_data{configuration};

    AmcConnectionManager amc_connection(event_dispatcher, registration_data);
    amc_connection.start();

    /* Initialize command server */
    auto http_server_connector = new json_rpc::HttpServerConnector(server_port, registration_data.get_ipv4_address());
    json_rpc::AbstractServerConnectorPtr http_server(http_server_connector);
    agent_framework::command::CommandServer server(http_server);
    server.add(command::Registry::get_instance()->get_commands());

    bool server_started = server.start();
    if (!server_started) {
        log_error("chassis-agent", "Could not start JSON-RPC command server on port "
            << server_port << " restricted to " << registration_data.get_ipv4_address()
            << ". " << "Quitting now...");
        amc_connection.stop();
        event_dispatcher.stop();
        ipmb_service.stop();
        return -3;
    }

    wait_for_interrupt();
    log_info("chassis-agent", "Stopping SDV PSME Chassis Agent.");

    /* Cleanup */
    for (auto& bmc: bmcs) {
        bmc->stop();
    }
    ipmb_service.stop();
    server.stop();
    amc_connection.stop();
    event_dispatcher.stop();
    Configuration::cleanup();
    LoggerFactory::cleanup();

    return 0;
}

const json::Json& init_configuration(int argc, const char** argv) {
    log_info("agent",
        agent_framework::generic::Version::build_info());
    auto& basic_config = Configuration::get_instance();
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
        log_info("agent", "Loading configuration schema!");

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

BmcCollection load_bmcs(const json::Json&) {
    BmcCollection bmcs{};
    agent::chassis::Bmc::Duration state_update_interval = agent::chassis::ipmb::Gpio::get_instance()->get_minimal_update_interval();

    auto drawer_manager_keys = CommonComponents::get_instance()->get_module_manager().get_keys("");
    auto sled_manager_keys = CommonComponents::get_instance()->
            get_module_manager().get_keys(drawer_manager_keys.front());

    for (const auto& manager_key: sled_manager_keys) {
        const auto manager = agent_framework::module::get_manager<agent_framework::model::Manager>()
            .get_entry(manager_key);
        const auto manager_uuid = agent::chassis::ChassisTreeStabilizer().stabilize(manager_key);
        auto slot = manager.get_slot();
        auto read_presence_fn = [slot]() {
            log_debug("bmc", "reading presence on slot: " << slot);
            return agent::chassis::ipmb::Gpio::get_instance()->is_present(uint8_t(slot));
        };

        const auto& connection = manager.get_connection_data();
        ipmi::ManagementController mc{connection.get_ip_address(), connection.get_port(),
            connection.get_username(), connection.get_password()};
        auto read_online_state_fn = [mc]() mutable {
            try {
                ipmi::command::generic::response::GetDeviceId device_rsp{};
                mc.send(ipmi::command::generic::request::GetDeviceId{}, device_rsp);
                log_debug("bmc", "BMC " << mc.get_info() << " is available...");
                return true;
            }
            catch (std::exception& e) {
                log_debug("bmc", "BMC " << mc.get_info() << " reading state error: " << e.what());
            }
            return false;
        };
        bmcs.push_back(agent_framework::Bmc::Ptr(new agent::chassis::Bmc(manager_uuid, connection,
                state_update_interval, read_presence_fn, read_online_state_fn)));
    }

    if (bmcs.empty()) {
        throw std::runtime_error("Invalid modules configuration");
    }

    return bmcs;
}
