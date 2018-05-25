/*!
 * @copyright
 * Copyright (c) 2017-2018 Intel Corporation
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
 * @file utils.cpp
 * @brief tgt utils
*/

#include "iscsi/errors.hpp"
#include "iscsi/manager.hpp"
#include "iscsi/response.hpp"
#include "iscsi/utils.hpp"
#include "iscsi/tgt/config/tgt_config.hpp"
#include "database/persistent_attributes.hpp"
#include "database/aggregate.hpp"

#include "agent-framework/module/storage_components.hpp"
#include "agent-framework/eventing/event_data.hpp"
#include "agent-framework/eventing/utils.hpp"


using namespace agent_framework;
using namespace agent_framework::model;
using namespace agent_framework::model::attribute;
using namespace agent_framework::module;
using namespace agent_framework::module::managers;
using namespace database;
using Errors = agent::storage::iscsi::tgt::Errors;

namespace {

struct TargetAuthAttributes {
    OptionalField<std::string> chap_username{};
    OptionalField<std::string> chap_secret{};
    OptionalField<std::string> mutual_chap_username{};
    OptionalField<std::string> mutual_chap_secret{};
};

static const constexpr char EMPTY_VALUE[] = "";
constexpr const char* TGT_INITIATOR_ADDRESS_ALL = "ALL";


// One Initiator Endpoint and one Target Endpoint in one Zone make an iSCSI Target.
// Zone can only have one Initiator Endpoint, but multiple Target Endpoints.
// Each Target Endpoint (if belongs to a Zone with one Initiator Endpoint) is a separate iSCSI Target.
void get_target_chap_attributes(const Uuid& target_endpoint_uuid,
                                TargetAuthAttributes& attributes) {
    auto& endpoint_manager = get_manager<Endpoint>();
    auto& zone_endpoint_manager = agent_framework::module::get_m2m_manager<Zone, Endpoint>();

    auto zone_uuid = zone_endpoint_manager.get_parents(target_endpoint_uuid).front();
    for (auto child_endpoint_uuid : zone_endpoint_manager.get_children(zone_uuid)) {
        const auto child_endpoint = endpoint_manager.get_entry(child_endpoint_uuid);
        for (const auto& entity : child_endpoint.get_connected_entities().get_array()) {
            if (entity.get_entity_role() == enums::EntityRole::Initiator) {
                if (child_endpoint.get_username().has_value()) {
                    attributes.chap_username = child_endpoint.get_username();
                    if (child_endpoint.get_password().has_value()) {
                        attributes.chap_secret = child_endpoint.get_password();
                    }
                }
            }
            else if (entity.get_entity_role() == enums::EntityRole::Target &&
                     child_endpoint.get_uuid() == target_endpoint_uuid) {
                if (child_endpoint.get_username().has_value()) {
                    attributes.mutual_chap_username = child_endpoint.get_username();
                    if (child_endpoint.get_password().has_value()) {
                        attributes.mutual_chap_secret = child_endpoint.get_password();
                    }
                }
            }
        }
    }
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
        log_debug("storage-agent", "No target initiator-name set.");
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
        log_debug("storage-agent", "No target initiator set.");
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
                 const Uuid& endpoint_uuid) {

    const auto& endpoint_manager = get_manager<Endpoint>();
    const auto endpoint = endpoint_manager.get_entry(endpoint_uuid);
    for (const auto& entity : endpoint.get_connected_entities()) {
        auto volume = get_manager<Volume>().get_entry(entity.get_entity());
        std::string entity_lun{};
        try {
            entity_lun = attribute::Identifier::get_lun(entity);
        }
        catch (const std::logic_error&) {
            THROW(agent_framework::exceptions::InvalidValue, "storage-agent", "Cannot read LUN Identifier");
        }
        auto lun_res = manager.create_lun(target_id,
                                          std::stoull(entity_lun, NULL, 0),
                                          Identifier::get_system_path(volume));
        if (!lun_res.is_valid()) {
            Errors::throw_exception(lun_res.get_error(), "Create lun error: ");
        }
        target.add_target_lun({
                                  uint32_t(std::strtoul(entity_lun.c_str(), NULL, 0)),
                                  volume.get_uuid()
                              });
    }
}


void validate_chap_data(const TargetAuthAttributes& target_auth_attributes) {
    // Check if CHAP account parameters specified

    if (target_auth_attributes.chap_username == EMPTY_VALUE) {
        THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
              "CHAP username parameter must be specified and set to string to add new credentials.");
    }

    if (target_auth_attributes.chap_username.has_value() &&
        ((!target_auth_attributes.chap_secret.has_value()) &&
         (!agent::storage::iscsi::tgt::validate_user_without_pass(target_auth_attributes.chap_username)))) {
        THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
              "CHAP username parameter must be specified and set to string to add new credentials.");
    }


    // Check if Mutual CHAP account parameters specified
    if (target_auth_attributes.mutual_chap_username == EMPTY_VALUE) {
        THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
              "Mutual CHAP username parameter must be specified and set to string to add new credentials.");
    }

    if (target_auth_attributes.mutual_chap_username.has_value() &&
        ((!target_auth_attributes.mutual_chap_secret.has_value()) &&
         (!agent::storage::iscsi::tgt::validate_user_without_pass(target_auth_attributes.mutual_chap_username)))) {
        THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
              "Mutual CHAP username parameter must be specified and set to string to add new credentials.");
    }
}


void unbind_chap_from_all(agent::storage::iscsi::tgt::Manager& manager, const std::string& username) {
    for (const auto& target_uuid : get_manager<IscsiTarget>().get_keys()) {
        auto target = get_manager<IscsiTarget>().get_entry(target_uuid);
        if (username == target.get_mutual_chap_username()) {
            auto response = manager.unbind_chap_account(target.get_target_id(), username, true);
            if (!response.is_valid()) {
                Errors::throw_exception(response.get_error(),
                                        "Cannot unbind mutual CHAP account from target: ");
            }
        }
        if (username == target.get_chap_username()) {
            auto response = manager.unbind_chap_account(target.get_target_id(), username);
            if (!response.is_valid()) {
                Errors::throw_exception(response.get_error(),
                                        "Cannot unbind CHAP account from target: ");
            }
        }
    }
}


void create_chap_account(agent::storage::iscsi::tgt::Manager& manager, const std::string& username_new,
                         const std::string& password_new) {
    auto response_new = manager.create_chap_account(username_new, password_new);
    if (!response_new.is_valid()) {
        Errors::throw_exception(response_new.get_error(),
                                "Cannot create CHAP account: ");
    }
}


void bind_chap_account(agent::storage::iscsi::tgt::Manager& manager, const std::int32_t target_id,
                       const std::string& username_new, bool mutual) {
    auto response_new = manager.bind_chap_account(target_id, username_new, mutual);
    if (!response_new.is_valid()) {
        Errors::throw_exception(response_new.get_error(),
                                "Cannot bind CHAP account: ");
    }
}


void remove_tgt_config_file(const std::string& target_iqn) {
    const auto& iscsi_data =
        get_manager<IscsiTarget, IscsiTargetManager>().get_iscsi_data();
    agent::storage::iscsi::tgt::config::TgtConfig tgt_config(iscsi_data.get_configuration_path());
    try {
        tgt_config.remove_target(target_iqn);
    }
    catch (const std::exception& ex) {
        log_warning("storage-agent",
                    "Unable to remove TGT target config file: " << ex.what());
    }
}


bool is_username_used_once(const std::string& username) {
    short usages = 0;
    for (auto target : get_manager<IscsiTarget>().get_entries()) {
        if (username == target.get_mutual_chap_username()) {
            usages++;
            if (usages > 1) {
                return false;
            }
        }
        if (username == target.get_chap_username()) {
            usages++;
            if (usages > 1) {
                return false;
            }
        }
    }
    return true;
}


// IQN from Initiator Endpoint can be only read via Zone to which given Target Endpoint belongs
std::string get_initiator_iqn_from_endpoint(const Uuid& target_endpoint_uuid) {
    const auto& endpoint_manager = get_manager<Endpoint>();
    const auto& zone_endpoint_manager = agent_framework::module::get_m2m_manager<Zone, Endpoint>();

    // Zone can only have one Initiator Endpoint
    auto zone_uuid = zone_endpoint_manager.get_parents(target_endpoint_uuid).front();
    for (auto child_endpoint_uuid : zone_endpoint_manager.get_children(zone_uuid)) {
        const auto& child_endpoint = endpoint_manager.get_entry(child_endpoint_uuid);
        for (const auto& entity : child_endpoint.get_connected_entities().get_array()) {
            if (entity.get_entity_role() == enums::EntityRole::Initiator) {
                try {
                    return attribute::Identifier::get_iqn(child_endpoint);
                }
                catch (const std::logic_error&) {
                    continue;
                }
            }
        }
    }
    return {};
}

}

namespace agent {
namespace storage {
namespace iscsi {
namespace tgt {

void add_chap_account_to_target(agent::storage::iscsi::tgt::Manager& manager, const std::int32_t target_id,
                                const std::string& username_new, const std::string& password_new, bool mutual,
                                bool only_bind) {
    if (!only_bind) {
        create_chap_account(manager, username_new, password_new);
    }
    bind_chap_account(manager, target_id, username_new, mutual);
}


void add_existing_chap_account_to_target(agent::storage::iscsi::tgt::Manager& manager, const std::int32_t target_id,
                                         const std::string& username_existing, bool mutual) {
    bind_chap_account(manager, target_id, username_existing, mutual);
}


void delete_chap_account(agent::storage::iscsi::tgt::Manager& manager, const std::string& username) {
    /* TGT can crash if a bound CHAP account is deleted, so we make sure all instances are unbounded */
    unbind_chap_from_all(manager, username);
    auto response_old = manager.delete_chap_account(username);
    if (!response_old.is_valid()) {
        Errors::throw_exception(response_old.get_error(), "Cannot unbind CHAP account from target: ");
    }
}


void replace_chap_account_in_target(agent::storage::iscsi::tgt::Manager& manager, const std::int32_t target_id,
                                    const std::string& username_old, const std::string& username_new,
                                    const std::string& password_new, bool mutual, bool only_bind) {
    if (!only_bind) {
        create_chap_account(manager, username_new, password_new);
        delete_chap_account(manager, username_old);
    }
    bind_chap_account(manager, target_id, username_new, mutual);
}


void update_password_for_chap_account(agent::storage::iscsi::tgt::Manager& manager, const std::int32_t target_id,
                                      const std::string& username, const std::string& password, bool mutual,
                                      bool only_bind) {
    if (!only_bind) {
        delete_chap_account(manager, username);
        create_chap_account(manager, username, password);
    }
    bind_chap_account(manager, target_id, username, mutual);
}


bool validate_user_without_pass(const std::string& username) {
    if (username.empty()) {
        return true;
    }
    for (const auto& iscsi_target : get_manager<IscsiTarget, IscsiTargetManager>().get_entries()) {
        if (iscsi_target.get_chap_username() == username ||
            iscsi_target.get_mutual_chap_username() == username) {
            return true;
        }
    }
    return false;
}


bool is_endpoint_valid_for_iscsi(const Uuid& endpoint_uuid) {
    auto& endpoint_manager = get_manager<Endpoint>();
    const auto endpoint = endpoint_manager.get_entry(endpoint_uuid);
    if (endpoint.get_status().get_health() != enums::Health::OK) {
        log_debug("storage-agent", "Endpoint is not healthy: " << endpoint_uuid);
        return false;
    }
    for (const auto& entity : endpoint.get_connected_entities().get_array()) {
        if (entity.get_entity_role() != enums::EntityRole::Target) {
            log_debug("storage-agent", "Endpoint role is not Target: " << endpoint_uuid);
            return false;
        }
        // Different ConnectedEntities cannot point to the same Entity (in this case a Volume)
        for (const auto& endpoint2 : endpoint_manager.get_entries()) {
            if (endpoint_uuid == endpoint2.get_uuid()) { continue; }
            for (const auto& entity2 : endpoint2.get_connected_entities().get_array()) {
                if (entity2.get_entity() == entity.get_entity()) {
                    log_debug("storage-agent", "Entity " << entity.get_entity()
                                                         << " is already used in Endpoint: "
                                                         << endpoint2.get_uuid());
                    return false;
                }
            }
        }
    }

    auto& zone_endpoint_manager = agent_framework::module::get_m2m_manager<Zone, Endpoint>();
    if (zone_endpoint_manager.get_parents(endpoint_uuid).empty()) {
        log_debug("storage-agent",
                  "Endpoint does not belong to any Zone: "
                      << endpoint_uuid);
        return false;
    }

    const auto zone_uuid = zone_endpoint_manager.get_parents(endpoint_uuid).front();
    for (const auto& child_endpoint_uuid : zone_endpoint_manager.get_children(zone_uuid)) {
        const auto child_endpoint = endpoint_manager.get_entry(child_endpoint_uuid);
        for (const auto& entity : child_endpoint.get_connected_entities().get_array()) {
            if (entity.get_entity_role() == enums::EntityRole::Initiator) {
                return true;
            }
        }
    }

    log_debug("storage-agent",
              "Target Endpoint does not belong to any Zone with Initiator Endpoint: "
                  << endpoint_uuid);
    return false;
}


bool is_endpoint_valid_for_chap(const Uuid& endpoint_uuid) {
    auto& endpoint_manager = get_manager<Endpoint>();
    const auto endpoint = endpoint_manager.get_entry(endpoint_uuid);
    auto& zone_endpoint_manager = agent_framework::module::get_m2m_manager<Zone, Endpoint>();

    if (endpoint.get_username().has_value()) {
        if (!validate_user_without_pass(endpoint.get_username())) {
            return false;
        }
    }

    const auto zone_uuid = zone_endpoint_manager.get_parents(endpoint_uuid).front();
    for (const auto& child_endpoint_uuid : zone_endpoint_manager.get_children(zone_uuid)) {
        const auto child_endpoint = endpoint_manager.get_entry(child_endpoint_uuid);
        for (const auto& entity : child_endpoint.get_connected_entities().get_array()) {
            if (entity.get_entity_role() == enums::EntityRole::Initiator) {
                if (child_endpoint.get_username().has_value()) {
                    if (!validate_user_without_pass(child_endpoint.get_username())) {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}


void remove_iscsi_target(const Uuid& target_uuid) {
    const auto target = get_manager<IscsiTarget>().get_entry(target_uuid);

    auto& previous_chap_username = target.get_chap_username();
    auto& previous_mutual_chap_username = target.get_mutual_chap_username();

    const auto target_id = get_manager<IscsiTarget, IscsiTargetManager>().
        get_target_id(target.get_uuid());
    ::agent::storage::iscsi::tgt::Manager manager{};

    // Delete CHAP account if exist and is not used in other iSCSI Targets
    if (previous_chap_username.has_value() && previous_chap_username.value() != EMPTY_VALUE) {
        if (is_username_used_once(previous_chap_username.value())) {
            try {
                delete_chap_account(manager, previous_chap_username.value());
            }
            catch (const std::exception& ex) {
                log_warning("storage-agent",
                            "CHAP account delete operation failed: " << ex.what());
            }
        }
    }

    // Delete Mutual CHAP account if exist and is not used in other iSCSI Targets
    if (previous_mutual_chap_username.has_value() && previous_mutual_chap_username.value() != EMPTY_VALUE) {
        if (is_username_used_once(previous_mutual_chap_username.value())) {
            try {
                delete_chap_account(manager, previous_mutual_chap_username.value());
            }
            catch (const std::exception& ex) {
                log_warning("storage-agent",
                            "Mutual CHAP account delete operation failed: " << ex.what());
            }
        }
    }

    auto tgt_response = manager.destroy_target(int(target_id));

    if (!tgt_response.is_valid()) {
        Errors::throw_exception(tgt_response.get_error(), "Cannot remove target: ");
    }
    remove_tgt_config_file(target.get_target_iqn());
    get_manager<IscsiTarget, IscsiTargetManager>().remove_target_id(target_id);

    log_info("storage-agent", "iSCSI target removed: " << target_uuid);
    get_manager<IscsiTarget>().remove_entry(target_uuid);
}


void add_iscsi_target(const Uuid& target_endpoint_uuid, bool skip_validation) {

    if (!skip_validation) {
        if (!is_endpoint_valid_for_iscsi(target_endpoint_uuid)) {
            THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
                  "Could not create iSCSI Target.");
        }
    }

    TargetAuthAttributes target_auth_attributes;
    get_target_chap_attributes(target_endpoint_uuid, target_auth_attributes);
    validate_chap_data(target_auth_attributes);

    const auto target_endpoint = get_manager<Endpoint>().get_entry(target_endpoint_uuid);
    const auto& iscsi_data = get_manager<IscsiTarget, IscsiTargetManager>().get_iscsi_data();
    const auto target_iqn = attribute::Identifier::get_iqn(target_endpoint);
    const auto initiator_iqn = get_initiator_iqn_from_endpoint(target_endpoint_uuid);

    IscsiTarget target{};
    target.set_initiator_iqn(initiator_iqn);
    target.set_target_iqn(target_iqn);
    target.set_target_address(iscsi_data.get_portal_ip());
    target.set_target_port(std::to_string(iscsi_data.get_portal_port()));
    target.set_status({enums::State::Enabled, enums::Health::OK});
    const auto target_id = get_manager<IscsiTarget, IscsiTargetManager>().create_target_id();
    target.set_target_id(target_id);
    agent::storage::iscsi::tgt::Manager manager{};
    create_target(manager, int(target_id), target_iqn);
    set_initiator_name(manager, int(target_id), initiator_iqn);
    set_initiator_address(manager, int(target_id), TGT_INITIATOR_ADDRESS_ALL);

    try {
        create_luns(manager, target, int(target_id), target_endpoint_uuid);
    }
    catch (...) {
        delete_target(manager, int(target_id));
        throw;
    }

    agent::storage::iscsi::tgt::config::TgtConfig tgt_config(iscsi_data.get_configuration_path());
    try {
        tgt_config.add_target(target);
    }
    catch (const std::exception& ex) {
        log_warning("storage-agent", "Unable to create TGT target config file: " << ex.what());
    }

    if (target_auth_attributes.chap_username.has_value()) {
        try {
            if (target_auth_attributes.chap_secret.has_value() &&
                (!agent::storage::iscsi::tgt::validate_user_without_pass(target_auth_attributes.chap_username))) {
                add_chap_account_to_target(manager, target_id,
                                           target_auth_attributes.chap_username,
                                           target_auth_attributes.chap_secret, false);
            }
            else {
                add_existing_chap_account_to_target(manager, target_id,
                                                    target_auth_attributes.chap_username, false);

            }
        }
        catch (...) {
            delete_target(manager, int(target_id));
            throw;
        }
        target.set_chap_username(target_auth_attributes.chap_username);
    }
    if (target_auth_attributes.mutual_chap_username.has_value()) {
        try {
            if (target_auth_attributes.mutual_chap_secret.has_value() &&
                (!agent::storage::iscsi::tgt::validate_user_without_pass(target_auth_attributes.mutual_chap_username))) {
                add_chap_account_to_target(manager, target_id,
                                           target_auth_attributes.mutual_chap_username,
                                           target_auth_attributes.mutual_chap_secret, true);
            }
            else {
                add_existing_chap_account_to_target(manager, target_id,
                                                    target_auth_attributes.mutual_chap_username, true);
            }
        }
        catch (...) {
            delete_chap_account(manager, target_auth_attributes.mutual_chap_username);
            delete_target(manager, int(target_id));
            throw;
        }
        target.set_mutual_chap_username(target_auth_attributes.mutual_chap_username);
    }

    get_manager<IscsiTarget, IscsiTargetManager>().add_target_id(target_id, target.get_uuid());
    get_manager<IscsiTarget>().add_entry(std::move(target));

    log_info("storage-agent", "iSCSI Target added: " << target_iqn);
}


void check_endpoints_added_to_zone(const attribute::Array<Uuid>& endpoint_uuids) {
    bool initiator_found = false;
    bool target_found = false;
    auto& zone_endpoint_manager = agent_framework::module::get_m2m_manager<Zone, Endpoint>();

    for (const Uuid& endpoint_uuid : endpoint_uuids) {
        auto const& endpoint = get_manager<Endpoint>().get_entry(endpoint_uuid);
        if (endpoint.get_status().get_health() != enums::Health::OK ||
            endpoint.get_status().get_state() != enums::State::Enabled) {
            THROW(exceptions::InvalidValue, "storage-agent",
                  "Zone has Endpoint with incorrect status: " + endpoint.get_uuid());
        }

        for (const auto& entity : endpoint.get_connected_entities()) {
            if (entity.get_entity_role() == enums::EntityRole::Initiator) {
                if (initiator_found) {
                    THROW(exceptions::InvalidValue, "storage-agent",
                          "Zone has duplicated Initiator Endpoint: " + endpoint.get_uuid());
                }
                initiator_found = true;
            }
            else if (entity.get_entity_role() == enums::EntityRole::Target) {
                target_found = true;
            }
        }

        if (zone_endpoint_manager.get_parents(endpoint.get_uuid()).size() > 0) {
            THROW(exceptions::InvalidValue, "storage-agent",
                  "Another Zone already contains this Endpoint: " + endpoint.get_uuid());
        }

        if (target_found) {
            auto& target_manager = get_manager<IscsiTarget, agent_framework::module::managers::IscsiTargetManager>();
            const auto& endpoint_iqn = attribute::Identifier::get_iqn(endpoint);
            for (auto const& target : target_manager.get_entries()) {
                if (endpoint_iqn == target.get_target_iqn()) {
                    THROW(exceptions::InvalidValue, "storage-agent",
                          "Zone contains Target Endpoint: " + endpoint.get_uuid() + ", with already existing IQN: " +
                          endpoint_iqn);
                }
            }
        }
    }
}


void create_iscsi_targets_from_endpoints(const attribute::Array<Uuid>& endpoint_uuids) {
    for (const Uuid& endpoint_uuid : endpoint_uuids) {
        auto const& endpoint = get_manager<Endpoint>().get_entry(endpoint_uuid);
        for (const auto& entity : endpoint.get_connected_entities()) {
            if (entity.get_entity_role() == enums::EntityRole::Target) {
                agent::storage::iscsi::tgt::add_iscsi_target(endpoint_uuid, false);
                break;
            }
        }
    }
}


void remove_iscsi_targets_from_endpoints(const attribute::Array<Uuid>& endpoint_uuids) {
    for (const Uuid& endpoint_uuid : endpoint_uuids) {
        auto endpoint_ref = get_manager<Endpoint>().get_entry_reference(endpoint_uuid);
        for (const auto& entity : endpoint_ref->get_connected_entities()) {
            if (entity.get_entity_role() == enums::EntityRole::Target) {
                auto& target_manager = get_manager<IscsiTarget, agent_framework::module::managers::IscsiTargetManager>();
                auto endpoint_iqn = attribute::Identifier::get_iqn(endpoint_ref.get_raw_ref());
                for (auto const& target : target_manager.get_entries()) {
                    if (endpoint_iqn == target.get_target_iqn()) {
                        agent::storage::iscsi::tgt::remove_iscsi_target(target.get_uuid());
                    }
                }
            }
            if (endpoint_ref->get_username().has_value() && (!endpoint_ref->get_password().has_value())) {
                // Credentials without password in endpoint must be cleaned, because password cannot be restored
                endpoint_ref->clean_username();
                endpoint_ref->clean_password();
                clean_chap_from_database(endpoint_uuid);
                eventing::send_event(endpoint_uuid,
                                     enums::Component::Endpoint,
                                     enums::Notification::Update,
                                     get_manager<Fabric>().get_keys().front());
                log_info("storage-discovery", "Removed username from Endpoint: " << endpoint_uuid);
            }

            // Finding one Connected Entity with role Target is sufficient to delete iSCSI Target
            break;
        }
    }
}


bool is_role_present(const attribute::Array<Uuid>& endpoint_uuids, enums::EntityRole role) {
    for (const Uuid& endpoint_uuid : endpoint_uuids) {
        auto const& endpoint = get_manager<Endpoint>().get_entry(endpoint_uuid);
        for (const auto& entity : endpoint.get_connected_entities()) {
            if (entity.get_entity_role() == role) {
                return true;
            }
        }
    }
    return false;
}


void clean_chap_from_endpoint(const Uuid& endpoint_uuid) {
    auto& endpoint_manager = get_manager<Endpoint>();
    auto endpoint = endpoint_manager.get_entry_reference(endpoint_uuid);
    if (!endpoint->get_username().has_value()) {
        return;
    }
    endpoint->clean_username();
    endpoint->clean_password();
    clean_chap_from_database(endpoint_uuid);
}


void clean_chap_from_database(const Uuid& endpoint_uuid) {
    auto& pa = PersistentAttributes::get_instance();
    Aggregate<PersistentAttributes> agg(pa, endpoint_uuid, literals::Endpoint::ENDPOINT);
    try {
        agg.del(literals::Endpoint::USERNAME);
        agg.dump();
        log_info("storage-discovery",
                 "Removed username from PersistentAttributes for Endpoint: " << endpoint_uuid);
    }
    catch (std::out_of_range) {
        log_warning("storage-discovery",
                    "Could not find PersistentAttributes for Endpoint: " << endpoint_uuid);
    }
}


bool endpoint_represents_iscsi_target(const Uuid& uuid) {

    auto& zone_endpoint_manager = agent_framework::module::get_m2m_manager<Zone, Endpoint>();
    if (zone_endpoint_manager.get_parents(uuid).size() == 0) {
        return false;
    }

    const auto zone_uuid = zone_endpoint_manager.get_parents(uuid).front();
    const auto endpoints = zone_endpoint_manager.get_children(zone_uuid);

    return (agent::storage::iscsi::tgt::is_role_present(endpoints, enums::EntityRole::Initiator) &&
            agent::storage::iscsi::tgt::is_role_present(endpoints, enums::EntityRole::Target));
}


bool is_endpoint_username_in_use(const std::string& username) {
    for (const auto& endpoint : get_manager<Endpoint>().get_entries()) {
        if (endpoint.get_username() == username) {
            return true;
        }
    }
    return false;
}

}
}
}
}


