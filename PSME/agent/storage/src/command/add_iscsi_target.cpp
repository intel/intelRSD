/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 * */

#include "agent-framework/module/storage_components.hpp"
#include "agent-framework/command-ref/registry.hpp"
#include "agent-framework/command-ref/storage_commands.hpp"

#include "iscsi/manager.hpp"
#include "iscsi/response.hpp"
#include "iscsi/tgt/config/tgt_config.hpp"
#include "event/storage_event.hpp"
#include "iscsi/utils.hpp"
#include "tree_stability/storage_tree_stabilizer.hpp"



using namespace agent_framework::command_ref;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;
using namespace agent_framework::model::attribute;
using namespace agent_framework::module;
using namespace agent_framework::module::managers;
using namespace agent::storage::iscsi::tgt::config;

using agent::storage::StorageTreeStabilizer;

namespace {

struct TargetAuthAttributes {
    OptionalField<enums::TargetAuthenticationMethod> AuthenticationMethod{};
    OptionalField<std::string> ChapUsername{};
    OptionalField<std::string> ChapSecret{};
    OptionalField<std::string> MutualChapUsername{};
    OptionalField<std::string> MutualChapSecret{};
};

static const constexpr char EMPTY_VALUE[] = "";
constexpr const char* TGT_INITIATOR_ADDRESS_ALL = "ALL";
using Errors = agent::storage::iscsi::tgt::Errors;

void get_target_chap_attributes(const AddIscsiTarget::Request& request,
                                TargetAuthAttributes& attributes) {

    attributes.AuthenticationMethod = OptionalField<enums::TargetAuthenticationMethod>(request.get_authentication_method());
    attributes.ChapUsername = request.get_chap_username();
    attributes.ChapSecret = request.get_chap_secret();
    attributes.MutualChapUsername = request.get_mutual_chap_username();
    attributes.MutualChapSecret = request.get_mutual_chap_secret();
}


void create_target(agent::storage::iscsi::tgt::Manager& manager,
                   const std::int32_t target_id,
                   const std::string& target_iqn) {
    auto target_res = manager.create_target(target_id, target_iqn);
    if (!target_res.is_valid()) {
        Errors::throw_exception(target_res.get_error());
    }
}


void delete_target(agent::storage::iscsi::tgt::Manager& manager,
                   const std::int32_t target_id) {
    auto response = manager.destroy_target(target_id);

    if (!response.is_valid()) {
        Errors::throw_exception(response.get_error(), "Destroy target error: ");
    }
}


void set_initiator_name(agent::storage::iscsi::tgt::Manager& manager,
                        const std::int32_t target_id,
                        const std::string& initiator_name) {
    if (initiator_name.empty()) {
        log_debug(GET_LOGGER("storage-agent"), "No target initiator-name set.");
        return;
    }
    agent::storage::iscsi::tgt::Manager::OptionMapper options;
    options.emplace(std::make_pair("initiator-name", initiator_name));
    auto response = manager.bind_target(target_id, options);
    if (!response.is_valid()) {
        Errors::throw_exception(response.get_error(),
                                "Cannot bind initiator-name to target: ");
    }
}


void set_initiator_address(agent::storage::iscsi::tgt::Manager& manager,
                           const std::int32_t target_id,
                           const std::string& initiator_address) {
    if (initiator_address.empty()) {
        log_debug(GET_LOGGER("storage-agent"), "No target initiator set.");
        return;
    }
    agent::storage::iscsi::tgt::Manager::OptionMapper options;
    options.emplace(std::make_pair("initiator-address", initiator_address));
    auto response = manager.bind_target(target_id, options);
    if (!response.is_valid()) {
        Errors::throw_exception(response.get_error(),
                                "Cannot bind initiator-address to target: ");
    }
}


void create_luns(agent::storage::iscsi::tgt::Manager& manager,
                 IscsiTarget& target, int target_id,
                 const AddIscsiTarget::Request& request) {
    for (const auto& target_lun : request.get_target_luns()) {
        const auto drive = get_manager<LogicalDrive>().
            get_entry(target_lun.get_logical_drive());

        auto lun_res = manager.create_lun(target_id,
                                          target_lun.get_lun(),
                                          drive.get_device_path());
        if (!lun_res.is_valid()) {
            Errors::throw_exception(lun_res.get_error(), "Create lun error: ");
        }
        target.add_target_lun({target_lun.get_lun(), drive.get_uuid()});
    }
}


void validate_chap_data(const TargetAuthAttributes& target_auth_attributes) {
    if (!target_auth_attributes.AuthenticationMethod.has_value()) {
        // Check if CHAP account parameters specified
        if ((target_auth_attributes.ChapUsername.has_value() &&
             target_auth_attributes.ChapUsername.value() != EMPTY_VALUE) ||
            (target_auth_attributes.ChapSecret.has_value() &&
             target_auth_attributes.ChapSecret.value() != EMPTY_VALUE)) {
            THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
                  "CHAP username/secret parameters must be skipped or set to empty/null.");
        }

        // Check if Mutual CHAP account parameters specified
        if ((target_auth_attributes.MutualChapUsername.has_value() &&
             target_auth_attributes.MutualChapUsername.value() != EMPTY_VALUE) ||
            (target_auth_attributes.MutualChapSecret.has_value() &&
             target_auth_attributes.MutualChapSecret.value() != EMPTY_VALUE)) {
            THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
                  "Mutual CHAP username/secret parameters must be skipped or set to empty/null.");
        }

    } else if (enums::TargetAuthenticationMethod::OneWay == target_auth_attributes.AuthenticationMethod) {
        // Check if CHAP account parameters specified
        if (!target_auth_attributes.ChapUsername.has_value() ||
            target_auth_attributes.ChapUsername.value() == EMPTY_VALUE ||
            !target_auth_attributes.ChapSecret.has_value()) {
            THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
                  "CHAP username parameter must be specified and set to string to add new credentials.");
        }

        // Check if Mutual CHAP account parameters specified
        if ((target_auth_attributes.MutualChapUsername.has_value() &&
             target_auth_attributes.MutualChapUsername.value() != EMPTY_VALUE) ||
            (target_auth_attributes.MutualChapSecret.has_value() &&
             target_auth_attributes.MutualChapSecret.value() != EMPTY_VALUE)) {
            THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
                  "Mutual CHAP username/secret parameters must be skipped or set to empty/null.");
        }

    } else if (enums::TargetAuthenticationMethod::Mutual == target_auth_attributes.AuthenticationMethod) {
        // Check if CHAP account parameters specified
        if (!target_auth_attributes.ChapUsername.has_value() ||
            target_auth_attributes.ChapUsername.value() == EMPTY_VALUE ||
            !target_auth_attributes.ChapSecret.has_value()) {
            THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
                  "CHAP username parameter must be specified and set to string to add new credentials.");
        }

        // Check if CHAP account parameters specified
        if (!target_auth_attributes.MutualChapUsername.has_value() ||
            target_auth_attributes.MutualChapUsername.value() == EMPTY_VALUE ||
            !target_auth_attributes.MutualChapSecret.has_value()) {
            THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
                  "Mutual CHAP username parameter must be specified and set to string to add new credentials.");
        }
    }
}


void add_iscsi_target(const AddIscsiTarget::Request& request, AddIscsiTarget::Response& response) {
    TargetAuthAttributes target_auth_attributes;
    // read target chap attributes form request
    get_target_chap_attributes(request, target_auth_attributes);
    validate_chap_data(target_auth_attributes);

    IscsiTarget target{};
    const auto& iscsi_data = get_manager<IscsiTarget, IscsiTargetManager>().get_iscsi_data();
    const std::string target_uuid = target.get_uuid();
    const std::string target_iqn = request.get_target_iqn();

    target.set_initiator_iqn(request.get_initiator_iqn());
    target.set_target_iqn(request.get_target_iqn());
    target.set_target_address(iscsi_data.get_portal_ip());
    target.set_target_port(std::to_string(iscsi_data.get_portal_port()));
    target.set_status({State::Enabled, Health::OK});
    const auto target_id = get_manager<IscsiTarget, IscsiTargetManager>().
        create_target_id();
    target.set_target_id(target_id);
    agent::storage::iscsi::tgt::Manager manager{};
    create_target(manager, int(target_id), request.get_target_iqn());
    set_initiator_name(manager, int(target_id),
                       request.get_initiator_iqn().has_value() ? request.get_initiator_iqn().value() : EMPTY_VALUE);
    set_initiator_address(manager, int(target_id), TGT_INITIATOR_ADDRESS_ALL);

    try {
        create_luns(manager, target, int(target_id), request);
    }
    catch (...) {
        delete_target(manager, int(target_id));
        throw;
    }

    TgtConfig tgtConfig(iscsi_data.get_configuration_path());
    try {
        tgtConfig.add_target(target);
    }
    catch (const std::exception& ex) {
        log_warning(GET_LOGGER("storage-agent"),
                    "Unable to create TGT target config file: " << ex.what());
    }

    if(target_auth_attributes.AuthenticationMethod.has_value()) {
        if (enums::TargetAuthenticationMethod::OneWay == target_auth_attributes.AuthenticationMethod) {
            try {
                add_chap_account_to_target(manager, target_id,
                                           target_auth_attributes.ChapUsername,
                                           target_auth_attributes.ChapSecret, false);
            }
            catch (...) {
                delete_target(manager, int(target_id));
                throw;
            }
            target.set_chap_username(target_auth_attributes.ChapUsername);

        }
        else if (enums::TargetAuthenticationMethod::Mutual == target_auth_attributes.AuthenticationMethod) {
            try {
                add_chap_account_to_target(manager, target_id,
                                           target_auth_attributes.ChapUsername,
                                           target_auth_attributes.ChapSecret, false);
            }
            catch (...) {
                delete_target(manager, int(target_id));
                throw;
            }
            target.set_chap_username(target_auth_attributes.ChapUsername);

            try {
                add_chap_account_to_target(manager, target_id,
                                           target_auth_attributes.MutualChapUsername,
                                           target_auth_attributes.MutualChapSecret, true);
            }
            catch (...) {
                delete_chap_account(manager, target_auth_attributes.ChapUsername);
                delete_target(manager, int(target_id));
                throw;
            }
            target.set_mutual_chap_username(target_auth_attributes.MutualChapUsername);
        }
    }
    else {
        // no action is required. validate_chap_data(target_auth_attributes) already makes sure that
        // usernames and passwords are empty if authentication_method is null
    }

    if (target_auth_attributes.AuthenticationMethod.has_value()) {
        target.set_authentication_method(target_auth_attributes.AuthenticationMethod.value().to_string());
    }

    get_manager<IscsiTarget, IscsiTargetManager>().
        add_target_id(target_id, target.get_uuid());
    get_manager<IscsiTarget>().add_entry(std::move(target));

    const std::string& target_persistent_uuid = StorageTreeStabilizer().stabilize_target(target_uuid);
    response.set_target(target_persistent_uuid);
    log_info(GET_LOGGER("storage-agent"), "iSCSI Target added: " << target_iqn);
}

}

REGISTER_COMMAND(AddIscsiTarget, ::add_iscsi_target);
