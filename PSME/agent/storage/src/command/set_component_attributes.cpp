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
 * */


#include "agent-framework/module/storage_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/command/registry.hpp"
#include "agent-framework/command/storage_commands.hpp"
#include "agent-framework/module/requests/validation/storage.hpp"
#include "iscsi/errors.hpp"
#include "iscsi/manager.hpp"
#include "iscsi/response.hpp"
#include "iscsi/utils.hpp"
#include "iscsi/tgt/config/tgt_config.hpp"
#include "lvm/lvm_api.hpp"
#include "common/utils.hpp"
#include "lvm/lvm_attribute.hpp"



using namespace agent_framework;
using namespace agent_framework::command;
using namespace agent_framework::model;
using namespace agent_framework::model::attribute;
using namespace agent_framework::model::requests::validation;
using namespace agent_framework::module;
using namespace agent_framework::module::managers;
using namespace agent::storage::iscsi::tgt;
using namespace agent::storage::iscsi::tgt::config;
using namespace agent::storage::utils;
using Errors = agent::storage::iscsi::tgt::Errors;

namespace {

struct ConfigurableiSCSITargetAttributes {
    bool InitiatorIQNGiven{false};
    OptionalField<std::string> InitiatorIQN{};
    bool AuthenticationMethodGiven{false};
    OptionalField<enums::TargetAuthenticationMethod> AuthenticationMethod{};
    OptionalField<std::string> ChapUsername{};
    OptionalField<std::string> ChapSecret{};
    OptionalField<std::string> MutualChapUsername{};
    OptionalField<std::string> MutualChapSecret{};
};

static const constexpr char EMPTY_VALUE[] = "";


void get_command_attributes(const attribute::Attributes& attributes,
                            ConfigurableiSCSITargetAttributes& attributes_to_configure) {

    const auto attributes_names = attributes.get_names();
    for (const auto& attribute_name : attributes_names) {
        const auto& value = attributes.get_value(attribute_name);

        log_debug(GET_LOGGER("storage-agent"), "Attribute name: " << attribute_name);
        log_debug(GET_LOGGER("storage-agent"), "Attribute value: " << value.dump());

        if (literals::IscsiTarget::INITIATOR_IQN == attribute_name) {
            attributes_to_configure.InitiatorIQNGiven = true;
            attributes_to_configure.InitiatorIQN = OptionalField<std::string>(value);
        }
        else if (literals::IscsiTarget::AUTHENTICATION_METHOD == attribute_name) {
            attributes_to_configure.AuthenticationMethodGiven = true;
            attributes_to_configure.AuthenticationMethod = OptionalField<enums::TargetAuthenticationMethod>(value);
        }
        else if (literals::IscsiTarget::CHAP_USERNAME == attribute_name) {
            attributes_to_configure.ChapUsername = OptionalField<std::string>(value.is_null() ? EMPTY_VALUE : value);
        }
        else if (literals::IscsiTarget::CHAP_SECRET == attribute_name) {
            attributes_to_configure.ChapSecret = OptionalField<std::string>(value.is_null() ? EMPTY_VALUE : value);
        }
        else if (literals::IscsiTarget::MUTUAL_CHAP_USERNAME == attribute_name) {
            attributes_to_configure.MutualChapUsername = OptionalField<std::string>(
                value.is_null() ? EMPTY_VALUE : value);
        }
        else if (literals::IscsiTarget::MUTUAL_CHAP_SECRET == attribute_name) {
            attributes_to_configure.MutualChapSecret = OptionalField<std::string>(value.is_null() ? EMPTY_VALUE : value);
        }
        else {
            log_warning(GET_LOGGER("storage-agent"), "Unsupported attribute: " << attribute_name << ".");
        }
    }
}


void update_tgt_config_file(const IscsiTarget& target) {

    const auto& iscsi_data = get_manager<IscsiTarget, IscsiTargetManager>().get_iscsi_data();
    TgtConfig tgtConfig(iscsi_data.get_configuration_path());

    try {
        tgtConfig.add_target(target);
    }
    catch (const std::exception& ex) {
        log_warning(GET_LOGGER("storage-agent"),
                    "Unable to update TGT target config file: " << ex.what());
    }
}


void update_initiator_name(agent::storage::iscsi::tgt::Manager& manager, const std::int32_t target_id,
                           const OptionalField<std::string>& initiator_name_old,
                           const OptionalField<std::string>& initiator_name_new) {

    if (initiator_name_old.has_value()) {
        agent::storage::iscsi::tgt::Manager::OptionMapper options_old;
        options_old.emplace(std::make_pair("initiator-name", initiator_name_old));
        auto response_old = manager.unbind_target(target_id, options_old);
        if (!response_old.is_valid()) {
            Errors::throw_exception(response_old.get_error(),
                                    "Cannot unbind initiator-address from target: ");
        }
    }

    if (initiator_name_new.has_value()) {
        agent::storage::iscsi::tgt::Manager::OptionMapper options_new;
        options_new.emplace(std::make_pair("initiator-name", initiator_name_new));
        auto response_new = manager.bind_target(target_id, options_new);
        if (!response_new.is_valid()) {
            Errors::throw_exception(response_new.get_error(),
                                    "Cannot bind initiator-address to target: ");
        }
    }
}


void process_iscsi_target_intiator_iqn(const std::string& uuid,
                                       const ConfigurableiSCSITargetAttributes& attributes_to_configure,
                                       SetComponentAttributes::Response& response) {

    auto target = get_manager<IscsiTarget>().get_entry_reference(uuid);
    auto target_id = target->get_target_id();

    agent::storage::iscsi::tgt::Manager manager{};

    try {
        // get current values
        auto prev_initiator_iqn = target->get_initiator_iqn();

        update_initiator_name(manager, target_id, prev_initiator_iqn, attributes_to_configure.InitiatorIQN);

        target->set_initiator_iqn(attributes_to_configure.InitiatorIQN);

        update_tgt_config_file(get_manager<IscsiTarget>().get_entry(uuid));
    }
    catch (const exceptions::GamiException& ex) {
        response.add_status({literals::IscsiTarget::INITIATOR_IQN, ex.get_error_code(), ex.get_message()});
    }
}


void process_iscsi_target_auth_null(const std::string& uuid,
                                    const ConfigurableiSCSITargetAttributes& attributes_to_configure,
                                    SetComponentAttributes::Response& response) {

    auto target = get_manager<IscsiTarget>().get_entry_reference(uuid);

    auto prev_chap_username = target->get_chap_username();
    auto prev_mutual_chap_username = target->get_mutual_chap_username();

    agent::storage::iscsi::tgt::Manager manager{};

    if (!attributes_to_configure.ChapUsername.has_value() &&
        (prev_chap_username.has_value() && prev_chap_username.value() != EMPTY_VALUE)) {
        THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
              "CHAP username parameter must be specified and set to empty/null to erase defined credentials.");
    }

    // Check if CHAP account parameters specified
    if (attributes_to_configure.ChapUsername.has_value() && ((attributes_to_configure.ChapUsername != EMPTY_VALUE) ||
                                                             (attributes_to_configure.ChapSecret != EMPTY_VALUE))) {
        if (prev_chap_username.has_value() && prev_chap_username.value() != EMPTY_VALUE) {
            THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
                  "CHAP username and password parameters must be set to empty/null to erase defined credentials.");
        }
        else {
            THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
                  "CHAP username and password parameters must be skipped or set to empty/null.");
        }
    }

    if (!attributes_to_configure.MutualChapUsername.has_value() &&
        (prev_mutual_chap_username.has_value() && prev_mutual_chap_username.value() != EMPTY_VALUE)) {
        THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
              "Mutual CHAP username parameter must be specified and set to empty/null to erase defined credentials.");
    }

    // Check if Mutual CHAP account parameters specified
    if (attributes_to_configure.MutualChapUsername.has_value() &&
        ((attributes_to_configure.MutualChapUsername != EMPTY_VALUE) ||
         (attributes_to_configure.MutualChapSecret != EMPTY_VALUE))
        ) {
        if (prev_mutual_chap_username.has_value() && prev_mutual_chap_username.value() != EMPTY_VALUE) {
            THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
                  "Mutual CHAP username and password parameters must be set to empty/null to erase defined credentials.");
        }
        else {
            THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
                  "Mutual CHAP username and password parameters must be skipped or set to empty/null.");
        }
    }

    // Delete CHAP account if exist
    if (prev_chap_username.has_value() && prev_chap_username.value() != EMPTY_VALUE) {
        try {
            delete_chap_account(manager, prev_chap_username.value());
        }
        catch (const exceptions::GamiException& ex) {
            response.add_status({literals::IscsiTarget::CHAP_USERNAME, ex.get_error_code(), ex.get_message()});
            return;
        }
        target->set_chap_username(OptionalField<std::string>());
    }

    // Delete Mutual CHAP account if exist
    if (prev_mutual_chap_username.has_value() && prev_mutual_chap_username.value() != EMPTY_VALUE) {
        try {
            delete_chap_account(manager, prev_mutual_chap_username.value());
        }
        catch (const exceptions::GamiException& ex) {
            response.add_status(
                {literals::IscsiTarget::MUTUAL_CHAP_USERNAME, ex.get_error_code(), ex.get_message()});
            return;
        }
        target->set_mutual_chap_username(OptionalField<std::string>());
    }

    target->set_authentication_method(OptionalField<std::string>());
}


void process_iscsi_target_auth_chap(const std::string& uuid,
                                    const ConfigurableiSCSITargetAttributes& attributes_to_configure,
                                    SetComponentAttributes::Response& response) {

    auto target = get_manager<IscsiTarget>().get_entry_reference(uuid);
    auto target_id = target->get_target_id();

    auto prev_chap_username = target->get_chap_username();
    auto prev_mutual_chap_username = target->get_mutual_chap_username();

    agent::storage::iscsi::tgt::Manager manager{};

    if (!attributes_to_configure.ChapUsername.has_value()) {
        if (!prev_chap_username.has_value() || prev_chap_username.value() == EMPTY_VALUE) {
            THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
                  "CHAP username parameter must be specified and set to string to add new credentials.");
        }
    }

    // Check if CHAP account parameters specified
    if (attributes_to_configure.ChapUsername.has_value() &&
        attributes_to_configure.ChapUsername.value() == EMPTY_VALUE) {
        THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
              "CHAP username parameter must be set to string to add new or update defined credentials.");
    }

    if (!attributes_to_configure.MutualChapUsername.has_value() &&
        (prev_mutual_chap_username.has_value() && prev_mutual_chap_username.value() != EMPTY_VALUE)) {
        THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
              "Mutual CHAP username parameter must be specified and set to empty/null to erase defined credentials.");
    }

    // Check if Mutual CHAP account parameters specified
    if (attributes_to_configure.MutualChapUsername.has_value() &&
        ((attributes_to_configure.MutualChapUsername != EMPTY_VALUE) ||
         (attributes_to_configure.MutualChapSecret != EMPTY_VALUE))
        ) {
        if (prev_mutual_chap_username.has_value() && prev_mutual_chap_username.value() != EMPTY_VALUE) {
            THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
                  "Mutual CHAP username and password parameters must be set to empty/null to erase defined credentials.");
        }
        else {
            THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
                  "Mutual CHAP username and password parameters must be skipped or set to empty/null.");
        }
    }

    // Add/update CHAP account if specified
    if (attributes_to_configure.ChapUsername.has_value()) {
        try {
            if (prev_chap_username.has_value() && prev_chap_username.value() != EMPTY_VALUE) {
                if (attributes_to_configure.ChapUsername.value() != prev_chap_username.value()) {
                    replace_chap_account_in_target(manager, target_id, prev_chap_username.value(),
                                                   attributes_to_configure.ChapUsername,
                                                   attributes_to_configure.ChapSecret, false);
                }
                else {
                    update_password_for_chap_account(manager, target_id, attributes_to_configure.ChapUsername,
                                                     attributes_to_configure.ChapSecret, false);
                }
            }
            else {
                add_chap_account_to_target(manager, target_id,
                                           attributes_to_configure.ChapUsername,
                                           attributes_to_configure.ChapSecret, false);
            }
        }
        catch (const exceptions::GamiException& ex) {
            response.add_status(
                {literals::IscsiTarget::CHAP_USERNAME, ex.get_error_code(), ex.get_message()});
            return;
        }
        target->set_chap_username(attributes_to_configure.ChapUsername);
    }

    // Delete Mutual CHAP account if exist
    if (prev_mutual_chap_username.has_value() && prev_mutual_chap_username.value() != EMPTY_VALUE) {
        try {
            delete_chap_account(manager, prev_mutual_chap_username.value());
        }
        catch (const exceptions::GamiException& ex) {
            response.add_status(
                {literals::IscsiTarget::MUTUAL_CHAP_USERNAME, ex.get_error_code(), ex.get_message()});
            return;
        }
        target->set_mutual_chap_username(OptionalField<std::string>());
    }

    target->set_authentication_method(attributes_to_configure.AuthenticationMethod.value().to_string());
}


void process_iscsi_target_auth_mutual_chap(const std::string& uuid,
                                           const ConfigurableiSCSITargetAttributes& attributes_to_configure,
                                           SetComponentAttributes::Response& response) {

    auto target = get_manager<IscsiTarget>().get_entry_reference(uuid);
    auto target_id = target->get_target_id();

    auto prev_chap_username = target->get_chap_username();
    auto prev_mutual_chap_username = target->get_mutual_chap_username();

    agent::storage::iscsi::tgt::Manager manager{};

    // Check if CHAP account parameters specified
    if (attributes_to_configure.ChapUsername.has_value() &&
        attributes_to_configure.ChapUsername == EMPTY_VALUE) {
        if (prev_chap_username.has_value() && prev_chap_username.value() != EMPTY_VALUE) {
            THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
                  "CHAP username parameter must be set to string to update defined credentials.");
        }
        else {
            THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
                  "CHAP username parameter must be set to string to add new credentials.");
        }
    } else if (!attributes_to_configure.ChapUsername.has_value() &&
        (!prev_chap_username.has_value() || prev_chap_username.value() == EMPTY_VALUE)) {
        THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
              "CHAP username parameter must be set to string to add new credentials.");
    }

    // Check if Mutual CHAP account parameters specified
    if (attributes_to_configure.MutualChapUsername.has_value() &&
        attributes_to_configure.MutualChapUsername == EMPTY_VALUE) {
        if (prev_mutual_chap_username.has_value() && prev_mutual_chap_username.value() != EMPTY_VALUE) {
            THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
                  "Mutual CHAP username parameter must be set to string to update defined credentials.");
        }
        else {
            THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
                  "Mutual CHAP username parameter must be set to string to add new credentials.");
        }
    } else if (!attributes_to_configure.MutualChapUsername.has_value() &&
        (!prev_mutual_chap_username.has_value() || prev_mutual_chap_username.value() == EMPTY_VALUE)) {
        THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
              "Mutual CHAP username parameter must be set to string to add new credentials.");
    }

    // Add a new CHAP account if specified
    if (attributes_to_configure.ChapUsername.has_value()) {
        try {
            if (prev_chap_username.has_value() && prev_chap_username.value() != EMPTY_VALUE) {
                if (attributes_to_configure.ChapUsername.value() != prev_chap_username.value()) {
                    replace_chap_account_in_target(manager, target_id, prev_chap_username.value(),
                                                   attributes_to_configure.ChapUsername,
                                                   attributes_to_configure.ChapSecret, false);
                }
                else {
                    update_password_for_chap_account(manager, target_id, attributes_to_configure.ChapUsername,
                                                     attributes_to_configure.ChapSecret, false);
                }
            }
            else {
                add_chap_account_to_target(manager, target_id,
                                           attributes_to_configure.ChapUsername,
                                           attributes_to_configure.ChapSecret, false);
            }
        }
        catch (const exceptions::GamiException& ex) {
            response.add_status(
                {literals::IscsiTarget::CHAP_USERNAME, ex.get_error_code(), ex.get_message()});
            return;
        }
        target->set_chap_username(attributes_to_configure.ChapUsername);
    }

    // Add/update Mutual CHAP account if specified
    if (attributes_to_configure.MutualChapUsername.has_value()) {
        try {
            if (prev_mutual_chap_username.has_value() && prev_mutual_chap_username.value() != EMPTY_VALUE) {
                if (attributes_to_configure.MutualChapUsername.value() != prev_mutual_chap_username.value()) {
                    replace_chap_account_in_target(manager, target_id, prev_mutual_chap_username.value(),
                                                   attributes_to_configure.MutualChapUsername,
                                                   attributes_to_configure.MutualChapSecret, true);
                }
                else {
                    update_password_for_chap_account(manager, target_id, attributes_to_configure.MutualChapUsername,
                                                     attributes_to_configure.MutualChapSecret, true);
                }
            }
            else {
                add_chap_account_to_target(manager, target_id,
                                           attributes_to_configure.MutualChapUsername,
                                           attributes_to_configure.MutualChapSecret, true);
            }
        }
        catch (const exceptions::GamiException& ex) {
            response.add_status(
                {literals::IscsiTarget::MUTUAL_CHAP_USERNAME, ex.get_error_code(), ex.get_message()});
            return;
        }
        target->set_mutual_chap_username(attributes_to_configure.MutualChapUsername);
    }

    target->set_authentication_method(attributes_to_configure.AuthenticationMethod.value().to_string());
}


void process_iscsi_target(const std::string& uuid, const Attributes& attributes,
                          SetComponentAttributes::Response& response) {

    ConfigurableiSCSITargetAttributes attributes_to_configure;
    StorageValidator::validate_set_iscsi_target_attributes(attributes);
    if (attributes.empty()) {
        log_info(GET_LOGGER("storage-agent"), "Nothing has been changed (empty request).");
        return;
    }

    // read parameters from command
    get_command_attributes(attributes, attributes_to_configure);

    if (attributes_to_configure.InitiatorIQNGiven) {
        process_iscsi_target_intiator_iqn(uuid, attributes_to_configure, response);
    }

    if (attributes_to_configure.AuthenticationMethodGiven) {

        if (attributes_to_configure.ChapUsername.has_value() !=
            attributes_to_configure.ChapSecret.has_value()) {
            THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
                  "Both CHAP username and secret must be specified.");
        }

        if (attributes_to_configure.MutualChapUsername.has_value() !=
            attributes_to_configure.MutualChapSecret.has_value()) {
            THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
                  "Both Mutual CHAP username and secret must be specified.");
        }

        if (attributes_to_configure.ChapUsername.has_value() && attributes_to_configure.MutualChapSecret.has_value()) {
            if (attributes_to_configure.ChapUsername.value() == attributes_to_configure.MutualChapUsername.value() &&
                attributes_to_configure.ChapUsername.value() != EMPTY_VALUE) {
                THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
                      "CHAP usernames must be unique.");
            }
        }

        if (!attributes_to_configure.AuthenticationMethod.has_value()) {
            process_iscsi_target_auth_null(uuid, attributes_to_configure, response);

        }
        else if (enums::TargetAuthenticationMethod::OneWay == attributes_to_configure.AuthenticationMethod) {
            process_iscsi_target_auth_chap(uuid, attributes_to_configure, response);

        }
        else if (enums::TargetAuthenticationMethod::Mutual == attributes_to_configure.AuthenticationMethod) {
            process_iscsi_target_auth_mutual_chap(uuid, attributes_to_configure, response);
        }
    }
}


void process_bootable_tag(const std::string& uuid, const bool value) {
    auto drive = get_manager<LogicalDrive>().get_entry_reference(uuid);
    if (agent_framework::model::enums::LogicalDriveMode::LV != drive->get_mode()) {
        THROW(agent_framework::exceptions::InvalidValue,
              "storage-agent", "Only logical volume can be tagged!");
    }
    drive->set_bootable(value);
    agent::storage::lvm::LvmAPI lvm_api;
    const auto vg_name = get_name_from_device_path(
            (get_manager<LogicalDrive>().get_entry(drive->get_parent_uuid())).get_device_path());
    const auto lv_name = get_name_from_device_path(drive->get_device_path());
    if (value) {
        lvm_api.add_logical_volume_tag(vg_name,lv_name,
                                       agent::storage::lvm::attribute::LV_BOOTABLE_ATTR);
    }
    else {
        lvm_api.remove_logical_volume_tag(vg_name,lv_name,
                                          agent::storage::lvm::attribute::LV_BOOTABLE_ATTR);
    }
    log_debug(GET_LOGGER("storage-agent"),
              "Set 'bootable' attribute to " << std::boolalpha << value << ".");
}


void process_logical_drive(const std::string& uuid, const Attributes& attributes,
                           SetComponentAttributes::Response& response) {

    StorageValidator::validate_set_logical_drive_attributes(attributes);
    if (attributes.empty()) {
        log_info(GET_LOGGER("storage-agent"), "Nothing has been changed (empty request).");
        return;
    }
    const auto attributes_names = attributes.get_names();
    for (const auto& attribute_name : attributes_names) {
        const auto& field_value = attributes.get_value(attribute_name);
        try {
            if (literals::LogicalDrive::BOOTABLE == attribute_name) {
                const auto value = attributes.get_value(attribute_name).get<bool>();
                process_bootable_tag(uuid, value);
            }
            else if (literals::LogicalDrive::OEM == attribute_name) {
                THROW(exceptions::UnsupportedField, "storage-agent", "Setting attribute is not supported.",
                      attribute_name, field_value);
            }
        }
        catch (const exceptions::GamiException& ex) {
            response.add_status({attribute_name, ex.get_error_code(), ex.get_message()});
        }
    }
}


bool exists_in_storage_components(const std::string& uuid) {
    const auto& common_components = CommonComponents::get_instance();
    const auto& storage_components = StorageComponents::get_instance();

    return common_components->get_module_manager().entry_exists(uuid)
           || storage_components->get_storage_service_manager().entry_exists(uuid)
           || storage_components->get_physical_drive_manager().entry_exists(uuid);
}


void set_component_attributes(const SetComponentAttributes::Request& request,
                              SetComponentAttributes::Response& response) {

    log_debug(GET_LOGGER("storage-agent"), "Trying to execute SetComponentAttributes method.");
    const auto& uuid = request.get_component();
    const auto& attributes = request.get_attributes();

    if (StorageComponents::get_instance()->get_iscsi_target_manager().entry_exists(uuid)) {
        process_iscsi_target(uuid, attributes, response);
    }
    else if (StorageComponents::get_instance()->get_logical_drive_manager().entry_exists(uuid)) {
        process_logical_drive(uuid, attributes, response);
    }
    else if (exists_in_storage_components(uuid)) {
        THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
              "Operation not available for this component.");
    }
    else {
        THROW(agent_framework::exceptions::InvalidUuid, "storage-agent",
              "No component with UUID = '" + uuid + "'.");
    }

    log_info(GET_LOGGER("storage-agent"), "setComponentAttributes finished successfully.");
}

}

REGISTER_COMMAND(SetComponentAttributes, ::set_component_attributes);
