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
 *
 * @brief Implementation of SetComponentAttributes command
 * */

#include "agent-framework/command/compute_commands.hpp"
#include "agent-framework/command/registry.hpp"
#include "agent-framework/module/compute_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/requests/validation/compute.hpp"
#include "agent-framework/module/requests/validation/common.hpp"
#include "agent-framework/action/task_runner.hpp"
#include "agent-framework/action/task.hpp"
#include "agent-framework/action/task_creator.hpp"

#include "ipmi/manager/ipmitool/management_controller.hpp"
#include "ipmi/command/generic/chassis_control_command.hpp"
#include "ipmi/command/generic/set_system_boot_options.hpp"
#include "ipmi/command/sdv/rsd/constants.hpp"
#include "ipmi/command/sdv/rsd/set_system_mode.hpp"
#include "ipmi/command/sdv/rsd/set_tpm_configuration.hpp"
#include "ipmi/command/sdv/rsd/get_tpm_configuration.hpp"

#include "command/set_network_device_function_attributes.hpp"


using namespace agent_framework;
using namespace agent_framework::command;
using namespace agent_framework::exceptions;
using namespace agent_framework::model;
using namespace agent_framework::model::requests::validation;

using namespace ipmi::manager::ipmitool;
using namespace ipmi::command::generic;

namespace {

using agent_framework::module::ComputeComponents;
using agent_framework::module::CommonComponents;
using PowerState = ipmi::command::generic::request::ChassisControlCommand::PowerState;


struct ConfigurableSystemAttributes {
    OptionalField<enums::BootOverride> BootOverride{};
    OptionalField<enums::BootOverrideTarget> BootOverrideTarget{};
    OptionalField<enums::BootOverrideMode> BootMode{};
    OptionalField<enums::ResetType> PowerState{};
    OptionalField<std::string> AssetTag{};
    OptionalField<bool> UserModeEnabled{};
    OptionalField<attribute::Oem> Oem{};
};

struct ConfigurableTpmAttributes {
    ipmi::command::sdv::TpmCommandStatus command_status{};
    std::uint8_t version_selection{};
    ipmi::command::sdv::ClearTpmAction clear_tpm{};
};


BootOverrideTarget convert_boot_override_target_to_ipmi_enum(enums::BootOverrideTarget target) {
    switch (target) {
        case enums::BootOverrideTarget::Hdd:
        case enums::BootOverrideTarget::RemoteDrive:
            return BootOverrideTarget::Hdd;
        case enums::BootOverrideTarget::Pxe:
            return BootOverrideTarget::Pxe;
        case enums::BootOverrideTarget::None:
        case enums::BootOverrideTarget::Floppy:
        case enums::BootOverrideTarget::Cd:
        case enums::BootOverrideTarget::Usb:
        case enums::BootOverrideTarget::BiosSetup:
        case enums::BootOverrideTarget::Utilities:
        case enums::BootOverrideTarget::Diags:
        case enums::BootOverrideTarget::UefiTarget:
        case enums::BootOverrideTarget::SDCard:
        case enums::BootOverrideTarget::UefiHttp:
        default:
            return BootOverrideTarget::None;
    }
}


BootOverride convert_boot_override_to_ipmi_enum(enums::BootOverride boot_override) {
    switch (boot_override) {
        case enums::BootOverride::Once:
            return BootOverride::Once;
        case enums::BootOverride::Continuous:
            return BootOverride::Continuous;
        case enums::BootOverride::Disabled:
        default:
            return BootOverride::Disabled;
    }
}


BootMode convert_boot_mode_to_ipmi_enum(enums::BootOverrideMode mode) {
    switch (mode) {
        case enums::BootOverrideMode::UEFI:
            return BootMode::UEFI;
        case enums::BootOverrideMode::Legacy:
        default:
            return BootMode::Legacy;
    }
}

ipmi::command::sdv::ClearTpmAction get_clear_tpm(bool value) {
    if (value == true) {
        return ipmi::command::sdv::ClearTpmAction::CLEAR_USER_TPM_OWNERSHIP;
    }
    return ipmi::command::sdv::ClearTpmAction::PRESERVE_USER_TPM_OWNERSHIP;
}

/*
 * PSME REST server sends GAMI request using TPM's UUID and boolean value determining whether this specific TPM
 * is supposed to be enabled or disabled. However, BMC identifies the TPM using its unique configuration index,
 * which is 1-based value for each TPM. If the TPM is supposed to be disabled, the device index should be set to 0.
 * If the TPM is supposed to be enabled, the device index should be set to its configuration index. If requested
 * TPM state is null (no change), the configuration index is set to actual TPM state.
 * */
std::uint8_t get_version_selection(const TrustedModule& tpm, const json::Json& value) {
    if (value.is_null()) {
        if (tpm.get_status().get_state() == enums::State::Enabled) {
            return tpm.get_configuration_index();
        }
        return ipmi::command::sdv::constants::DISABLE_TPM;
    }
    else if (value.get<bool>() == true) {
        return tpm.get_configuration_index();
    }
    return ipmi::command::sdv::constants::DISABLE_TPM;
}


void send_set_boot_options(ManagementController& mc, ConfigurableSystemAttributes& attributes_to_configure) {
    request::SetSystemBootOptions request{convert_boot_override_to_ipmi_enum(attributes_to_configure.BootOverride),
                                          convert_boot_mode_to_ipmi_enum(attributes_to_configure.BootMode),
                                          convert_boot_override_target_to_ipmi_enum(
                                              attributes_to_configure.BootOverrideTarget)};
    try {
        response::SetSystemBootOptions response{};
        mc.send(request, response);
    }
    catch (ipmi::ResponseError& error) {
        THROW(IpmiError, "compute-agent",
              "Error Sending Set Boot Options: " + std::string(error.what())
              + " CC: " + std::to_string(unsigned(error.get_completion_code())));
    }
}


void send_tpm_configuration(ManagementController& mc, ConfigurableTpmAttributes& attributes) {
    ipmi::command::sdv::rsd::request::SetTpmConfiguration request{};
    request.set_clear_tpm_action(attributes.clear_tpm);
    request.set_tpm_version_selection(attributes.version_selection);
    request.set_tpm_command_status(ipmi::command::sdv::TpmCommandStatus::COMMAND_VALID);
    try {
        ipmi::command::sdv::rsd::response::SetTpmConfiguration response{};
        mc.send(request, response);
    }
    catch (ipmi::ResponseError& error) {
        THROW(IpmiError, "compute-agent",
              "Error Sending Set TPM configuration: " + std::string(error.what())
              + " CC: " + std::to_string(unsigned(error.get_completion_code())));
    }
}


void send_chassis_control_command(PowerState power_state, const std::string& action, ManagementController& mc) {
    request::ChassisControlCommand request{};
    response::ChassisControlCommand response{};

    request.set_power_state(power_state);

    try {
        mc.send(request, response);
    }
    catch (ipmi::ResponseError& error) {
        THROW(IpmiError, "compute-agent",
              "Error Sending " + action + ": " + std::string(error.what())
              + " CC: " + std::to_string(unsigned(error.get_completion_code())));
    }
}

std::function<void(ManagementController& mc)> send_power_on =
    std::bind(send_chassis_control_command, PowerState::POWER_UP, "Power On", std::placeholders::_1);
std::function<void(ManagementController& mc)> send_power_off =
    std::bind(send_chassis_control_command, PowerState::POWER_DOWN, "Power Off", std::placeholders::_1);
std::function<void(ManagementController& mc)> send_power_cycle =
    std::bind(send_chassis_control_command, PowerState::POWER_CYCLE, "Power Cycle", std::placeholders::_1);
std::function<void(ManagementController& mc)> send_soft_power_off =
    std::bind(send_chassis_control_command, PowerState::ACPI_SOFT_SHUTDOWN, "Soft Power Off", std::placeholders::_1);
std::function<void(ManagementController& mc)> send_reset =
    std::bind(send_chassis_control_command, PowerState::HARD_RESET, "Reset", std::placeholders::_1);


System get_system(const std::string& system_uuid) {
    auto& sm = CommonComponents::get_instance()->get_system_manager();
    return sm.get_entry(system_uuid);
}


System::Reference get_system_reference(const std::string& system_uuid) {
    auto& sm = CommonComponents::get_instance()->get_system_manager();
    return sm.get_entry_reference(system_uuid);
}


Manager find_parent_manager(const std::string& system_uuid) {
    auto system = get_system(system_uuid);

    auto& mm = CommonComponents::get_instance()->get_module_manager();
    auto managers = mm.get_keys("", [&system](const Manager& manager) {
        return manager.get_uuid() == system.get_parent_uuid();
    });

    if (managers.empty()) {
        THROW(InvalidUuid, "compute-agent",
              "Manger for System: '" + system_uuid + "' not found!");
    }

    return mm.get_entry(managers.front());
}


void set_connection_data(ManagementController& mc, const std::string& system_uuid) {
    auto manager = find_parent_manager(system_uuid);
    auto connection_data = manager.get_connection_data();

    mc.set_ip(connection_data.get_ip_address());
    mc.set_port(connection_data.get_port());
    mc.set_username(connection_data.get_username());
    mc.set_password(connection_data.get_password());
}

void set_boot_options(const std::string& system_uuid, ConfigurableSystemAttributes& attributes_to_configure) {
    const auto system = get_system(system_uuid);
    // Fill boot options missing from setComponentAttributes
    // with current values for the system.
    if (!attributes_to_configure.BootOverride.has_value()) {
        attributes_to_configure.BootOverride = OptionalField<enums::BootOverride>(system.get_boot_override());
    }
    if (!attributes_to_configure.BootMode.has_value()) {
        attributes_to_configure.BootMode = OptionalField<enums::BootOverrideMode>(system.get_boot_override_mode());
    }
    if (!attributes_to_configure.BootOverrideTarget.has_value()) {
        attributes_to_configure.BootOverrideTarget = OptionalField<enums::BootOverrideTarget>(
            system.get_boot_override_target());
    }

    log_info(GET_LOGGER("compute-agent"), "Sending Set Boot Options for System: " << system_uuid);
    log_info(GET_LOGGER("compute-agent"), "BootOverride: " << attributes_to_configure.BootOverride);
    log_info(GET_LOGGER("compute-agent"), "BootMode: " << attributes_to_configure.BootMode);
    log_info(GET_LOGGER("compute-agent"), "BootOverrideTarget: " << attributes_to_configure.BootOverrideTarget);

    ManagementController mc{};
    set_connection_data(mc, system_uuid);

    send_set_boot_options(mc, attributes_to_configure);
    if (attributes_to_configure.BootOverrideTarget == enums::BootOverrideTarget::RemoteDrive) {
        agent::compute::set_iscsi_oob_parameters(system, mc);
    }
    else {
        agent::compute::clear_iscsi_oob_parameters(system, mc);
    }

    auto system_ref = get_system_reference(system_uuid);
    system_ref->set_boot_override(attributes_to_configure.BootOverride);
    system_ref->set_boot_override_mode(attributes_to_configure.BootMode);
    system_ref->set_boot_override_target(attributes_to_configure.BootOverrideTarget);
}


void set_power_state(const std::string& system_uuid, enums::ResetType power_state) {
    log_info(GET_LOGGER("compute-agent"), "Set Power State Options for System: " << system_uuid);
    log_info(GET_LOGGER("compute-agent"), "ResetType: " << power_state);

    ManagementController mc{};
    set_connection_data(mc, system_uuid);

    switch (power_state) {
        case enums::ResetType::On:
            log_debug(GET_LOGGER("compute-agent"), "Sending Power State On.");
            send_power_on(mc);
            get_system_reference(system_uuid)->set_power_state(enums::PowerState::On);
            break;
        case enums::ResetType::ForceOff:
            log_debug(GET_LOGGER("compute-agent"), "Sending Power State Off.");
            send_power_off(mc);
            get_system_reference(system_uuid)->set_power_state(enums::PowerState::Off);
            break;

        case enums::ResetType::GracefulRestart:
            log_debug(GET_LOGGER("compute-agent"), "Sending GracefulRestart.");
            send_reset(mc);
            break;
        case enums::ResetType::ForceRestart:
            log_debug(GET_LOGGER("compute-agent"), "Sending ForceRestart.");
            send_power_cycle(mc);
            break;
        case enums::ResetType::GracefulShutdown:
            log_debug(GET_LOGGER("compute-agent"), "Sending SoftShutdown.");
            send_soft_power_off(mc);
            get_system_reference(system_uuid)->set_power_state(enums::PowerState::Off);
            break;

        case enums::ResetType::ForceOn:
        case enums::ResetType::PushPowerButton:
        case enums::ResetType::Nmi:
            THROW(UnsupportedValue, "compute-agent",
                  std::string(power_state.to_string()) + " not supported.");
            break;

        case enums::ResetType::None:
        default:
            log_debug(GET_LOGGER("compute-agent"), "Sending Power State None.");
            break;
    }
}

void set_system_mode(const std::string& system_uuid,
                     const ConfigurableSystemAttributes& attributes) {
    log_info(GET_LOGGER("compute-agent"), "Setting System Mode for System: " << system_uuid);

    auto system = get_system(system_uuid);

    ManagementController mc{};
    set_connection_data(mc, system_uuid);

    ipmi::command::sdv::rsd::request::SetSystemMode request{};
    ipmi::command::sdv::rsd::response::SetSystemMode response{};

    request.set_system_mode(attributes.UserModeEnabled.has_value() && true == attributes.UserModeEnabled.value() ?
                            ipmi::command::sdv::SystemMode::USER_MODE : ipmi::command::sdv::SystemMode::ADMIN_MODE);

    try {
        mc.send(request, response);
    }
    catch (const ipmi::ResponseError& error) {
        THROW(IpmiError, "compute-agent",
              "Error sending " + std::string(request.get_command_name()) + ": " + std::string(error.what())
              + " CC: " + std::to_string(unsigned(error.get_completion_code())));
    }

    get_system_reference(system_uuid)->set_user_mode_enabled(attributes.UserModeEnabled);
}

void process_system(const std::string& system_uuid,
                    const attribute::Attributes& attributes,
                    SetComponentAttributes::Response& response) {

    CommonValidator::validate_set_system_attributes(attributes);
    const auto attribute_names = attributes.get_names();
    if (attribute_names.empty()) {
        log_info(GET_LOGGER("compute-agent"), "Nothing has been changed (empty request).");
        return;
    }

    ConfigurableSystemAttributes attributes_to_configure;
    for (const auto& attribute_name : attribute_names) {
        const auto& value = attributes.get_value(attribute_name);

        log_debug(GET_LOGGER("compute-agent"), "Attribute name: " << attribute_name);
        log_debug(GET_LOGGER("compute-agent"), "Attribute value: " << value.dump());

        if (literals::System::BOOT_OVERRIDE == attribute_name) {
            attributes_to_configure.BootOverride = OptionalField<enums::BootOverride>(value);
        }
        else if (literals::System::BOOT_OVERRIDE_MODE == attribute_name) {
            attributes_to_configure.BootMode = OptionalField<enums::BootOverrideMode>(value);
        }
        else if (literals::System::BOOT_OVERRIDE_TARGET == attribute_name) {
            attributes_to_configure.BootOverrideTarget = OptionalField<enums::BootOverrideTarget>(value);
        }
        else if (literals::System::POWER_STATE == attribute_name) {
            attributes_to_configure.PowerState = OptionalField<enums::ResetType>(value);
        }
        else if (literals::System::ASSET_TAG == attribute_name) {
            attributes_to_configure.AssetTag = OptionalField<std::string>(value);
        }
        else if (literals::System::USER_MODE_ENABLED == attribute_name) {
            attributes_to_configure.UserModeEnabled = OptionalField<bool>(value);
        }
        else if (literals::System::OEM == attribute_name) {
            attributes_to_configure.Oem = OptionalField<attribute::Oem>(attribute::Oem::from_json(value));
        }
    }

    // if one is disabled/none, set the second one
    if (enums::BootOverride::Disabled == attributes_to_configure.BootOverride ||
        enums::BootOverrideTarget::None == attributes_to_configure.BootOverrideTarget) {
        attributes_to_configure.BootOverrideTarget = enums::BootOverrideTarget::None;
        attributes_to_configure.BootOverride = enums::BootOverride::Disabled;
    }

    try {
        if (attributes_to_configure.AssetTag.has_value()) {
            auto system = get_system_reference(system_uuid);
            system->set_asset_tag(attributes_to_configure.AssetTag);
        }
    }
    catch (const GamiException& ex) {
        response.add_status({literals::System::ASSET_TAG, ex.get_error_code(), ex.get_message()});
    }

    try {
        if (attributes_to_configure.Oem.has_value()) {
            const auto& value = attributes.get_value(literals::System::OEM);
            THROW(UnsupportedField, "compute-agent", "Setting attribute is not supported.", literals::System::OEM, value);
        }
    }
    catch (const GamiException& ex) {
        response.add_status({literals::System::OEM, ex.get_error_code(), ex.get_message()});
    }

    // if any of the boot options has changed, update boot options
    if (attributes_to_configure.BootOverride.has_value() || attributes_to_configure.BootMode.has_value() ||
        attributes_to_configure.BootOverrideTarget.has_value()) {
        try {
            if (enums::BootOverride::Disabled != attributes_to_configure.BootOverride) {
                set_boot_options(system_uuid, attributes_to_configure);
            }
        }
        catch (const GamiException& ex) {
            response.add_status({literals::System::BOOT_OVERRIDE, ex.get_error_code(), ex.get_message()});
            response.add_status({literals::System::BOOT_OVERRIDE_MODE, ex.get_error_code(), ex.get_message()});
            response.add_status({literals::System::BOOT_OVERRIDE_TARGET, ex.get_error_code(), ex.get_message()});
        }
    }

    if (attributes_to_configure.UserModeEnabled.has_value()) {
        try {
            set_system_mode(system_uuid, attributes_to_configure);
        }
        catch (const GamiException& ex) {
            response.add_status({literals::System::USER_MODE_ENABLED, ex.get_error_code(), ex.get_message()});
        }
    }

    // if reset was sent, update power status
    if (attributes_to_configure.PowerState.has_value() &&
        enums::ResetType::None != attributes_to_configure.PowerState) {
        try {
            set_power_state(system_uuid, attributes_to_configure.PowerState);
        }
        catch (const GamiException& ex) {
            response.add_status({literals::System::POWER_STATE, ex.get_error_code(), ex.get_message()});
        }
    }
}


void process_subsystem(const std::string&, const attribute::Attributes& attributes,
                       SetComponentAttributes::Response& response) {

    ComputeValidator::validate_set_storage_subsystem_attributes(attributes);
    const auto attribute_names = attributes.get_names();
    if (attribute_names.empty()) {
        log_debug(GET_LOGGER("compute-agent"), "setComponentAttributes: nothing has been changed (empty request).");
        return;
    }

    for (const auto& name : attribute_names) {
        try {
            // The response must have a message for every attribute that could not be set.
            const auto& value = attributes.get_value(name);
            THROW(UnsupportedField, "compute-agent", "Setting attribute is not supported.", name, value);
        }
        catch (const GamiException& ex) {
            response.add_status({name, ex.get_error_code(), ex.get_message()});
        }
    }
}


void process_drive(const std::string& uuid, const attribute::Attributes& attributes,
                   SetComponentAttributes::Response& response) {

    CommonValidator::validate_set_drive_attributes(attributes);
    const auto attribute_names = attributes.get_names();
    if (attribute_names.empty()) {
        log_debug(GET_LOGGER("compute-agent"), "setComponentAttributes: nothing has been changed (empty request).");
        return;
    }
    for (const auto& name : attribute_names) {
        const auto& value = attributes.get_value(name);
        try {
            if (literals::Drive::ERASED == name) {
                THROW(UnsupportedField, "compute-agent",
                      "Setting field is not supported in local storage drives!", name, value);
            }
            else if (literals::Drive::ASSET_TAG == name) {
                CommonComponents::get_instance()->get_drive_manager().get_entry_reference(uuid)->set_asset_tag(value);
            }
            else if (literals::Drive::OEM == name) {
                THROW(UnsupportedField, "compute-agent", "Setting attribute is not supported.", name, value);
            }
        }
        catch (const GamiException& ex) {
            response.add_status({name, ex.get_error_code(), ex.get_message()});
        }
    }
}


void process_chassis(const std::string& uuid, const attribute::Attributes& attributes,
                   SetComponentAttributes::Response& response) {

    CommonValidator::validate_set_chassis_attributes(attributes);
    const auto attribute_names = attributes.get_names();
    if (attribute_names.empty()) {
        log_debug(GET_LOGGER("compute-agent"), "setComponentAttributes: nothing has been changed (empty request).");
        return;
    }
    for (const auto& name : attribute_names) {
        const auto& value = attributes.get_value(name);
        try {
            if (literals::Drive::ASSET_TAG == name) {
                CommonComponents::get_instance()->get_chassis_manager().get_entry_reference(uuid)->set_asset_tag(value);
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
}

void process_trusted_module(const std::string& uuid, const attribute::Attributes& attributes,
                     SetComponentAttributes::Response& response) {

    ComputeValidator::validate_set_trusted_module_attributes(attributes);
    const auto attribute_names = attributes.get_names();
    if (attribute_names.empty()) {
        log_debug(GET_LOGGER("compute-agent"), "setComponentAttributes: nothing has been changed (empty request).");
        return;
    }
    auto tpm = agent_framework::module::get_manager<TrustedModule>().get_entry(uuid);
    ConfigurableTpmAttributes attributes_to_set{};

    ManagementController mc{};
    set_connection_data(mc, tpm.get_parent_uuid());

    // We know that setting anything on a TPM requires creating a task
    action::TaskCreator task_creator;
    task_creator.prepare_task();

    auto promised_response_builder = []() {
        return json::Json{};
    };

    auto task_completion_monitor = [] (ManagementController controller) {
        ipmi::command::sdv::rsd::request::GetTpmConfiguration get_tpm_status_request{};
        ipmi::command::sdv::rsd::response::GetTpmConfiguration get_tpm_status_response{};

        do {
            log_info(GET_LOGGER("compute-agent"), "Checking if TPM settings have changed.");

            std::this_thread::sleep_for(std::chrono::seconds(3));
            controller.send(get_tpm_status_request, get_tpm_status_response);

            if (get_tpm_status_response.get_tpm_command_status() == ipmi::command::sdv::TpmCommandStatus::COMMAND_INVALID) {
                log_info(GET_LOGGER("compute-agent"), "TPM settings applied.");
                break;
            }
            else {
                log_info(GET_LOGGER("compute-agent"), "TPM setting not applied yet. Waiting.");
            }
        } while (true);
    };

    for (const auto& name : attribute_names) {
        const auto& value = attributes.get_value(name);
        try {
            if (literals::TrustedModule::DEVICE_ENABLED == name) {
                attributes_to_set.version_selection = get_version_selection(tpm, value);
            }
            else if (literals::TrustedModule::CLEAR_OWNERSHIP == name) {
                attributes_to_set.clear_tpm = get_clear_tpm(value.get<bool>());
            }
            else {
                // The response must have a message for every attribute that could not be set.
                THROW(UnsupportedField, "compute-agent", "Setting attribute is not supported.", name, value);
            }

            task_creator.set_promised_error_thrower([] () {
                return exceptions::GamiException(exceptions::ErrorCode::COMPUTE, "Failed to apply changes to TPM.");
            });
        }
        catch (const GamiException& ex) {
            response.add_status({name, ex.get_error_code(), ex.get_message()});
        }
    }

    if (!response.get_statuses().empty()) {
        return;
    }

    task_creator.add_subtask(std::bind(send_tpm_configuration, mc, attributes_to_set));
    task_creator.add_subtask(std::bind(task_completion_monitor, mc));
    task_creator.set_promised_response(promised_response_builder);

    task_creator.register_task();
    auto set_tpm_configuration_task = task_creator.get_task();
    action::TaskRunner::get_instance().run(set_tpm_configuration_task);

    log_info(GET_LOGGER("compute-agent"), "Setting TPM configuration will be finished after sled reboot.");

    response.set_task(task_creator.get_task_resource().get_uuid());
}


bool exists_in_compute(const std::string& uuid) {
    const auto& compute_components = ComputeComponents::get_instance();
    const auto& common_components = CommonComponents::get_instance();

    return common_components->get_module_manager().entry_exists(uuid) ||
           agent_framework::module::get_manager<Processor>().entry_exists(uuid) ||
           compute_components->get_memory_manager().entry_exists(uuid) ||
           compute_components->get_network_device_manager().entry_exists(uuid) ||
           compute_components->get_storage_controller_manager().entry_exists(uuid);
}


void set_component_attributes(const SetComponentAttributes::Request& request,
                              SetComponentAttributes::Response& response) {
    log_info(GET_LOGGER("compute-agent"), "Executing setComponentAttributes.");

    const auto& uuid = request.get_component();
    const auto& attributes = request.get_attributes();

    if (CommonComponents::get_instance()->get_system_manager().entry_exists(uuid)) {
        process_system(uuid, attributes, response);
    }
    else if (agent_framework::module::get_manager<StorageSubsystem>().entry_exists(uuid)) {
        process_subsystem(uuid, attributes, response);
    }
    else if (agent_framework::module::get_manager<Drive>().entry_exists(uuid)) {
        process_drive(uuid, attributes, response);
    }
    else if (agent_framework::module::get_manager<Chassis>().entry_exists(uuid)) {
        process_chassis(uuid, attributes, response);
    }
    else if (agent_framework::module::get_manager<NetworkDeviceFunction>().entry_exists(uuid)) {
        agent::compute::process_network_device_function(uuid, attributes, response);
    }
    else if (agent_framework::module::get_manager<TrustedModule>().entry_exists(uuid)) {
        process_trusted_module(uuid, attributes, response);
    }
    else if (agent_framework::module::get_manager<NetworkInterface>().entry_exists(uuid)) {
        THROW(UnsupportedValue, "compute-agent", "Operation for Network Interface is not supported!");
    }
    else if (agent_framework::module::get_manager<ThermalZone>().entry_exists(uuid)) {
        THROW(UnsupportedValue, "compute-agent", "Operation for Thermal Zone is not supported!");
    }
    else if (exists_in_compute(uuid)) {
        THROW(InvalidValue, "compute-agent", "Operation not available for this component.");
    }
    else {
        THROW(InvalidUuid, "compute-agent", "No component with UUID = '" + uuid + "'.");
    }

    log_info(GET_LOGGER("compute-agent"), "setComponentAttributes finished successfully.");
}

}

REGISTER_COMMAND(SetComponentAttributes, ::set_component_attributes);
