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
 *
 * */

#include "app.hpp"
#include "psme/core/agent/agent_manager.hpp"
#include "psme/rest/eventing/manager/subscription_manager.hpp"
#include "psme/rest/eventing/event_service.hpp"
#include "psme/rest/security/session/session_service.hpp"
#include "psme/rest/registries/config/registry_configurator.hpp"
#include "psme/rest/registries/config/base_configuration.hpp"
#include "psme/rest/registries/config/intel_rackscale_configuration.hpp"
#include "psme/ssdp/ssdp_config_loader.hpp"
#include "default_configuration.hpp"
#include "configuration/configuration.hpp"
#include "configuration/configuration_validator.hpp"
#include "agent-framework/version.hpp"
#include "agent-framework/module/service_uuid.hpp"
#include "agent-framework/logger_loader.hpp"
#include "agent-framework/eventing/events_queue.hpp"
#include "ssdp/ssdp_service.hpp"
#include "database/database.hpp"

#include <csignal>
#include <string>



namespace {
const json::Json& load_configuration(int argc, const char** argv) {
    /* Initialize configuration */
    log_info("app", agent_framework::generic::Version::build_info());
    auto& basic_config = configuration::Configuration::get_instance();
    basic_config.set_default_configuration(psme::app::DEFAULT_CONFIGURATION);
    basic_config.set_default_file(psme::app::DEFAULT_FILE);
    basic_config.set_default_env_file(psme::app::DEFAULT_ENV_FILE);

    while (argc > 1) {
        basic_config.add_file(argv[argc - 1]);
        --argc;
    }
    const json::Json& configuration = basic_config.to_json();

    json::Json json_schema = json::Json();
    if (configuration::string_to_json(psme::app::DEFAULT_VALIDATOR_JSON, json_schema)) {
        log_info("app", "JSON Schema load!");
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

void App::init_database() {
    if (m_configuration.value("database", json::Json::object()).value("location", json::Json()).is_string()) {
        database::Database::set_default_location(m_configuration["database"]["location"].get<std::string>());
    }
}

void App::init_logger() {
    logger_cpp::LoggerLoader loader(m_configuration);
    loader.load(logger_cpp::LoggerFactory::instance());
}

void App::init_network_change_notifier() {
    try {
        m_network_change_notifier = net::NetworkChangeNotifier::create();
    } catch (const std::exception&) {
        log_error("app", "Failed to initialize NetworkChangeNotifier.");
    }
}

void App::init_ssdp_service() {
    const auto& service_uuid = agent_framework::module::ServiceUuid::get_instance()->get_service_uuid();
    m_ssdp_service.reset(new ssdp::SsdpService{ssdp::load_ssdp_config(m_configuration, service_uuid)});
    if (m_network_change_notifier) {
        m_network_change_notifier->add_listener(m_ssdp_service);
    }
}

void App::init_eventing_server() {
    using psme::app::eventing::EventingServer;
    m_eventing_server.reset(new EventingServer{m_configuration});
}

void App::init_registration_server() {
    using psme::app::registration::RegistrationServer;
    m_registration_server.reset(new RegistrationServer{m_configuration});
}

void App::init_rest_server() {
    using psme::rest::server::RestServer;
    m_rest_server.reset(new RestServer);
}

void App::init_rest_event_service() {
    using psme::rest::eventing::EventService;
    m_rest_event_service.reset(new EventService());
}

void App::init_rest_session_service() {
    using psme::rest::security::session::SessionService;
    m_rest_session_service.reset(new SessionService());
}

void App::init_registries() {
    using namespace rest::registries;
    const std::string& base_configuration{make_base_configuration()};
    const std::string& intel_rack_scale_configuration{make_intel_rackscale_configuration()};
    RegistryConfigurator::get_instance()->load(base_configuration);
    RegistryConfigurator::get_instance()->load(intel_rack_scale_configuration);
}

void App::init() {
    try {
        init_database();
        init_logger();
        agent_framework::module::ServiceUuid::get_instance();
        init_network_change_notifier();
        init_ssdp_service();
        init_eventing_server();
        init_rest_event_service();
        init_rest_session_service();
        init_registration_server();
        psme::rest::eventing::manager::SubscriptionManager::get_instance();
        init_registries();
        init_rest_server();
        m_model_watcher.reset(new rest::model::Watcher());
    }
    catch (std::exception& e) {
        log_error("app",
                "Failed to initialize Application: " << e.what());
        cleanup();
        exit(-1);
    }
    catch (...) {
        log_error("app",
                "Failed to initialize Application: Unknown exception.");
        cleanup();
        exit(-1);
    }
}

void App::run() {
    try {
        if (m_network_change_notifier) {
            m_network_change_notifier->start();
        }
        m_eventing_server->start();
        m_rest_event_service->start();
        m_rest_session_service->start();
        m_registration_server->start();
        m_rest_server->start();
        m_model_watcher->start();
        m_ssdp_service->start();

        wait_for_termination();
        log_info(LOGUSR, "Stopping PSME Application...");
    }
    catch (std::exception& e) {
        log_error("app", e.what());
    }
    catch (...) {
        log_error("app", "Unknown exception.");
    }
}

void App::wait_for_termination() {
    log_debug(LOGUSR, "Waiting for termination signal...");

    sigset_t sset;
    sigemptyset(&sset);
    sigaddset(&sset, SIGINT);
    sigaddset(&sset, SIGQUIT);
    sigaddset(&sset, SIGTERM);
    sigprocmask(SIG_BLOCK, &sset, NULL);
    int sig;
    sigwait(&sset, &sig);
}

void App::statics_cleanup() {
    psme::rest::eventing::manager::SubscriptionManager::get_instance()->clean_up();
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
    if (m_rest_session_service) {
        m_rest_session_service->stop();
        m_rest_session_service.reset();
    }
    if (m_rest_server) {
        m_rest_server->stop();
        m_rest_server.reset();
    }
    if (m_ssdp_service) {
        m_ssdp_service->stop();
        m_ssdp_service.reset();
    }
    if (m_model_watcher) {
        m_model_watcher->stop();
        m_model_watcher.reset();
    }
    if (m_network_change_notifier) {
        m_network_change_notifier->stop();
        m_network_change_notifier.reset();
    }
    statics_cleanup();
}
