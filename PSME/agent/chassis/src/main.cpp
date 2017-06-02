/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
#include "agent-framework/state_machine/state_machine_thread.hpp"
#include "agent-framework/module/compute_components.hpp"
#include "agent-framework/module/common_components.hpp"

#include "agent-framework/eventing/event_data.hpp"
#include "agent-framework/eventing/events_queue.hpp"

#include "agent-framework/command-ref/command_server.hpp"

#include "loader/chassis_loader.hpp"
#include "loader/state_machine_action_loader.hpp"
#include "status/status_manager.hpp"
#include "status/state_machine_action.hpp"

#include "configuration/configuration.hpp"
#include "configuration/configuration_validator.hpp"
#include "default_configuration.hpp"

#include "ipmb/service.hpp"
#include "ipmb/watcher/thermal_sensor_task.hpp"
#include "ipmb/watcher/drawer_power_task.hpp"
#include "ipmb/mux.hpp"
#include "ipmb/watcher/watcher.hpp"

#include <jsonrpccpp/server/connectors/httpserver.h>
#include <csignal>

using namespace std;
using namespace agent_framework;
using namespace agent_framework::generic;
using namespace agent_framework::state_machine;
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

void add_state_machine_entries(StateMachineThread* machine_thread, StateMachineThreadAction& action);
const json::Value& init_configuration(int argc, const char** argv);
bool check_configuration(const json::Value& json);

/*!
 * @brief Generic Agent main method.
 * */
int main(int argc, const char* argv[]) {
    using agent::chassis::StateMachineAction;
    using StateMachineActionUnique = unique_ptr<StateMachineAction>;

    unsigned int server_port = DEFAULT_SERVER_PORT;

    StateMachineThreadUniquePtr state_machine_thread_u_ptr = nullptr;
    StateMachineActionUnique state_action = nullptr;

    /* Initialize configuration */
    const json::Value& configuration = ::init_configuration(argc, argv);
    if (!::check_configuration(configuration)) {
        return -1;
    }

    /* Initialize logger */
    LoggerLoader loader(configuration);
    LoggerFactory::instance().set_loggers(loader.load());
    LoggerFactory::set_main_logger_name("agent");
    log_info(GET_LOGGER("agent"), "Running PSME Chassis SDV...\n");

    ::agent::chassis::loader::ChassisLoader module_loader{};
    if (!module_loader.load(configuration)) {
        std::cerr << "Invalid modules configuration" << std::endl;
        return -2;
    }

    EventDispatcher event_dispatcher;
    event_dispatcher.start();

    AmcConnectionManager amc_connection(event_dispatcher);
    amc_connection.start();

    /* Initialize command server */
    jsonrpc::HttpServer http_server((int(server_port)));
    agent_framework::command_ref::CommandServer server(http_server);
    server.add(command_ref::Registry::get_instance()->get_commands());
    server.start();

    watcher::Watcher ipmb_watcher{};
    ipmb_watcher.add_task(std::make_shared<watcher::ThermalSensorTask>());
    ipmb_watcher.add_task(std::make_shared<watcher::DrawerPowerTask>());
    ipmb_watcher.start();

    ipmb::Service ipmb_service;
    ipmb_service.start();

    /* Start state machine */
    try {
        ::agent::chassis::loader::StateMachineActionLoader state_machine_loader{};
        if (!state_machine_loader.load(configuration)) {
            std::cerr << "Invalid IPMI and/or Discovery configuration" << std::endl;
            return -3;
        }
        state_action = state_machine_loader.get_state_machine();
        state_machine_thread_u_ptr.reset(new StateMachineThread());
        ::add_state_machine_entries(state_machine_thread_u_ptr.get(), *state_action);
        state_machine_thread_u_ptr->start();
    }
    catch (const std::exception& e) {
        log_error(GET_LOGGER("agent"), "State Machine error.");
        log_debug(GET_LOGGER("agent"), e.what());
        return -10;
    }

    ::agent::chassis::loader::ChassisLoader::wait_for_complete();

    auto cc = CommonComponents::get_instance();
    auto managers_uuids_vec = cc->get_module_manager().get_keys("");

    for (const auto& manager_uuid: managers_uuids_vec) {
        EventData event_data{};
        event_data.set_component(manager_uuid);
        event_data.set_type(agent_framework::model::enums::Component::Manager);
        event_data.set_notification(eventing::Notification::Add);
        EventsQueue::get_instance()->push_back(event_data);
    }

    /* Stop the program and wait for interrupt */
    wait_for_interrupt();

    /* Cleanup */
    ipmb_watcher.stop();
    ipmb_service.stop();

    server.stop();
    amc_connection.stop();
    event_dispatcher.stop();
    Configuration::cleanup();
    LoggerFactory::cleanup();

    return 0;

}

void add_state_machine_entries(StateMachineThread* machine_thread, StateMachineThreadAction& action) {
    auto drawer_keys = CommonComponents::get_instance()->
            get_module_manager().get_keys("");

    if (drawer_keys.empty()) {
        log_error(GET_LOGGER("agent"), "No drawer manager found!");
        std::exit(-1);
    }

    log_debug(GET_LOGGER("agent"), "Drawer: " << drawer_keys.front());

    /* Process Blade manager */
    auto blade_keys = CommonComponents::get_instance()->
            get_module_manager().get_keys(drawer_keys.front());

    for (const auto& key : blade_keys) {
        auto entry = CommonComponents::get_instance()->get_module_manager().get_entry(key);
        auto status_manager = std::make_shared<::agent::chassis::status::StatusManager>(
                entry.get_slot(),entry.get_connection_data().get_ip_address());
        auto state_thread_entry = std::make_shared<StateThreadEntry>(key, status_manager);
        auto module_thread = std::make_shared<StateMachineModuleThread>(state_thread_entry, action);
        machine_thread->add_module_thread(module_thread);
    }
}
const json::Value& init_configuration(int argc, const char** argv) {
    log_info(GET_LOGGER("agent"),
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
        log_info(GET_LOGGER("agent"), "Loading configuration schema!");

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
