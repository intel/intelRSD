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
 * @brief Implementation of SetComponentAttributes command
 * */

#include "agent-framework/command/compute_commands.hpp"
#include "agent-framework/command/registry.hpp"
#include "agent-framework/module/compute_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/requests/validation/compute.hpp"
#include "agent-framework/module/requests/validation/common.hpp"
#include "agent-framework/action/task_runner.hpp"
#include "agent-framework/action/task_creator.hpp"

#include "ipmi/manager/ipmitool/management_controller.hpp"
#include "ipmi/command/generic/get_chassis_status.hpp"
#include "ipmi/command/generic/chassis_control_command.hpp"
#include "ipmi/command/generic/set_system_boot_options.hpp"
#include "ipmi/command/sdv/rsd/set_system_mode.hpp"

#include "command/handle_dcpmem_erase.hpp"
#include "command/set_network_device_function_attributes.hpp"
#include "command/set_performance_configuration.hpp"
#include "command/set_trusted_module_attributes.hpp"
#include "command/handle_system_power_state.hpp"

#include "generic/assertions.hpp"


using namespace agent_framework;
using namespace agent_framework::command;
using namespace agent_framework::exceptions;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework::model::requests::validation;
using namespace ipmi::manager::ipmitool;
using namespace ipmi::command::generic;
using namespace ::generic;

namespace {

struct ConfigurableSystemAttributes {
    OptionalField<enums::BootOverride> boot_override{};
    OptionalField<enums::BootOverrideTarget> boot_override_target{};
    OptionalField<enums::BootOverrideMode> boot_mode{};
    OptionalField<enums::ResetType> reset_type{};
    OptionalField<std::string> asset_tag{};
    OptionalField<bool> user_mode_enabled{};
    OptionalField<bool> dcpmem_reset_configuration{};
    OptionalField<bool> dcpmem_erase_configuration_keys{};
    OptionalField<attribute::Oem> oem{};
    OptionalField<std::uint8_t> current_performance_configuration{};
};


BootOverrideTarget convert_boot_override_target_to_ipmi_enum(enums::BootOverrideTarget target) {
    switch (target) {
        case enums::BootOverrideTarget::Hdd:
            return BootOverrideTarget::Hdd;
        case enums::BootOverrideTarget::RemoteDrive:
            return BootOverrideTarget::RemoteDrive;
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


System get_system(const Uuid& system_uuid) {
    auto& sm = get_manager<System>();
    return sm.get_entry(system_uuid);
}


Manager find_parent_manager(const Uuid& system_uuid) {
    auto system = get_system(system_uuid);

    auto& mm = get_manager<Manager>();
    auto managers = mm.get_keys("", [&system](const Manager& manager) {
        return manager.get_uuid() == system.get_parent_uuid();
    });

    if (managers.empty()) {
        THROW(InvalidUuid, "compute-agent",
            "Manger for System: '" + system_uuid + "' not found!");
    }

    return mm.get_entry(managers.front());
}


void set_connection_data(ManagementController& mc, const Uuid& system_uuid) {
    auto manager = find_parent_manager(system_uuid);
    auto connection_data = manager.get_connection_data();

    mc.set_ip(connection_data.get_ip_address());
    mc.set_port(connection_data.get_port());
    mc.set_username(connection_data.get_username());
    mc.set_password(connection_data.get_password());
}


void send_set_boot_options(ManagementController& mc, ConfigurableSystemAttributes& attributes_to_configure) {
    request::SetSystemBootOptions request{convert_boot_override_to_ipmi_enum(attributes_to_configure.boot_override),
                                          convert_boot_mode_to_ipmi_enum(attributes_to_configure.boot_mode),
                                          convert_boot_override_target_to_ipmi_enum(
                                              attributes_to_configure.boot_override_target)};
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


System::Reference get_system_reference(const Uuid& system_uuid) {
    auto& sm = get_manager<System>();
    return sm.get_entry_reference(system_uuid);
}


void set_boot_options(const Uuid& system_uuid, ConfigurableSystemAttributes& attributes_to_configure) {
    const auto system = get_system(system_uuid);
    // Fill boot options missing from setComponentAttributes
    // with current values for the system.
    if (!attributes_to_configure.boot_override.has_value()) {
        attributes_to_configure.boot_override = system.get_boot_override();
    }
    if (!attributes_to_configure.boot_mode.has_value()) {
        attributes_to_configure.boot_mode = system.get_boot_override_mode();
    }
    if (!attributes_to_configure.boot_override_target.has_value()) {
        attributes_to_configure.boot_override_target = system.get_boot_override_target();
    }

    log_info("compute-agent", "Sending Set Boot Options for System: " << system_uuid);
    log_info("compute-agent", "boot_override: " << attributes_to_configure.boot_override);
    log_info("compute-agent", "boot_mode: " << attributes_to_configure.boot_mode);
    log_info("compute-agent", "boot_override_target: " << attributes_to_configure.boot_override_target);

    ManagementController mc{};
    set_connection_data(mc, system_uuid);

    send_set_boot_options(mc, attributes_to_configure);
    if (attributes_to_configure.boot_override_target == enums::BootOverrideTarget::RemoteDrive) {
        agent::compute::set_iscsi_oob_parameters(system, mc);
    }
    else {
        agent::compute::clear_iscsi_oob_parameters(system, mc);
    }

    auto system_ref = get_system_reference(system_uuid);
    system_ref->set_boot_override(attributes_to_configure.boot_override);
    system_ref->set_boot_override_mode(attributes_to_configure.boot_mode);
    system_ref->set_boot_override_target(attributes_to_configure.boot_override_target);
}

void set_system_mode(const Uuid& system_uuid,
                     const ConfigurableSystemAttributes& attributes) {
    log_info("compute-agent", "Setting System Mode for System: " << system_uuid);

    auto system = get_system(system_uuid);

    ManagementController mc{};
    set_connection_data(mc, system_uuid);

    ipmi::command::sdv::rsd::request::SetSystemMode request{};
    ipmi::command::sdv::rsd::response::SetSystemMode response{};

    request.set_system_mode(attributes.user_mode_enabled.has_value() && true == attributes.user_mode_enabled.value() ?
                            ipmi::command::sdv::SystemMode::USER_MODE : ipmi::command::sdv::SystemMode::ADMIN_MODE);

    try {
        mc.send(request, response);
    }
    catch (const ipmi::ResponseError& error) {
        THROW(IpmiError, "compute-agent",
              "Error sending " + std::string(request.get_command_name()) + ": " + std::string(error.what())
              + " CC: " + std::to_string(unsigned(error.get_completion_code())));
    }

    get_system_reference(system_uuid)->set_user_mode_enabled(attributes.user_mode_enabled);
}

void process_system(const Uuid& system_uuid,
                    const attribute::Attributes& attributes,
                    SetComponentAttributes::Response& response) {

    CommonValidator::validate_set_system_attributes(attributes);
    const auto attribute_names = attributes.get_names();
    if (attribute_names.empty()) {
        log_info("compute-agent", "Nothing has been changed (empty request).");
        return;
    }

    bool power_state_found{false};
    bool dcpmem_erase_found{false};
    bool performance_configuration_found{false};
    bool other_found{false};
    ConfigurableSystemAttributes attributes_to_configure;
    for (const auto& attribute_name : attribute_names) {
        const auto& value = attributes.get_value(attribute_name);

        log_debug("compute-agent", "Attribute name: " << attribute_name);
        log_debug("compute-agent", "Attribute value: " << value.dump());

        if (literals::System::BOOT_OVERRIDE == attribute_name) {
            attributes_to_configure.boot_override = OptionalField<enums::BootOverride>(value);
            other_found = true;
        }
        else if (literals::System::BOOT_OVERRIDE_MODE == attribute_name) {
            attributes_to_configure.boot_mode = OptionalField<enums::BootOverrideMode>(value);
            other_found = true;
        }
        else if (literals::System::BOOT_OVERRIDE_TARGET == attribute_name) {
            attributes_to_configure.boot_override_target = OptionalField<enums::BootOverrideTarget>(value);
            other_found = true;
        }
        else if (literals::System::RESET == attribute_name) {
            attributes_to_configure.reset_type = OptionalField<enums::ResetType>(value);
            power_state_found = true;
        }
        else if (literals::System::ASSET_TAG == attribute_name) {
            attributes_to_configure.asset_tag = OptionalField<std::string>(value);
            other_found = true;
        }
        else if (literals::System::USER_MODE_ENABLED == attribute_name) {
            attributes_to_configure.user_mode_enabled = OptionalField<bool>(value);
            other_found = true;
        }
        else if (literals::System::RESET_CONFIGURATION == attribute_name) {
            attributes_to_configure.dcpmem_reset_configuration = OptionalField<bool>(value);
            dcpmem_erase_found = true;
        }
        else if (literals::System::ERASE_CONFIGURATION_KEYS == attribute_name) {
            attributes_to_configure.dcpmem_erase_configuration_keys = OptionalField<bool>(value);
            dcpmem_erase_found = true;
        }
        else if (literals::System::OEM == attribute_name) {
            attributes_to_configure.oem = OptionalField<attribute::Oem>(attribute::Oem::from_json(value));
            other_found = true;
        }
        else if (literals::System::CURRENT_PERFORMANCE_CONFIGURATION == attribute_name) {
            attributes_to_configure.current_performance_configuration = OptionalField<std::uint8_t>(value.get<std::uint8_t>());
            performance_configuration_found = true;
        }
    }

    if (power_state_found && (other_found || dcpmem_erase_found || performance_configuration_found)) {
        THROW(UnsupportedField, "agent-framework", "Not supported with other attributes.",
            literals::System::RESET, attributes_to_configure.reset_type);
    }

    if (dcpmem_erase_found && (other_found || power_state_found || performance_configuration_found)) {
        std::string field_name;
        json::Json field_value;

        if (attributes_to_configure.dcpmem_reset_configuration.has_value()) {
            field_name = literals::System::RESET_CONFIGURATION;
            field_value = attributes_to_configure.dcpmem_reset_configuration;
        }
        else {
            field_name = literals::System::ERASE_CONFIGURATION_KEYS;
            field_value = attributes_to_configure.dcpmem_erase_configuration_keys;
        }

        THROW(UnsupportedField, "agent-framework", "Not supported with other attributes.", field_name, field_value);
    }

    if (performance_configuration_found && (other_found || power_state_found || dcpmem_erase_found)) {
        THROW(UnsupportedField, "agent-framework", "Not supported with other attributes.",
              literals::System::CURRENT_PERFORMANCE_CONFIGURATION, attributes_to_configure.current_performance_configuration.value());
    }

    // if one is disabled/none, set the second one
    if (enums::BootOverride::Disabled == attributes_to_configure.boot_override ||
        enums::BootOverrideTarget::None == attributes_to_configure.boot_override_target) {
        attributes_to_configure.boot_override_target = enums::BootOverrideTarget::None;
        attributes_to_configure.boot_override = enums::BootOverride::Disabled;
    }

    try {
        if (attributes_to_configure.asset_tag.has_value()) {
            auto system = get_system_reference(system_uuid);
            system->set_asset_tag(attributes_to_configure.asset_tag);
        }
    }
    catch (const GamiException& ex) {
        response.add_status({literals::System::ASSET_TAG, ex.get_error_code(), ex.get_message()});
    }

    try {
        if (attributes_to_configure.oem.has_value()) {
            const auto& value = attributes.get_value(literals::System::OEM);
            THROW(UnsupportedField, "compute-agent", "Setting attribute is not supported.", literals::System::OEM, value);
        }
    }
    catch (const GamiException& ex) {
        response.add_status({literals::System::OEM, ex.get_error_code(), ex.get_message()});
    }

    // if any of the boot options has changed, update boot options
    if (attributes_to_configure.boot_override.has_value() || attributes_to_configure.boot_mode.has_value() ||
        attributes_to_configure.boot_override_target.has_value()) {
        try {
            if (enums::BootOverride::Disabled != attributes_to_configure.boot_override) {
                set_boot_options(system_uuid, attributes_to_configure);
            }
        }
        catch (const GamiException& ex) {
            response.add_status({literals::System::BOOT_OVERRIDE, ex.get_error_code(), ex.get_message()});
            response.add_status({literals::System::BOOT_OVERRIDE_MODE, ex.get_error_code(), ex.get_message()});
            response.add_status({literals::System::BOOT_OVERRIDE_TARGET, ex.get_error_code(), ex.get_message()});
        }
    }

    if (attributes_to_configure.user_mode_enabled.has_value()) {
        try {
            set_system_mode(system_uuid, attributes_to_configure);
        }
        catch (const GamiException& ex) {
            response.add_status({literals::System::USER_MODE_ENABLED, ex.get_error_code(), ex.get_message()});
        }
    }

    // if reset was sent, update power status
    if (attributes_to_configure.reset_type.has_value()) {
        if (enums::ResetType::None != attributes_to_configure.reset_type) {
            try {
                Uuid task = agent::compute::process_system_power_state(system_uuid, attributes_to_configure.reset_type);
                if (!task.empty()) {
                    response.set_task(task);
                }
            }
            catch (const GamiException& ex) {
                response.add_status({literals::System::RESET, ex.get_error_code(), ex.get_message()});
            }
        }
    }

    if (dcpmem_erase_found) {
        agent::compute::EraseDcpmemAttributes erase_attributes{false, false};
        if (attributes_to_configure.dcpmem_reset_configuration.has_value()) {
            erase_attributes.reset_configuration = attributes_to_configure.dcpmem_reset_configuration.value();
        }

        if (attributes_to_configure.dcpmem_erase_configuration_keys.has_value()) {
            erase_attributes.erase_configuration_keys = attributes_to_configure.dcpmem_erase_configuration_keys.value();
        }

        try {
            Uuid task = agent::compute::process_dcpmem_erase(system_uuid, erase_attributes);

            if (!task.empty()) {
                response.set_task(task);
            }
        }
        catch (const GamiException& ex) {
            response.add_status({literals::System::ERASE_DCPMEM, ex.get_error_code(), ex.get_message()});
        }
    }

    if (performance_configuration_found) {
        if (attributes_to_configure.current_performance_configuration.value() > 7 ||
            attributes_to_configure.current_performance_configuration.value() >= get_system(system_uuid).get_performance_configurations().size()) {
            THROW(UnsupportedValue, "compute-agent", "Configuration index must be below number of configurations and not greater than 7.");
        }
        // Node index and CPU index set to 0 as this sets configuration for the whole system
        agent::compute::PerformanceConfigurationAttributes configuration{0, 0, attributes_to_configure.current_performance_configuration.value()};
        try {
            Uuid task = agent::compute::process_performance_configuration(system_uuid, configuration);

            if (!task.empty()) {
                response.set_task(task);
            }
        }
        catch (const GamiException& ex) {
            response.add_status({literals::System::CURRENT_PERFORMANCE_CONFIGURATION, ex.get_error_code(), ex.get_message()});
        }
    }
}


void process_subsystem(const Uuid&, const attribute::Attributes& attributes,
                       SetComponentAttributes::Response& response) {

    ComputeValidator::validate_set_storage_subsystem_attributes(attributes);
    const auto attribute_names = attributes.get_names();
    if (attribute_names.empty()) {
        log_debug("compute-agent", "setComponentAttributes: nothing has been changed (empty request).");
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


void process_drive(const Uuid& uuid, const attribute::Attributes& attributes,
                   SetComponentAttributes::Response& response) {

    CommonValidator::validate_set_drive_attributes(attributes);
    const auto attribute_names = attributes.get_names();
    if (attribute_names.empty()) {
        log_debug("compute-agent", "setComponentAttributes: nothing has been changed (empty request).");
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
                get_manager<Drive>().get_entry_reference(uuid)->set_asset_tag(value);
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


void process_chassis(const Uuid& uuid, const attribute::Attributes& attributes,
                   SetComponentAttributes::Response& response) {

    CommonValidator::validate_set_chassis_attributes(attributes);
    const auto attribute_names = attributes.get_names();
    if (attribute_names.empty()) {
        log_debug("compute-agent", "setComponentAttributes: nothing has been changed (empty request).");
        return;
    }
    for (const auto& name : attribute_names) {
        const auto& value = attributes.get_value(name);
        try {
            if (literals::Chassis::ASSET_TAG == name) {
                get_manager<Chassis>().get_entry_reference(uuid)->set_asset_tag(value);
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


void process_pcie_device(const Uuid& uuid, const attribute::Attributes& attributes,
                     SetComponentAttributes::Response& response) {

    CommonValidator::validate_set_pcie_device_attributes(attributes);
    const auto attribute_names = attributes.get_names();
    if (attribute_names.empty()) {
        log_debug("compute-agent", "setComponentAttributes: nothing has been changed (empty request).");
        return;
    }
    for (const auto& name : attribute_names) {
        const auto& value = attributes.get_value(name);
        try {
            if (literals::PcieDevice::ASSET_TAG == name) {
                get_manager<PcieDevice>().get_entry_reference(uuid)->set_asset_tag(value);
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


bool exists_in_compute(const Uuid& uuid) {
    return get_manager<Manager>().entry_exists(uuid) ||
           get_manager<Processor>().entry_exists(uuid) ||
           get_manager<Memory>().entry_exists(uuid) ||
           get_manager<NetworkDevice>().entry_exists(uuid) ||
           get_manager<StorageController>().entry_exists(uuid) ||
           get_manager<PcieFunction>().entry_exists(uuid);
}


void set_component_attributes(const SetComponentAttributes::Request& request,
                              SetComponentAttributes::Response& response) {
    log_info("compute-agent", "Executing setComponentAttributes.");

    const auto& uuid = request.get_component();
    const auto& attributes = request.get_attributes();

    if (get_manager<System>().entry_exists(uuid)) {
        process_system(uuid, attributes, response);
    }
    else if (get_manager<StorageSubsystem>().entry_exists(uuid)) {
        process_subsystem(uuid, attributes, response);
    }
    else if (get_manager<Drive>().entry_exists(uuid)) {
        process_drive(uuid, attributes, response);
    }
    else if (get_manager<Chassis>().entry_exists(uuid)) {
        process_chassis(uuid, attributes, response);
    }
    else if (get_manager<NetworkDeviceFunction>().entry_exists(uuid)) {
        agent::compute::process_network_device_function(uuid, attributes, response);
    }
    else if (get_manager<TrustedModule>().entry_exists(uuid)) {
        agent::compute::process_trusted_module(uuid, attributes, response);
    }
    else if (get_manager<PcieDevice>().entry_exists(uuid)) {
        process_pcie_device(uuid, attributes, response);
    }
    else if (get_manager<NetworkInterface>().entry_exists(uuid)) {
        THROW(UnsupportedValue, "compute-agent", "Operation for Network Interface is not supported!");
    }
    else if (get_manager<ThermalZone>().entry_exists(uuid)) {
        THROW(UnsupportedValue, "compute-agent", "Operation for Thermal Zone is not supported!");
    }
    else if (exists_in_compute(uuid)) {
        THROW(InvalidValue, "compute-agent", "Operation not available for this component.");
    }
    else {
        THROW(InvalidUuid, "compute-agent", "No component with UUID = '" + uuid + "'.");
    }

    log_info("compute-agent", "setComponentAttributes finished successfully.");
}

}

REGISTER_COMMAND(SetComponentAttributes, ::set_component_attributes);
