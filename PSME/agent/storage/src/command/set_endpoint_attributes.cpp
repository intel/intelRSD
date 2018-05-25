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
 * */

#include "agent-framework/module/requests/validation/common.hpp"
#include "iscsi/manager.hpp"
#include "iscsi/utils.hpp"
#include "database/persistent_attributes.hpp"
#include "database/aggregate.hpp"
#include "command/set_endpoint_attributes.hpp"


using namespace database;
using namespace agent::storage;
using namespace agent_framework;
using namespace agent_framework::command;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework::model::attribute;
using namespace agent_framework::model::requests::validation;

namespace {

void get_endpoint_attributes(const attribute::Attributes& attributes,
                             ConfigurableEndpointAttributes& attributes_to_configure) {

    const auto attributes_names = attributes.get_names();

    for (const auto& attribute_name : attributes_names) {
        const auto& value = attributes.get_value(attribute_name);

        log_debug("storage-agent", "Attribute name: " << attribute_name);
        log_debug("storage-agent", "Attribute value: " << value.dump());

        if (literals::Endpoint::USERNAME == attribute_name) {
            attributes_to_configure.username = OptionalField<std::string>(
                value.is_null() ? EMPTY_VALUE : value);
        }
        else if (literals::Endpoint::PASSWORD == attribute_name) {
            attributes_to_configure.password = OptionalField<std::string>(value.is_null() ? EMPTY_VALUE : value);
        }
        else {
            log_warning("storage-agent", "Unsupported attribute: " << attribute_name << ".");
        }
    }
}


void update_persistent_attributes(const Uuid& endpoint_uuid, const std::string username) {
    auto& pa = PersistentAttributes::get_instance();

    try {
        Aggregate<PersistentAttributes> aggr_endpoint{pa, endpoint_uuid, literals::Endpoint::ENDPOINT};
        try {
            aggr_endpoint.update(literals::Endpoint::USERNAME, username);
        }
        catch (std::out_of_range) {
            aggr_endpoint.put(literals::Endpoint::USERNAME, username);
        }
        aggr_endpoint.dump();
    }
    catch (std::out_of_range) {
        log_warning("storage-discovery",
                    "Could not udpate PersistentAttributes for Endpoint: " << endpoint_uuid);
    }
}


GenericManager<IscsiTarget>::Reference get_iscsi_target_reference(const Uuid& endpoint_uuid) {
    auto const& endpoint = get_manager<Endpoint>().get_entry(endpoint_uuid);
    for (const auto& entity : endpoint.get_connected_entities()) {
        if (entity.get_entity_role() == enums::EntityRole::Target) {
            auto& target_manager = get_manager<IscsiTarget, agent_framework::module::managers::IscsiTargetManager>();
            const auto& endpoint_iqn = attribute::Identifier::get_iqn(endpoint);
            for (auto const& target : target_manager.get_entries()) {
                if (endpoint_iqn == target.get_target_iqn()) {
                    return get_manager<IscsiTarget>().get_entry_reference(target.get_uuid());
                }
            }
        }
    }
    THROW(agent_framework::exceptions::InvalidValue, "storage-agent", "Could not find iSCSI Target in TGT");
}


void delete_chap_from_target(const Uuid& endpoint_uuid, SetComponentAttributes::Response& response, bool mutual = false,
                          bool already_deleted_from_tgt = false) {
    agent::storage::iscsi::tgt::Manager manager{};
    auto target = get_iscsi_target_reference(endpoint_uuid);
    log_debug("storage-agent",
              "Deleting CHAP credentials in iSCSI Target: " << target->get_target_iqn() << " and target endpoint: " <<
                                                           endpoint_uuid);
    if (!already_deleted_from_tgt) {
        try {
            delete_chap_account(manager, mutual ? target->get_mutual_chap_username() : target->get_chap_username());
        }
        catch (const exceptions::GamiException& ex) {
            response.add_status(
                {literals::Endpoint::USERNAME, ex.get_error_code(), ex.get_message()});
            return;
        }
    }
    if (mutual) {
        target->set_mutual_chap_username(OptionalField<std::string>());
    }
    else {
        target->set_chap_username(OptionalField<std::string>());
    }
}


void process_endpoint_auth_null(const Uuid& uuid, SetComponentAttributes::Response& response) {

    auto endpoint = get_manager<Endpoint>().get_entry_reference(uuid);
    if (!endpoint->get_username().has_value()) {
        THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
              "username and password parameters are already empty for this endpoint");
    }
    auto& zone_endpoint_manager = agent_framework::module::get_m2m_manager<Zone, Endpoint>();
    if (agent::storage::iscsi::tgt::endpoint_represents_iscsi_target(uuid)) {
        if (agent::storage::iscsi::tgt::is_role_present({uuid}, enums::EntityRole::Initiator)) {
            // Remove One-way CHAP from all Target Endpoints from the Zone with this Initiator Endpoint
            const auto zone_uuid = zone_endpoint_manager.get_parents(uuid).front();
            bool account_deleted = false;
            for (auto& endpoint_uuid : zone_endpoint_manager.get_children(zone_uuid)) {
                if (endpoint_uuid == uuid) {
                    continue;
                }
                delete_chap_from_target(endpoint_uuid, response, false, account_deleted);
                account_deleted = true; // Account was deleted, next time only remove if from iSCSI Target data
            }
        }
        else if (agent::storage::iscsi::tgt::is_role_present({uuid}, enums::EntityRole::Target)) {
            // Remove Mutual CHAP only from given Target Endpoint
            delete_chap_from_target(uuid, response, true);
        }
    }
    else {
        log_debug("storage-agent", "Deleting CHAP credentials from endpoint which is not in use: " + uuid);
    }
    agent::storage::iscsi::tgt::clean_chap_from_endpoint(uuid);
}


void check_username_usage(const std::string username) {
    if (agent::storage::iscsi::tgt::is_endpoint_username_in_use(username)) {
        THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
              "Given username is already in use in another endpoint");
    }
}


void process_endpoint_auth_chap(const std::string& uuid,
                                const ConfigurableEndpointAttributes& attributes_to_configure,
                                SetComponentAttributes::Response& response) {

    auto endpoint = get_manager<Endpoint>().get_entry_reference(uuid);
    auto& zone_endpoint_manager = agent_framework::module::get_m2m_manager<Zone, Endpoint>();

    agent::storage::iscsi::tgt::Manager manager{};
    if (agent::storage::iscsi::tgt::endpoint_represents_iscsi_target(uuid)) {
        // Update One-way CHAP in all Target Endpoints from the Zone with this Initiator Endpoint
        try {
            if (endpoint->get_username().has_value() && endpoint->get_username().value() != EMPTY_VALUE) {
                if (attributes_to_configure.username.value() != endpoint->get_username().value()) {
                    // Replace exising CHAP account
                    check_username_usage(attributes_to_configure.username);
                    bool only_bind = false;
                    const auto zone_uuid = zone_endpoint_manager.get_parents(uuid).front();
                    for (auto& endpoint_uuid : zone_endpoint_manager.get_children(zone_uuid)) {
                        if (endpoint_uuid == uuid) {
                            continue;
                        }
                        auto target = get_iscsi_target_reference(endpoint_uuid);
                        log_debug("storage-agent",
                                  "Replacing existing CHAP One-way credentials in iSCSI Target: "
                                      << target->get_target_iqn() << " and initiator endpoint: " + endpoint_uuid);
                        replace_chap_account_in_target(manager, target->get_target_id(), target->get_chap_username(),
                                                       attributes_to_configure.username,
                                                       attributes_to_configure.password, false, only_bind);
                        only_bind = true; // Account was created, next time only bind already existing account
                        target->set_chap_username(attributes_to_configure.username);
                        target->set_chap_secret(attributes_to_configure.password);
                    }
                    update_persistent_attributes(uuid, attributes_to_configure.username);
                }
                else {
                    // Update password of existing CHAP account
                    bool only_bind = false;
                    const auto zone_uuid = zone_endpoint_manager.get_parents(uuid).front();
                    for (const auto& endpoint_uuid : zone_endpoint_manager.get_children(zone_uuid)) {
                        if (endpoint_uuid == uuid) {
                            continue;
                        }
                        auto target = get_iscsi_target_reference(endpoint_uuid);
                        log_debug("storage-agent",
                                  "Updating existing CHAP One-way credentials in iSCSI Target: "
                                      << target->get_target_iqn() << " and initiator endpoint: " << uuid);
                        update_password_for_chap_account(manager, target->get_target_id(),
                                                         attributes_to_configure.username,
                                                         attributes_to_configure.password, false, only_bind);
                        only_bind = true; // Account was created, next time only bind already existing account
                        target->set_chap_username(attributes_to_configure.username);
                        target->set_chap_secret(attributes_to_configure.password);
                    }
                }
            }
            else {
                // Create new CHAP account
                check_username_usage(attributes_to_configure.username);
                bool only_bind = false;
                const auto zone_uuid = zone_endpoint_manager.get_parents(uuid).front();
                for (auto& endpoint_uuid : zone_endpoint_manager.get_children(zone_uuid)) {
                    if (endpoint_uuid == uuid) {
                        continue;
                    }
                    auto target = get_iscsi_target_reference(endpoint_uuid);
                    log_debug("storage-agent",
                              "Adding new CHAP One-way credentials to iSCSI Target: " << target->get_target_iqn() <<
                                                                                     " and initiator endpoint: "
                                                                                     << uuid);
                    add_chap_account_to_target(manager, target->get_target_id(),
                                               attributes_to_configure.username,
                                               attributes_to_configure.password, false, only_bind);
                    only_bind = true; // Account was created, next time only bind already existing account
                    target->set_chap_username(attributes_to_configure.username);
                    target->set_chap_secret(attributes_to_configure.password);
                }
                update_persistent_attributes(uuid, attributes_to_configure.username);
            }
            endpoint->set_username(attributes_to_configure.username);
            endpoint->set_password(attributes_to_configure.password);
        }
        catch (const exceptions::GamiException& ex) {
            response.add_status(
                {literals::IscsiTarget::CHAP_USERNAME, ex.get_error_code(), ex.get_message()});
            return;
        }
    }
    else {
        // Only add CHAP credentials to Endpoint, do not create CHAP account in TGT
        log_debug("storage-agent",
                  "Adding CHAP One-way credentials to initiator endpoint which is not in use: " + uuid);
        check_username_usage(attributes_to_configure.username);
        update_persistent_attributes(uuid, attributes_to_configure.username);
        endpoint->set_username(attributes_to_configure.username);
        endpoint->set_password(attributes_to_configure.password);
    }
}


void process_endpoint_auth_mutual_chap(const Uuid& uuid,
                                       const ConfigurableEndpointAttributes& attributes_to_configure,
                                       SetComponentAttributes::Response& response) {

    auto endpoint = get_manager<Endpoint>().get_entry_reference(uuid);
    agent::storage::iscsi::tgt::Manager manager{};

    // Set Mututal CHAP credentials only in given Target Endpoint
    if (agent::storage::iscsi::tgt::endpoint_represents_iscsi_target(uuid)) {
        try {
            auto target = get_iscsi_target_reference(uuid);
            if (endpoint->get_username().has_value() && endpoint->get_username().value() != EMPTY_VALUE) {
                if (attributes_to_configure.username.value() != endpoint->get_username().value()) {
                    log_debug("storage-agent",
                              "Replacing existing CHAP Mutual credentials in iSCSI Target: " << target->get_target_iqn()
                                                                                            <<
                                                                                            " and target endpoint: " <<
                                                                                            uuid);
                    check_username_usage(attributes_to_configure.username);
                    replace_chap_account_in_target(manager, target->get_target_id(), target->get_mutual_chap_username(),
                                                   attributes_to_configure.username,
                                                   attributes_to_configure.password, true);
                }
                else {
                    log_debug("storage-agent",
                              "Updating existing CHAP Mutual credentials in iSCSI Target: " << target->get_target_iqn()
                                                                                           <<
                                                                                           " and target endpoint: "
                                                                                           << uuid);
                    update_password_for_chap_account(manager, target->get_target_id(),
                                                     attributes_to_configure.username,
                                                     attributes_to_configure.password, true);
                }
            }
            else {
                log_debug("storage-agent",
                          "Adding new CHAP Mutual credentials to iSCSI Target: " << target->get_target_iqn() <<
                                                                                " and target endpoint: " << uuid);
                check_username_usage(attributes_to_configure.username);
                add_chap_account_to_target(manager, target->get_target_id(),
                                           attributes_to_configure.username,
                                           attributes_to_configure.password, true);
            }
            target->set_mutual_chap_username(attributes_to_configure.username);
            target->set_mutual_chap_secret(attributes_to_configure.password);
        }
        catch (const exceptions::GamiException& ex) {
            response.add_status({literals::IscsiTarget::CHAP_USERNAME, ex.get_error_code(), ex.get_message()});
            return;
        }
    }
    else {
        log_debug("storage-agent", "Adding CHAP Mutual credentials to target endpoint which is not in use: " + uuid);
        check_username_usage(attributes_to_configure.username);
    }
    endpoint->set_username(attributes_to_configure.username);
    endpoint->set_password(attributes_to_configure.password);
    update_persistent_attributes(uuid, attributes_to_configure.username);
}

}

void agent::storage::process_endpoint_attributes(const std::string& uuid, const Attributes& attributes,
                                                 SetComponentAttributes::Response& response) {

    ConfigurableEndpointAttributes attributes_to_configure;
    CommonValidator::validate_set_endpoint_attributes(attributes);
    if (attributes.empty()) {
        log_info("storage-agent", "Nothing has been changed (empty request)");
        return;
    }

    // read parameters from command
    get_endpoint_attributes(attributes, attributes_to_configure);

    if (!attributes_to_configure.username.has_value()) {
        THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
              "username must be specified or null");
    }

    if (attributes_to_configure.username.has_value() && attributes_to_configure.username.value() != EMPTY_VALUE &&
        ((!attributes_to_configure.password.has_value()) ||
         (attributes_to_configure.password.has_value() && attributes_to_configure.password.value() == EMPTY_VALUE))) {
        THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
              "If username is not empty or null, then password must be specified");
    }

    if (attributes_to_configure.username.has_value() && attributes_to_configure.username.value() == EMPTY_VALUE &&
        attributes_to_configure.password.has_value() && attributes_to_configure.password.value() != EMPTY_VALUE) {
        THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
              "If username is empty or null, then password must not be specified");
    }

    if (attributes_to_configure.username.value() == EMPTY_VALUE) {
        process_endpoint_auth_null(uuid, response);
    }
    else if (agent::storage::iscsi::tgt::is_role_present({uuid}, enums::EntityRole::Initiator)) {
        process_endpoint_auth_chap(uuid, attributes_to_configure, response);

    }
    else if (agent::storage::iscsi::tgt::is_role_present({uuid}, enums::EntityRole::Target)) {
        process_endpoint_auth_mutual_chap(uuid, attributes_to_configure, response);
    }
}
