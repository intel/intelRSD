/*!
 * @brief Definition of function processing Set Component Attributes command on Trusted Module
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
 * @file set_trusted_module_attributes.cpp
 */

#include "command/set_trusted_module_attributes.hpp"
#include "command/handle_system_power_state.hpp"

#include "agent-framework/command/compute_commands.hpp"
#include "agent-framework/module/requests/validation/compute.hpp"
#include "agent-framework/module/compute_components.hpp"
#include "agent-framework/action/task_runner.hpp"
#include "agent-framework/action/task_creator.hpp"

#include "ipmi/manager/ipmitool/management_controller.hpp"
#include "ipmi/command/sdv/rsd/set_tpm_configuration.hpp"
#include "ipmi/command/sdv/rsd/get_tpm_configuration.hpp"
#include "ipmi/command/sdv/rsd/constants.hpp"


using namespace agent_framework::model;
using namespace agent_framework::model::requests::validation;
using namespace agent_framework::module;
using namespace agent_framework::command;
using namespace agent_framework::exceptions;
using namespace ipmi::manager::ipmitool;


namespace {

void set_connection_data(ManagementController& mc, const TrustedModule& tpm) {
    const auto manager_uuid = get_manager<System>().get_entry(tpm.get_parent_uuid()).get_parent_uuid();
    auto connection_data = get_manager<Manager>().get_entry(manager_uuid).get_connection_data();

    mc.set_ip(connection_data.get_ip_address());
    mc.set_port(connection_data.get_port());
    mc.set_username(connection_data.get_username());
    mc.set_password(connection_data.get_password());
}


struct ConfigurableTpmAttributes {
    std::uint8_t version_selection{};
    ipmi::command::sdv::ClearTpmAction clear_tpm{};
};


ipmi::command::sdv::ClearTpmAction get_clear_tpm(bool clear) {
    if (clear) {
        return ipmi::command::sdv::ClearTpmAction::CLEAR_USER_TPM_OWNERSHIP;
    }
    else {
        return ipmi::command::sdv::ClearTpmAction::PRESERVE_USER_TPM_OWNERSHIP;
    }
}


/*
 * PSME REST server sends GAMI request using TPM's UUID and boolean value determining whether this specific TPM
 * is supposed to be enabled or disabled. However, BMC identifies the TPM using its unique configuration index,
 * which is 1-based value for each TPM. If the TPM is supposed to be disabled, the device index should be set to 0.
 * If the TPM is supposed to be enabled, the device index should be set to its configuration index. If requested
 * TPM state is null (no change), the configuration index is set to actual TPM state.
 * */
std::uint8_t get_version_selection(const TrustedModule& tpm, bool enabled) {

    if (enabled) {
        auto index = tpm.get_configuration_index();
        if (index == ipmi::command::sdv::constants::DISABLE_TPM) {
            log_error("compute-agent", "TPM enable request is received, but no version is configured!");
        }
        return index;
    }
    else {
        return ipmi::command::sdv::constants::DISABLE_TPM;
    }
}

void send_tpm_configuration(ManagementController& mc, const ConfigurableTpmAttributes& attributes) {
    log_info("compute-agent", "Sending TPM configuration "
        << (attributes.version_selection == ipmi::command::sdv::constants::DISABLE_TPM
            ? "disabled"
            : "enabled " + std::to_string(unsigned(attributes.version_selection)))
        << "/"
        << (attributes.clear_tpm == ipmi::command::sdv::ClearTpmAction::CLEAR_USER_TPM_OWNERSHIP
            ? "clear"
            : "preserve") << " ownership");

    ipmi::command::sdv::rsd::request::SetTpmConfiguration request{};
    request.set_clear_tpm_action(attributes.clear_tpm);
    request.set_tpm_version_selection(attributes.version_selection);
    request.set_tpm_command_status(ipmi::command::sdv::TpmCommandStatus::COMMAND_VALID);
    try {
        ipmi::command::sdv::rsd::response::SetTpmConfiguration response{};
        mc.send(request, response);
    }
    catch (const ipmi::ResponseError& error) {
        THROW(IpmiError, "compute-agent",
              "Error Sending Set TPM configuration: " + std::string(error.what())
              + " CC: " + std::to_string(unsigned(error.get_completion_code())));
    }
}


void tpm_task(const Uuid& tpm_uuid, bool clear, bool enable) {
    auto tpm = get_manager<TrustedModule>().get_entry(tpm_uuid);

    ManagementController mc{};
    set_connection_data(mc, tpm);

    ConfigurableTpmAttributes attributes_to_set{};
    attributes_to_set.version_selection = get_version_selection(tpm, enable);
    attributes_to_set.clear_tpm = get_clear_tpm(clear);

    agent::compute::handle_system_power_state(
        ipmi::command::generic::request::ChassisControlCommand::PowerState::LAST,
        agent::compute::PowerStateCycle::CYCLE, "Request POST", tpm.get_parent_uuid(),
        /* command requires the system locked  */
        [&mc, &attributes_to_set] { send_tpm_configuration(mc, attributes_to_set); },
        /* TODO WORKAROUND: do not wait for TPM confirmation */
        [] {}
    );
}


void unlock_tpm(const Uuid& tpm_uuid) {
    /* critical section on TPM: clear task uuid in TPM model */
    auto tpm_ref = get_manager<TrustedModule>().get_entry_reference(tpm_uuid);
    tpm_ref->set_task({});
}

}


void agent::compute::process_trusted_module(const Uuid& tpm_uuid, const attribute::Attributes& attributes,
                            SetComponentAttributes::Response& response) {

    ComputeValidator::validate_set_trusted_module_attributes(attributes);
    const auto attribute_names = attributes.get_names();
    if (attribute_names.empty()) {
        log_debug("compute-agent", "setComponentAttributes: nothing has been changed (empty request).");
        return;
    }

    bool clear{};
    bool enable{};
    for (const auto& name : attribute_names) {
        const auto& value = attributes.get_value(name);
        try {
            if (literals::TrustedModule::DEVICE_ENABLED == name) {
                if (value.is_null()) {
                    auto tpm = get_manager<TrustedModule>().get_entry(tpm_uuid);
                    enable = (tpm.get_status().get_state() == enums::State::Enabled);
                }
                else {
                    enable = value.get<bool>();
                }
            }
            else if (literals::TrustedModule::CLEAR_OWNERSHIP == name) {
                clear = value.get<bool>();
            }
            else {
                // The response must have a message for every attribute that could not be set.
                THROW(UnsupportedField, "compute-agent", "Setting attribute is not supported.", name, value);
            }
        }
        catch (const GamiException& ex) {
            response.add_status({name, ex.get_error_code(), ex.get_message()});
        }
    }
    if (!response.get_statuses().empty()) {
        return;
    }

    // We know that setting anything on a TPM requires creating a task
    agent_framework::action::TaskCreator task_creator;
    task_creator.prepare_task();

    {
        /* critical section on TPM */
        auto tpm_ref = get_manager<TrustedModule>().get_entry_reference(tpm_uuid);
        if (tpm_ref->get_task().has_value()) {
            THROW(MethodConflict, "compute-agent", "There already is a running task applying TPM configuration",
                tpm_ref->get_task());
        }
        else {
            tpm_ref->set_task(task_creator.get_task_resource().get_uuid());
        }
    }

    if (clear && !enable) {
        /* Clear and disable is a special case: first clear and enable, than preserve and disable */
        task_creator.add_subtask([tpm_uuid]() { tpm_task(tpm_uuid, true, true); });
        /* TODO WORKAROUND: The TPM will not be disabled due to some issues */
        //task_creator.add_subtask([tpm_uuid]() { tpm_task(tpm_uuid, false, false); });
    } else {
        task_creator.add_subtask([tpm_uuid, clear, enable]() { tpm_task(tpm_uuid, clear, enable); });
    }

    task_creator.set_promised_response([tpm_uuid] {
        log_info("compute-agent", "TPM settings applied, task is done.");
        unlock_tpm(tpm_uuid);
        return json::Json{};
    });
    task_creator.set_promised_error_thrower([tpm_uuid] (const GamiException& exception) {
        unlock_tpm(tpm_uuid);
        return GamiException(ErrorCode::COMPUTE, "Failed to apply changes to TPM." + exception.get_message());
    });

    task_creator.register_task();
    auto task = task_creator.get_task();
    // add a logging exception handler to the task
    task.add_exception_handler([](const agent_framework::exceptions::GamiException& e) {
        log_error("compute-agent", "Failed to apply changes to TPM: " << e.what());
    });

    agent_framework::action::TaskRunner::get_instance().run(std::move(task));
    response.set_task(task_creator.get_task_resource().get_uuid());
}
