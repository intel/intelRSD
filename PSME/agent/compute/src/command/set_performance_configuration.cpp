/*!
 * @brief Definition of function processing Set Component Attributes command on performance configuration
 *
 * @copyright Copyright (c) 2019 Intel Corporation
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
 * @file set_performance_configuration.cpp
 */

#include "command/handle_system_power_state.hpp"
#include "command/set_performance_configuration.hpp"

#include "agent-framework/module/requests/validation/compute.hpp"
#include "agent-framework/action/task_runner.hpp"
#include "agent-framework/action/task_creator.hpp"

#include "ipmi/manager/ipmitool/management_controller.hpp"
#include "ipmi/command/sdv/rsd/set_speed_select_configuration.hpp"

using namespace agent_framework;
using namespace agent_framework::model;
using namespace agent_framework::model::requests::validation;
using namespace agent_framework::module;
using namespace agent_framework::command;
using namespace agent_framework::exceptions;
using namespace ipmi::manager::ipmitool;


namespace {

void set_connection_data(ManagementController& mc, const System& system) {
    auto& mm = get_manager<Manager>();
    const auto& manager = mm.get_entry(system.get_parent_uuid());
    const auto& connection_data = manager.get_connection_data();

    mc.set_ip(connection_data.get_ip_address());
    mc.set_port(connection_data.get_port());
    mc.set_username(connection_data.get_username());
    mc.set_password(connection_data.get_password());
}

void send_performance_configuration(ManagementController& mc,
                                    const agent::compute::PerformanceConfigurationAttributes& attributes) {
    log_info("compute-agent", "Set Speed Select configuration: "
            << "Node index = " << unsigned(attributes.node_index)
            << " CPU index = " << unsigned(attributes.cpu_index)
            << " Configuration index = " << unsigned(attributes.configuration_index));

    ipmi::command::sdv::rsd::request::SetSpeedSelectConfiguration request{};
    request.set_node_index(attributes.node_index);
    request.set_cpu_index(attributes.cpu_index);
    request.set_configuration_index(attributes.configuration_index);

    try {
        ipmi::command::sdv::rsd::response::SetSpeedSelectConfiguration response{};
        mc.send(request, response);
    }
    catch (const ipmi::ResponseError& error) {
        THROW(IpmiError, "compute-agent",
              "Error sending - Set Speed Select configuration: " + std::string(error.what())
              + " CC: " + std::to_string(unsigned(error.get_completion_code())));
    }
}

void set_performance_configuration_task(const Uuid &system_uuid,
                                        const agent::compute::PerformanceConfigurationAttributes& attributes) {
    auto system = get_manager<System>().get_entry(system_uuid);
    ManagementController mc{};
    set_connection_data(mc, system);

    agent::compute::handle_system_power_state(
            ipmi::command::generic::request::ChassisControlCommand::PowerState::POWER_CYCLE,
            agent::compute::PowerStateCycle::CYCLE, "Power cycle", system.get_uuid(),
            [&mc, &attributes] { send_performance_configuration(mc, attributes); }
    );
}

void lock_performance_configuration(const Uuid& system_uuid) {
    auto system_ref = get_manager<System>().get_entry_reference(system_uuid);
    system_ref->set_is_performance_configuration_task_running(true);
}

void unlock_and_update_configuration(const Uuid& system_uuid,
                                      const agent::compute::PerformanceConfigurationAttributes& attributes) {
    auto system_ref = get_manager<System>().get_entry_reference(system_uuid);
    system_ref->set_current_performance_configuration(attributes.configuration_index);
    system_ref->set_is_performance_configuration_task_running(false);
}

}

Uuid agent::compute::process_performance_configuration(const Uuid& system_uuid,
                                                       const agent::compute::PerformanceConfigurationAttributes& attributes) {
    agent_framework::action::TaskCreator task_creator;
    action::Task::SubtaskType configuration_task;
    task_creator.prepare_task();

    auto system_ref = get_manager<System>().get_entry_reference(system_uuid);

    if (system_ref->is_performance_configuration_task_running()) {
        THROW(MethodNotAllowed, "compute-agent", "There already is a running task applying performance configuration.");
    }

    configuration_task = [system_uuid, attributes] { set_performance_configuration_task(system_uuid, attributes); };

    if (system_ref->get_current_performance_configuration().value() != attributes.configuration_index) {
        task_creator.add_subtask([system_uuid]() { lock_performance_configuration(system_uuid); });
        task_creator.add_subtask([system_uuid, attributes] { set_performance_configuration_task(system_uuid, attributes); });
        task_creator.add_subtask([system_uuid, attributes]() { unlock_and_update_configuration(system_uuid, attributes); });
    }
    task_creator.set_promised_response([system_uuid] {
        return SetComponentAttributes::Response().to_json();
    });
    task_creator.set_promised_error_thrower([system_uuid] (const GamiException& exception) {
        return GamiException(ErrorCode::COMPUTE, "Failed to set Speed Select configuration: " + exception.get_message());
    });

    task_creator.register_task();
    auto task = task_creator.get_task();
    // Add a logging exception handler to the task
    task.add_exception_handler([](const agent_framework::exceptions::GamiException& e) {
        log_error("compute-agent", "Failed to set Speed Select configuration: " << e.what());
    });

    agent_framework::action::TaskRunner::get_instance().run(std::move(task));
    return task_creator.get_task_resource().get_uuid();
}
