/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * @file set_endpoint_attributes.cpp
 * */

#include "agent-framework/module/requests/validation/common.hpp"
#include "agent-framework/module/storage_components.hpp"
#include "agent-framework/database/database_entity.hpp"
#include "agent/command/set_endpoint_attributes.hpp"
#include "agent/utils/iscsi/utils.hpp"

using namespace agent::storage;
using namespace agent::storage::utils;
using namespace agent::storage::command;
using namespace agent_framework::model;
using namespace agent_framework::module;

namespace {

void get_endpoint_attributes(const attribute::Attributes& attributes,
                             ConfigurableEndpointAttributes& attributes_to_configure,
                             SetComponentAttributes::Response& response) {
    const auto attributes_names = attributes.get_names();

    for (const auto& attribute_name : attributes_names) {
        const auto& value = attributes.get_value(attribute_name);

        try {
            log_debug("storage-agent", "Attribute name: " << attribute_name);
            log_debug("storage-agent", "Attribute value: " << value.dump());

            if (literals::Endpoint::USERNAME == attribute_name) {
                attributes_to_configure.username = OptionalField<std::string>(value.is_null() ? EMPTY_VALUE : value);
            }
            else if (literals::Endpoint::PASSWORD == attribute_name) {
                attributes_to_configure.password = OptionalField<std::string>(value.is_null() ? EMPTY_VALUE : value);
            }
            else {
                THROW(agent_framework::exceptions::UnsupportedField, "storage-agent",
                      "Setting attribute is not supported.", attribute_name, value);
            }
        }
        catch (const agent_framework::exceptions::GamiException& ex) {
            response.add_status({attribute_name, ex.get_error_code(), ex.get_message()});
        }
    }
}


void update_persistent_attributes(const Uuid& endpoint_uuid, const std::string username) {
    auto& pa = database::PersistentAttributes::get_instance();

    try {
        database::Aggregate<database::PersistentAttributes> aggr_endpoint{pa, endpoint_uuid, literals::Endpoint::ENDPOINT};
        try {
            aggr_endpoint.update(literals::Endpoint::USERNAME, username);
        }
        catch (std::out_of_range) {
            aggr_endpoint.put(literals::Endpoint::USERNAME, username);
        }
        aggr_endpoint.dump();
    }
    catch (const std::out_of_range&) {
        log_warning("storage-discovery",
                    "Could not update PersistentAttributes for Endpoint: " << endpoint_uuid);
    }
}


tgt::TargetData::Id get_iscsi_target_id_for_endpoint(AgentContext::SPtr context, const Uuid& endpoint_uuid) {
    auto endpoint = get_manager<Endpoint>().get_entry(endpoint_uuid);
    auto endpoint_iqn = attribute::Identifier::get_iqn(endpoint);

    for (const auto& entity : endpoint.get_connected_entities()) {
        if (entity.get_entity_role() == enums::EntityRole::Target) {
            for (auto const& target_entry : context->configuration->get_iscsi_targets()) {
                if (endpoint_iqn == target_entry.second.get_target_iqn()) {
                    return target_entry.first;
                }
            }
        }
    }
    THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
          "Could not find iSCSI Target with iQN " + endpoint_iqn + " in TGT.");
}


void delete_chap_from_target(AgentContext::SPtr context,
                             const Uuid& endpoint_uuid,
                             SetComponentAttributes::Response& response,
                             bool mutual = false,
                             bool already_deleted_from_tgt = false) {
    tgt::Manager manager{context->configuration->get_tgt_socket()};
    auto target_id = get_iscsi_target_id_for_endpoint(context, endpoint_uuid);
    auto target = context->configuration->get_iscsi_targets().at(target_id);

    log_debug("storage-agent", "Deleting CHAP credentials in iSCSI Target: "
        << target.get_target_iqn() << " and target endpoint: " << endpoint_uuid);

    if (!already_deleted_from_tgt) {
        try {
            agent::storage::utils::delete_chap_account(context, manager,
                mutual ? target.get_mutual_chap_username() : target.get_chap_username());
        }
        catch (const agent_framework::exceptions::GamiException& ex) {
            response.add_status({literals::Endpoint::USERNAME, ex.get_error_code(), ex.get_message()});
            return;
        }
    }

    if (mutual) {
        target.set_mutual_chap_username(::EMPTY_VALUE);
    }
    else {
        target.set_chap_username(::EMPTY_VALUE);
    }
    context->configuration->add_iscsi_target(target_id, target);
}


void process_endpoint_auth_null(AgentContext::SPtr context, const Uuid& uuid, SetComponentAttributes::Response& response) {

    auto endpoint = get_manager<Endpoint>().get_entry_reference(uuid);
    if (!endpoint->get_username().has_value()) {
        THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
              "username and password parameters are already empty for this endpoint");
    }
    auto& zone_endpoint_manager = agent_framework::module::get_m2m_manager<Zone, Endpoint>();
    if (agent::storage::utils::endpoint_represents_iscsi_target(uuid)) {
        if (Endpoint::is_in_role(uuid, enums::EntityRole::Initiator)) {
            // Remove One-way CHAP from all Target Endpoints from the Zone with this Initiator Endpoint
            const auto zone_uuid = zone_endpoint_manager.get_parents(uuid).front();
            bool account_deleted = false;
            for (auto& endpoint_uuid : zone_endpoint_manager.get_children(zone_uuid)) {
                if (endpoint_uuid == uuid) {
                    continue;
                }
                delete_chap_from_target(context, endpoint_uuid, response, false, account_deleted);
                account_deleted = true; // Account was deleted, next time only remove if from iSCSI Target data
            }
        }
        else if (Endpoint::is_in_role(uuid, enums::EntityRole::Target)) {
            // Remove Mutual CHAP only from given Target Endpoint
            delete_chap_from_target(context, uuid, response, true);
        }
    }
    else {
        log_debug("storage-agent", "Deleting CHAP credentials from endpoint which is not in use: " + uuid);
    }
    agent::storage::utils::clean_chap_from_endpoint(uuid);
}


void check_username_usage(const std::string username) {
    if (is_endpoint_username_in_use(username)) {
        THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
              "Given username is already in use in another endpoint");
    }
}


void process_endpoint_auth_chap(AgentContext::SPtr context,
                                const Uuid& uuid,
                                const ConfigurableEndpointAttributes& attributes_to_configure,
                                SetComponentAttributes::Response& response) {

    auto endpoint = get_manager<Endpoint>().get_entry_reference(uuid);
    auto& zone_endpoint_manager = agent_framework::module::get_m2m_manager<Zone, Endpoint>();

    tgt::Manager manager{context->configuration->get_tgt_socket()};
    if (agent::storage::utils::endpoint_represents_iscsi_target(uuid)) {
        // Update One-way CHAP in all Target Endpoints from the Zone with this Initiator Endpoint
        try {
            if (endpoint->get_username().has_value() && endpoint->get_username().value() != EMPTY_VALUE) {
                if (attributes_to_configure.username.value() != endpoint->get_username().value()) {
                    // Replace existing CHAP account
                    check_username_usage(attributes_to_configure.username);
                    bool only_bind = false;
                    const auto zone_uuid = zone_endpoint_manager.get_parents(uuid).front();
                    for (auto& endpoint_uuid : zone_endpoint_manager.get_children(zone_uuid)) {
                        if (endpoint_uuid == uuid) {
                            continue;
                        }

                        auto target_id = get_iscsi_target_id_for_endpoint(context, endpoint_uuid);
                        auto target = context->configuration->get_iscsi_targets().at(target_id);
                        log_debug("storage-agent", "Replacing existing CHAP One-way credentials in iSCSI Target: "
                            << target.get_target_iqn() << " and initiator endpoint: " + endpoint_uuid);

                        replace_chap_account_in_target(context, manager,
                            target.get_target_id(), target.get_chap_username(),
                            attributes_to_configure.username, attributes_to_configure.password,
                            false, only_bind);

                        only_bind = true; // Account was created, next time only bind already existing account
                        target.set_chap_username(attributes_to_configure.username.value_or(::EMPTY_VALUE));
                        target.set_chap_password(attributes_to_configure.password.value_or(::EMPTY_VALUE));
                        context->configuration->add_iscsi_target(target_id, target);
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

                        auto target_id = get_iscsi_target_id_for_endpoint(context, endpoint_uuid);
                        auto target = context->configuration->get_iscsi_targets().at(target_id);

                        log_debug("storage-agent",
                                  "Updating existing CHAP One-way credentials in iSCSI Target: "
                                      << target.get_target_iqn() << " and initiator endpoint: " << uuid);

                        update_password_for_chap_account(context, manager, target.get_target_id(),
                                                         attributes_to_configure.username,
                                                         attributes_to_configure.password,
                                                         false, only_bind);

                        only_bind = true; // Account was created, next time only bind already existing account
                        target.set_chap_username(attributes_to_configure.username.value_or(::EMPTY_VALUE));
                        target.set_chap_password(attributes_to_configure.password.value_or(::EMPTY_VALUE));
                        context->configuration->add_iscsi_target(target_id, target);
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

                    auto target_id = get_iscsi_target_id_for_endpoint(context, endpoint_uuid);
                    auto target = context->configuration->get_iscsi_targets().at(target_id);
                    log_debug("storage-agent",
                              "Adding new CHAP One-way credentials to iSCSI Target: "
                                  << target.get_target_iqn() << " and initiator endpoint: " << uuid);

                    add_chap_account_to_target(manager, target.get_target_id(),
                                               attributes_to_configure.username,
                                               attributes_to_configure.password,
                                               false, only_bind);

                    only_bind = true; // Account was created, next time only bind already existing account
                    target.set_chap_username(attributes_to_configure.username.value_or(::EMPTY_VALUE));
                    target.set_chap_password(attributes_to_configure.password.value_or(::EMPTY_VALUE));
                    context->configuration->add_iscsi_target(target_id, target);
                }
                update_persistent_attributes(uuid, attributes_to_configure.username);
            }
            endpoint->set_username(attributes_to_configure.username);
            endpoint->set_password(attributes_to_configure.password);
        }
        catch (const agent_framework::exceptions::GamiException& ex) {
            response.add_status({literals::Endpoint::USERNAME, ex.get_error_code(), ex.get_message()});
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


void process_endpoint_auth_mutual_chap(AgentContext::SPtr context,
                                       const Uuid& uuid,
                                       const ConfigurableEndpointAttributes& attributes_to_configure,
                                       SetComponentAttributes::Response& response) {

    auto endpoint = get_manager<Endpoint>().get_entry_reference(uuid);
    tgt::Manager manager{context->configuration->get_tgt_socket()};

    // Set Mutual CHAP credentials only in given Target Endpoint
    if (endpoint_represents_iscsi_target(uuid)) {
        try {
            auto target_id = get_iscsi_target_id_for_endpoint(context, uuid);
            auto target = context->configuration->get_iscsi_targets().at(target_id);

            if (endpoint->get_username().has_value() && endpoint->get_username().value() != EMPTY_VALUE) {
                if (attributes_to_configure.username.value() != endpoint->get_username().value()) {
                    log_debug("storage-agent", "Replacing existing CHAP Mutual credentials in iSCSI Target: "
                        << target.get_target_iqn() << " and target endpoint: " << uuid);

                    check_username_usage(attributes_to_configure.username);
                    replace_chap_account_in_target(context, manager, target.get_target_id(),
                                                   target.get_mutual_chap_username(),
                                                   attributes_to_configure.username,
                                                   attributes_to_configure.password, true);
                }
                else {
                    log_debug("storage-agent", "Updating existing CHAP Mutual credentials in iSCSI Target: "
                        << target.get_target_iqn() << " and target endpoint: " << uuid);

                    update_password_for_chap_account(context, manager, target.get_target_id(),
                                                     attributes_to_configure.username,
                                                     attributes_to_configure.password, true);
                }
            }
            else {
                log_debug("storage-agent", "Adding new CHAP Mutual credentials to iSCSI Target: "
                    << target.get_target_iqn() << " and target endpoint: " << uuid);

                check_username_usage(attributes_to_configure.username);
                add_chap_account_to_target(manager, target.get_target_id(),
                                           attributes_to_configure.username,
                                           attributes_to_configure.password, true);
            }
            target.set_mutual_chap_username(attributes_to_configure.username.value_or(::EMPTY_VALUE));
            target.set_mutual_chap_password(attributes_to_configure.password.value_or(::EMPTY_VALUE));
        }
        catch (const agent_framework::exceptions::GamiException& ex) {
            response.add_status({literals::Endpoint::USERNAME, ex.get_error_code(), ex.get_message()});
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

void agent::storage::command::process_endpoint_attributes(AgentContext::SPtr context,
                                                          const Uuid& uuid,
                                                          const attribute::Attributes& attributes,
                                                          SetComponentAttributes::Response& response) {

    ConfigurableEndpointAttributes attributes_to_configure{};
    requests::validation::CommonValidator::validate_set_endpoint_attributes(attributes);
    if (attributes.empty()) {
        log_info("storage-agent", "Nothing has been changed (empty request)");
        return;
    }

    // read parameters from command
    get_endpoint_attributes(attributes, attributes_to_configure, response);
    if (!response.get_statuses().empty()) {
        return; // Do not continue if there are errors in arguments to set
    }

    if (!attributes_to_configure.username.has_value()) {
        THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
              "Username must be specified or null");
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
        process_endpoint_auth_null(context, uuid, response);
    }
    else if (Endpoint::is_in_role(uuid, enums::EntityRole::Initiator)) {
        process_endpoint_auth_chap(context, uuid, attributes_to_configure, response);

    }
    else if (Endpoint::is_in_role(uuid, enums::EntityRole::Target)) {
        process_endpoint_auth_mutual_chap(context, uuid, attributes_to_configure, response);
    }
}
