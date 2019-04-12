/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file command/stubs/set_component_attributes.cpp
 * @brief Implementation of SetComponentAttributes command
 * */

#include "agent-framework/module/pnc_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/command/pnc_commands.hpp"
#include "agent-framework/command/registry.hpp"
#include "agent-framework/module/requests/validation/common.hpp"
#include "agent-framework/module/requests/validation/pnc.hpp"
#include "agent-framework/action/task_runner.hpp"
#include "agent-framework/action/task_creator.hpp"
#include "nvme/nvme_secure_erase.hpp"
#include "tools/secure_erase_task.hpp"
#include "tools/toolset.hpp"
#include "gas/global_address_space_registers.hpp"



using namespace agent_framework;
using namespace agent_framework::command;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::exceptions;
using namespace agent_framework::model::attribute;
using namespace agent_framework::model::requests::validation;
using namespace agent::pnc::nvme;
using namespace agent::pnc::tools;
using namespace agent::pnc::gas;

namespace {

/*! NVMe Drive detection delay after binding, seconds */
static constexpr uint32_t DRIVE_DETECTION_DELAY_SEC = 10;


template<typename RESOURCE>
void set_asset_tag(const std::string& uuid, const json::Json& asset_tag) {
    using RAW_TYPE = typename std::remove_reference<RESOURCE>::type;
    enums::Component component = RAW_TYPE::get_component();

    // keep resource locked!
    auto reference = get_manager<RAW_TYPE>().get_entry_reference(uuid);
    RESOURCE& resource = reference.get_raw_ref();

    log_info("pnc-gami", component.to_string() << "(" << uuid << "):: AssetTag = " << asset_tag.dump());
    resource.set_asset_tag(agent_framework::module::utils::OptionalField<std::string>(asset_tag));
}

template <typename T>
void throw_if_not_ready_for_secure_erase(const std::string& uuid) {
    auto drive = get_manager<T>().get_entry(uuid);

    if (drive.get_status().get_health() == enums::Health::Critical) {
        THROW(exceptions::PncError, "pnc-gami", "Device is in critical discovery state");
    }
    if (drive.get_is_being_discovered()) {
        THROW(exceptions::PncError, "pnc-gami", "Device is still being discovered");
    }
    if (drive.get_is_being_erased()) {
        THROW(exceptions::PncError, "pnc-gami", "Device is still being erased");
    }
}

template<typename T>
void securely_erase(const std::string& uuid, SetComponentAttributes::Response& response) {
    log_info("pnc-gami", "Executing erase device securely command.");

    throw_if_not_ready_for_secure_erase<T>(uuid);

    Toolset tools = Toolset::get();
    GlobalAddressSpaceRegisters gas{};
    try {
        Switch sw = get_manager<Switch>().get_entry(tools.model_tool->get_switch_for_device_uuid(uuid));
        gas = GlobalAddressSpaceRegisters::get_default(sw.get_memory_path());
    }
    catch (const std::exception& e) {
        THROW(exceptions::PncError, "pnc-gami", std::string("Secure device erase failed: ") + e.what());
    }

    auto response_builder = []() {
        SetComponentAttributes::Response res{};
        return res.to_json();
    };

    action::TaskCreator task_creator{};
    task_creator.prepare_task();

    task_creator.add_prerun_action(std::bind(std::mem_fn(&ModelTool::set_device_being_erased<T>), *tools.model_tool,
                                             uuid, true));

    task_creator.add_subtask(std::bind(std::mem_fn(&GasTool::bind_device_to_mgmt_partition), *tools.gas_tool,
                                       tools.model_tool, gas, uuid));
    task_creator.add_subtask([]() { std::this_thread::sleep_for(std::chrono::seconds(DRIVE_DETECTION_DELAY_SEC)); });
    task_creator.add_subtask(SecureEraseTask<T>{uuid});

    task_creator.add_exception_callback(std::bind(
        std::mem_fn(&GasTool::unbind_device_from_mgmt_partition), *tools.gas_tool, tools.model_tool, gas, uuid));

    task_creator.add_exception_callback(std::bind(std::mem_fn(&ModelTool::set_device_status<T>),
                                                  *tools.model_tool, uuid,
                                                  attribute::Status{enums::State::StandbyOffline,
                                                                    enums::Health::Warning}));
    task_creator.add_exception_callback(std::bind(
        std::mem_fn(&ModelTool::set_device_in_warning_state<T>), *tools.model_tool, uuid, true));
    task_creator.add_exception_callback(std::bind(
        std::mem_fn(&ModelTool::set_device_being_erased<T>), *tools.model_tool, uuid, false));

    task_creator.add_exception_callback(std::bind(
        std::mem_fn(&ModelTool::set_device_being_erased<T>), *tools.model_tool, uuid, false));

    task_creator.set_promised_response(response_builder);

    task_creator.register_task();
    auto erase_securely_task = task_creator.get_task();
    action::TaskRunner::get_instance().run(erase_securely_task);

    response.set_task(task_creator.get_task_resource().get_uuid());
}


void process_pcie_port(const std::string&, const attribute::Attributes& attributes,
                       SetComponentAttributes::Response& response) {

    PncValidator::validate_set_pcie_port_attributes(attributes);
    const auto attribute_names = attributes.get_names();
    if (attribute_names.empty()) {
        log_debug("pnc-gami", "setComponentAttributes: nothing has been changed (empty request).");
        return;
    }
    for (const auto& name : attribute_names) {
        const auto& value = attributes.get_value(name);
        try {
            if (literals::Port::POWER_STATE == name) {
                THROW(UnsupportedField, "pnc-gami", "Resetting a PCIePort is not supported!", name, value);
            }
            else if (literals::Port::OEM == name) {
                THROW(UnsupportedField, "pnc-gami", "Setting attribute is not supported.", name, value);
            }
        }
        catch (const GamiException& ex) {
            response.add_status({name, ex.get_error_code(), ex.get_message()});
        }
    }
}


void process_pcie_switch(const std::string& uuid, const attribute::Attributes& attributes,
                         SetComponentAttributes::Response& response) {

    PncValidator::validate_set_pcie_switch_attributes(attributes);
    const auto attribute_names = attributes.get_names();
    if (attribute_names.empty()) {
        log_debug("pnc-gami", "setComponentAttributes: nothing has been changed (empty request).");
        return;
    }
    for (const auto& name : attribute_names) {
        const auto& value = attributes.get_value(name);
        try {
            if (literals::Switch::ASSET_TAG == name) {
                set_asset_tag<Switch>(uuid, value);
            }
            else if (literals::Switch::POWER_STATE == name) {
                THROW(UnsupportedField, "pnc-gami", "Resetting a PCIeSwitch is not supported!", name, value);
            }
            else if (literals::Switch::OEM == name) {
                THROW(UnsupportedField, "pnc-gami", "Setting attribute is not supported.", name, value);
            }
        }
        catch (const GamiException& ex) {
            response.add_status({name, ex.get_error_code(), ex.get_message()});
        }
    }
}


void process_pcie_device(const std::string& uuid, const attribute::Attributes& attributes,
                         SetComponentAttributes::Response& response) {

    CommonValidator::validate_set_pcie_device_attributes(attributes);
    const auto attribute_names = attributes.get_names();
    if (attribute_names.empty()) {
        log_debug("agent", "setComponentAttributes: nothing has been changed (empty request).");
        return;
    }
    for (const auto& name : attribute_names) {
        const auto& value = attributes.get_value(name);

        try {
            if (literals::PcieDevice::ASSET_TAG == name) {
                set_asset_tag<PcieDevice>(uuid, value);
            }
            else {
                // The response must have a message for every attribute that could not be set.
                THROW(UnsupportedField, "pnc-gami", "Setting attribute is not supported.", name, value);
            }
        }
        catch (const GamiException& ex) {
            response.add_status({name, ex.get_error_code(), ex.get_message()});
        }
    }
}


void process_drive(const std::string& uuid, const attribute::Attributes& attributes,
                   SetComponentAttributes::Response& response) {

    CommonValidator::validate_set_drive_attributes(attributes);
    const auto& attribute_names = attributes.get_names();
    if (attribute_names.empty()) {
        log_debug("pnc-gami", "setComponentAttributes: nothing has been changed (empty request).");
        return;
    }
    Toolset tools = Toolset::get();
    for (const auto& name : attribute_names) {
        const auto& value = attributes.get_value(name);

        try {
            if (literals::Drive::ASSET_TAG == name) {
                set_asset_tag<Drive>(uuid, value);
            }
            else if (literals::Drive::ERASED == name) {
                tools.model_tool->update_device_erased<Drive>(uuid, value.get<bool>());
                log_debug("pnc-gami", "Set " + name + " attribute to " << std::boolalpha << value.get<bool>());
            }
            else if (literals::Drive::SECURELY_ERASE == name) {
                securely_erase<Drive>(uuid, response);
            }
            else if (literals::Drive::OEM == name) {
                THROW(UnsupportedField, "pnc-gami", "Setting attribute is not supported.", name, value);
            }
        }
        catch (const GamiException& ex) {
            response.add_status({name, ex.get_error_code(), ex.get_message()});
        }
    }
}

void process_processor(const std::string& uuid, const attribute::Attributes& attributes,
                   SetComponentAttributes::Response& response) {

    CommonValidator::validate_set_processor_attributes(attributes);
    const auto& attribute_names = attributes.get_names();
    if (attribute_names.empty()) {
        log_debug("pnc-gami", "setComponentAttributes: nothing has been changed (empty request).");
        return;
    }
    Toolset tools = Toolset::get();
    for (const auto& name : attribute_names) {
        const auto& value = attributes.get_value(name);

        try {
            if (literals::Fpga::SECURELY_ERASE == name) {
                securely_erase<Processor>(uuid, response);
            }
            else if (literals::Fpga::ERASED == name) {
                tools.model_tool->update_device_erased<Processor>(uuid, value.get<bool>());
                log_debug("pnc-gami", "Set " + name + " attribute to " << std::boolalpha << value.get<bool>());
            }
            else if (literals::Processor::OEM == name) {
                THROW(UnsupportedField, "pnc-gami", "Setting attribute is not supported.", name, value);
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
    const auto& attribute_names = attributes.get_names();
    if (attribute_names.empty()) {
        log_debug("pnc-gami", "setComponentAttributes: nothing has been changed (empty request).");
        return;
    }
    for (const auto& name : attribute_names) {
        const auto& value = attributes.get_value(name);
        try {
            if (literals::Chassis::ASSET_TAG == name) {
                set_asset_tag<Chassis>(uuid, value);
            }
            else {
                // The response must have a message for every attribute that could not be set.
                THROW(UnsupportedField, "pnc-gami", "Setting attribute is not supported.", name, value);
            }
        }
        catch (const GamiException& ex) {
            response.add_status({name, ex.get_error_code(), ex.get_message()});
        }
    }
}


void process_system(const std::string& uuid, const attribute::Attributes& attributes,
                    SetComponentAttributes::Response& response) {

    CommonValidator::validate_set_system_attributes(attributes);
    const auto& attribute_names = attributes.get_names();
    if (attribute_names.empty()) {
        log_debug("pnc-gami", "setComponentAttributes: nothing has been changed (empty request).");
        return;
    }
    for (const auto& name : attribute_names) {
        const auto& value = attributes.get_value(name);
        try {
            if (literals::System::ASSET_TAG == name) {
                set_asset_tag<System>(uuid, value);
            }
            else if ((literals::System::BOOT_OVERRIDE == name) ||
                     (literals::System::BOOT_OVERRIDE_MODE == name) ||
                     (literals::System::BOOT_OVERRIDE_TARGET == name)) {
                THROW(UnsupportedField, "pnc-gami", "Setting attribute is not supported in PNC.", name, value);
            }
            else {
                // The response must have a message for every attribute that could not be set.
                THROW(UnsupportedField, "pnc-gami", "Setting attribute is not supported.", name, value);
            }
        }
        catch (const GamiException& ex) {
            response.add_status({name, ex.get_error_code(), ex.get_message()});
        }
    }
}


void set_no_attributes(const attribute::Attributes& attributes, SetComponentAttributes::Response& response) {
    const auto attribute_names = attributes.get_names();
    if (attribute_names.empty()) {
        log_debug("pnc-gami", "setComponentAttributes: nothing has been changed (empty request).");
        return;
    }
    for (const auto& name : attribute_names) {
        try {
            // The response must have a message for every attribute that could not be set.
            const auto& field_value = attributes.get_value(name);
            THROW(agent_framework::exceptions::UnsupportedField, "pnc-gami",
                  "Setting attribute is not supported.", name, field_value);
        }
        catch (const GamiException& ex) {
            response.add_status({name, ex.get_error_code(), ex.get_message()});
        }
    }
}


void set_endpoint_attributes(const attribute::Attributes& attributes, SetComponentAttributes::Response& response) {
    CommonValidator::validate_set_endpoint_attributes(attributes);
    set_no_attributes(attributes, response);
}


bool exists_in_pnc(const std::string& uuid) {
    return get_manager<Manager>().entry_exists(uuid) ||
           get_manager<PcieFunction>().entry_exists(uuid) ||
           get_manager<Zone>().entry_exists(uuid);
}


void set_component_attributes(const SetComponentAttributes::Request& request,
                              SetComponentAttributes::Response& response) {
    log_info("pnc-gami", "Executing setComponentAttributes.");

    const auto& uuid = request.get_component();
    const auto& attributes = request.get_attributes();

    if (attributes.get_names().empty()) {
        log_debug("pnc-gami", "setComponentAttributes: nothing has been changed (empty request).");
        return;
    }

    if (get_manager<Drive>().entry_exists(uuid)) {
        process_drive(uuid, attributes, response);
    }
    else if (get_manager<Processor>().entry_exists(uuid)) {
        process_processor(uuid, attributes, response);
    }
    else if (get_manager<Chassis>().entry_exists(uuid)) {
        process_chassis(uuid, attributes, response);
    }
    else if (get_manager<System>().entry_exists(uuid)) {
        process_system(uuid, attributes, response);
    }
    else if (get_manager<PcieDevice>().entry_exists(uuid)) {
        process_pcie_device(uuid, attributes, response);
    }
    else if (get_manager<Switch>().entry_exists(uuid)) {
        process_pcie_switch(uuid, attributes, response);
    }
    else if (get_manager<Port>().entry_exists(uuid)) {
        process_pcie_port(uuid, attributes, response);
    }
    else if (get_manager<Endpoint>().entry_exists(uuid)) {
        log_debug("pnc-gami", "Setting Endpoint attributes isn't supported [ uuid=" << uuid << " ]");
        set_endpoint_attributes(attributes, response);
    }
    else if (exists_in_pnc(uuid)) {
        THROW(InvalidValue, "pnc-gami", "Operation not available for this component.");
    }
    else {
        THROW(InvalidUuid, "pnc-gami", "No component with UUID = '" + uuid + "'.");
    }

    log_info("pnc-gami", "setComponentAttributes finished successfully.");
}

}

REGISTER_COMMAND(SetComponentAttributes, set_component_attributes);
