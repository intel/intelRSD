/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * */

#include "app.hpp"
#include "psme/core/agent/agent_manager.hpp"
#include "agent-framework/eventing/events_queue.hpp"
#include "psme/rest-ref/eventing/config/subscription_config.hpp"
#include "psme/rest-ref/eventing/event_service.hpp"
#include "signal.hpp"
#include "default_configuration.hpp"
#include "configuration/configuration.hpp"
#include "configuration/configuration_validator.hpp"
#include "agent-framework/logger_ext.hpp"
#include "agent-framework/version.hpp"
#include "service_uuid.hpp"

#include <jsonrpccpp/server/connectors/httpserver.h>
#include <iostream>
#include <string>

namespace {
const json::Value& load_configuration(int argc, const char** argv) {
    /* Initialize configuration */
    log_info(GET_LOGGER("app"), agent_framework::generic::Version::build_info());
    auto& basic_config = configuration::Configuration::get_instance();
    basic_config.set_default_configuration(psme::app::DEFAULT_CONFIGURATION);
    basic_config.set_default_file(psme::app::DEFAULT_FILE);
    basic_config.set_default_env_file(psme::app::DEFAULT_ENV_FILE);

    while (argc > 1) {
        basic_config.add_file(argv[argc - 1]);
        --argc;
    }
    const json::Value& configuration = basic_config.to_json();

    json::Value json_schema;
    if (configuration::string_to_json(psme::app::DEFAULT_VALIDATOR_JSON,
                json_schema)) {
        log_info(GET_LOGGER("app"), "JSON Schema load!");
        configuration::SchemaErrors errors;
        configuration::SchemaValidator validator;
        configuration::SchemaReader reader;
        reader.load_schema(json_schema, validator);
        validator.validate(configuration, errors);

        if (!errors.is_valid()) {
            std::cerr << "Configuration invalid: "
                      << errors.to_string() << std::endl;
            exit(-1);
        }
    }
    return configuration;
}
}

using namespace psme::app;

App::App(int argc, const char* argv[])
    try : m_configuration(load_configuration(argc, argv)) {
        init();
    }
    catch(std::exception& e) {
        std::cerr << "\nFailed to initialize Application: " << e.what() << "\n";
        exit(-1);
    }

App::~App() { cleanup(); }

void App::init_logger() {
    logger_cpp::LoggerLoader loader(m_configuration);
    logger_cpp::LoggerFactory::instance().set_loggers(loader.load());
    logger_cpp::LoggerFactory::set_main_logger_name("app");
}

void App::init_eventing_server() {
    using psme::app::eventing::EventingServer;
    m_eventing_server.reset(new EventingServer{m_configuration});
    psme::command::CommandJson::Map::set_implementation("Eventing");
    m_eventing_server->add(psme::command::CommandJson::Map::get_instance());
}

void App::init_registration_server() {
    using jsonrpc::HttpServer;
    using psme::app::registration::RegistrationServer;
    m_reg_server_connector.reset(new HttpServer{
                        m_configuration["registration"]["port"].as_int()});
    m_registration_server.reset(new RegistrationServer{*m_reg_server_connector});
    psme::command::CommandJson::Map::set_implementation(
                        m_configuration["commands"]["generic"].as_string());
    m_registration_server->add(psme::command::CommandJson::Map::get_instance());
}

void App::init_rest_server() {
    using psme::rest::server::RestServer;
    m_rest_server.reset(new RestServer);
}

void App::init_rest_event_service() {
    using psme::rest::eventing::EventService;
    m_rest_event_service.reset(new EventService());
}

void App::init_subscription_config() {
    using rest::eventing::config::SubscriptionConfig;
    SubscriptionConfig::get_instance()->set_config_file(
            m_configuration["subscription-config-file"].as_string());
    SubscriptionConfig::get_instance()->load();
}

void App::init() {
    try {
        init_logger();
        ServiceUuid::get_instance();
        init_eventing_server();
        init_rest_event_service();
        init_registration_server();
        init_subscription_config();
        init_rest_server();
    }
    catch (std::exception& e) {
        log_error(GET_LOGGER("app"),
                "Failed to initialize Application: " << e.what());
        cleanup();
        exit(-1);
    }
    catch (...) {
        log_error(GET_LOGGER("app"),
                "Failed to initialize Application: Unknown exception.");
        cleanup();
        exit(-1);
    }
}

void App::run() {
    try {
        m_eventing_server->start();
        m_rest_event_service->start();
        m_registration_server->start();
        m_rest_server->start();

        psme::app::wait_for_interrupt();
        log_info(LOGUSR, "Stopping PSME Application...");
    }
    catch (std::exception& e) {
        log_error(GET_LOGGER("app"), e.what());
    }
    catch (...) {
        log_error(GET_LOGGER("app"), "Unknown exception.");
    }
}

void App::statics_cleanup() {
    psme::command::Command::Map::cleanup();
    psme::command::CommandJson::Map::cleanup();
    configuration::Configuration::cleanup();
    logger_cpp::LoggerFactory::cleanup();
}

void App::cleanup() {
    if (m_registration_server) {
        m_registration_server->stop();
        m_registration_server.reset();
    }
    if (m_eventing_server) {
        m_eventing_server->stop();
        m_eventing_server.reset();
    }
    if (m_rest_event_service) {
        m_rest_event_service->stop();
        m_rest_event_service.reset();
    }
    if (m_rest_server) {
        m_rest_server->stop();
        m_rest_server.reset();
    }
    statics_cleanup();
}
