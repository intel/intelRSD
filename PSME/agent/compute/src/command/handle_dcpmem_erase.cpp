/*!
 * @brief Definition of function for handling Intel Optane DC Persistent Memory erase operation
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation
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
 * @file handle_dcpmem_erase.cpp
 */

#include "command/handle_dcpmem_erase.hpp"
#include "command/handle_system_power_state.hpp"

#include "agent-framework/module/requests/validation/compute.hpp"
#include "agent-framework/action/task_runner.hpp"
#include "agent-framework/action/task_creator.hpp"

#include "ipmi/manager/ipmitool/management_controller.hpp"
#include "ipmi/command/sdv/rsd/set_dcpmem_configuration.hpp"
#include "ipmi/command/sdv/rsd/get_dcpmem_configuration.hpp"


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


ipmi::command::sdv::EraseDcpmemAction get_ipmi_erase_dcpmem_action(const agent::compute::EraseDcpmemAttributes& erase_dcpmem_attributes) {
    auto erase_action = (std::uint8_t)ipmi::command::sdv::EraseDcpmemAction::PRESERVE_EXISTING;

    if (erase_dcpmem_attributes.reset_configuration) {
        erase_action |= (std::uint8_t)ipmi::command::sdv::EraseDcpmemAction::OVERWRITE_PCD;
    }
    if (erase_dcpmem_attributes.erase_configuration_keys) {
        erase_action |= (std::uint8_t)ipmi::command::sdv::EraseDcpmemAction::SECURE_ERASE;
    }

    return (ipmi::command::sdv::EraseDcpmemAction)erase_action;
}


void send_dcpmem_configuration(ManagementController& mc, const agent::compute::EraseDcpmemAttributes& attributes) {
    log_info("compute-agent", "Set Optane DC Persistent Memory configuration: "
                              << "Reset Configuration = " << attributes.reset_configuration
                              << " / Erase Configuration Keys = " << attributes.erase_configuration_keys);

    ipmi::command::sdv::rsd::request::SetDcpmemConfiguration request{};
    request.set_erase_dcpmem_action(get_ipmi_erase_dcpmem_action(attributes));
    request.set_command_status(ipmi::command::sdv::DcpmemConfigurationCommandStatus::VALID);

    try {
        ipmi::command::sdv::rsd::response::SetDcpmemConfiguration response{};
        mc.send(request, response);
    }
    catch (const ipmi::ResponseError& error) {
        THROW(IpmiError, "compute-agent",
              "Error sending - Set Optane DC Persistent Memory configuration: " + std::string(error.what())
              + " CC: " + std::to_string(unsigned(error.get_completion_code())));
    }
}


void secure_erase_task(const Uuid& system_uuid, const agent::compute::EraseDcpmemAttributes& attributes_to_set) {
    auto system = get_manager<System>().get_entry(system_uuid);

    ManagementController mc{};
    set_connection_data(mc, system);

    /*!
     *  Erase Intel Optane DC Persistent Memory operation bitmap:
     * - SECURE_ERASE / OVERWRITE_PCD - restart is required to trigger the operation by the BIOS
     * - PRESERVE_EXISTING - restart is not be required, sends the IPMI command immediately
     * */
    if (get_ipmi_erase_dcpmem_action(attributes_to_set) != ipmi::command::sdv::EraseDcpmemAction::PRESERVE_EXISTING) {
        log_debug("compute-agent", "Execute DCPMEM Secure Erase operation on next boot (power cycle)");
        agent::compute::handle_system_power_state(
            ipmi::command::generic::request::ChassisControlCommand::PowerState::LAST,
            agent::compute::PowerStateCycle::CYCLE, "Request POST", system.get_uuid(),
            [&mc, &attributes_to_set] { send_dcpmem_configuration(mc, attributes_to_set); },
            // post_action - do not wait for the operation complete confirmation
            [] {}
        );
    }
    else {
        log_debug("compute-agent", "Execute DCPMEM Secure Erase operation - preserve existing configuration");
        send_dcpmem_configuration(mc, attributes_to_set);
    }
}


void unlock_dcpmem(const Uuid& system_uuid) {
    // Critical section on System: clear task uuid in System model
    auto system_ref = get_manager<System>().get_entry_reference(system_uuid);
    system_ref->set_dcpmem_erase_task({});
}

}


Uuid agent::compute::process_dcpmem_erase(const Uuid& system_uuid, const EraseDcpmemAttributes& erase_dcpmem_attributes) {
    // Persistent Memory Secure Erase is asynchronous operation - requires creating a task
    agent_framework::action::TaskCreator task_creator;
    task_creator.prepare_task();

    {
        // Critical section on System: set task uuid in System model
        auto system_ref = get_manager<System>().get_entry_reference(system_uuid);
        if (system_ref->get_dcpmem_erase_task().has_value()) {
            THROW(MethodConflict, "compute-agent", "There already is a running task applying DCPMEM configuration.",
                  system_ref->get_dcpmem_erase_task());
        }
        else {
            system_ref->set_dcpmem_erase_task(task_creator.get_task_resource().get_uuid());
        }
    }

    task_creator.add_subtask([system_uuid, erase_dcpmem_attributes] { secure_erase_task(system_uuid, erase_dcpmem_attributes); });

    task_creator.set_promised_response([system_uuid] {
        log_info("compute-agent", "DCPMEM settings applied, task is done.");
        unlock_dcpmem(system_uuid);
        return json::Json{};
    });
    task_creator.set_promised_error_thrower([system_uuid] (const GamiException& exception) {
        unlock_dcpmem(system_uuid);
        return GamiException(ErrorCode::COMPUTE, "Failed to apply changes to DCPMEM: " + exception.get_message());
    });

    task_creator.register_task();
    auto task = task_creator.get_task();
    // Add a logging exception handler to the task
    task.add_exception_handler([](const agent_framework::exceptions::GamiException& e) {
        log_error("compute-agent", "Failed to apply changes to DCPMEM: " << e.what());
    });

    agent_framework::action::TaskRunner::get_instance().run(std::move(task));
    return task_creator.get_task_resource().get_uuid();
}
