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
 * @file utils.hpp
 * @brief TGT-related utils
*/

#pragma once



#include "agent-framework/module/storage_components.hpp"

#include "tgt/target_data.hpp"
#include "tgt/iscsi_data.hpp"
#include "tgt/manager.hpp"

#include "storage_agent_context.hpp"


namespace agent {
namespace storage {
namespace utils {

/*!
 * @brief Create a new CHAP account and bind to a target
 * @param manager TGT manager
 * @param target_id Target id
 * @param username_new CHAP username which will be created and bound
 * @param password_new CHAP password for username_new account
 * @param mutual True if CHAP account should be bound as mutual,otherwise one-way
 * @param only_bind True if CHAP account already exists and should be only bound to a target
 * */
void add_chap_account_to_target(tgt::Manager& manager,
                                const tgt::TargetData::Id target_id,
                                const std::string& username_new,
                                const std::string& password_new,
                                bool mutual,
                                bool only_bind = false);


/*!
 * @brief Bind an existing CHAP account to a target
 * @param manager TGT manager
 * @param target_id Target id
 * @param username_existing CHAP username which will be bound
 * @param mutual True if CHAP account should be bound as mutual, otherwise one-way
 * */
void add_existing_chap_account_to_target(tgt::Manager& manager,
                                         const tgt::TargetData::Id target_id,
                                         const std::string& username_existing,
                                         bool mutual);


/*!
 * @brief Unbound a CHAP account from a target/targets and delete the account
 * @param context Agent context object
 * @param manager TGT manager
 * @param username CHAP username which will be unbound and removed
 * */
void delete_chap_account(AgentContext::SPtr context, tgt::Manager& manager, const std::string& username);


/*!
 * @brief Replace CHAP account in a target (unbind/delete an old one, create/bind a new one to the same target)
 * @param context Agent context object
 * @param manager TGT manager
 * @param target_id Target id
 * @param username_old CHAP username which will be unbound and removed
 * @param username_new CHAP username which will be created and bound
 * @param password_new CHAP password for username_new account
 * @param mutual True if CHAP account should be bound as mutual,otherwise one-way
 * @param only_bind True if CHAP account already exists and should be only bound to a target
 * */
void replace_chap_account_in_target(AgentContext::SPtr context,
                                    tgt::Manager& manager,
                                    const tgt::TargetData::Id target_id,
                                    const std::string& username_old,
                                    const std::string& username_new,
                                    const std::string& password_new,
                                    bool mutual,
                                    bool only_bind = false);


/*!
 * @brief Update CHAP account password (unbind/delete an old one, create/bind a new one with the same username)
 * @param context Agent context object
 * @param manager TGT manager
 * @param target_id Target id
 * @param username CHAP username of an account which password will be changed
 * @param password New CHAP password
 * @param mutual True if CHAP account should be bound as mutual,otherwise one-way
 * @param only_bind True if CHAP account already exists and should be only bound to a target
 * */
void update_password_for_chap_account(AgentContext::SPtr context,
                                      tgt::Manager& manager,
                                      const tgt::TargetData::Id target_id,
                                      const std::string& username,
                                      const std::string& password,
                                      bool mutual,
                                      bool only_bind = false);


/*!
 * @brief Remove iSCSI Target from tgt daemon
 * @param context Agent context object
 * @param target_id Target ID
 * */
void remove_iscsi_target(AgentContext::SPtr context, const tgt::TargetData::Id& target_id);


/*!
 * @brief Add iSCSI Target to tgt daemon
 * @param context Agent context object
 * @param target_endpoint_uuid Target Endpoint UUID
 * @param skip_validation If Endpoint was checked earlier the additional validation can be skipped
 * */
void add_iscsi_target(AgentContext::SPtr context,
                      const Uuid& target_endpoint_uuid,
                      bool skip_validation = false);


/*!
 * @brief Validates user without password.
 *
 * When Storage Agent is restarted the password is lost,
 * but if the username is currently used in one of tgt targets
 * then tgt remembers the password and the username can be reused.
 *
 * @param context Agent context object
 * @param username Username to be checked
 * @return True if username already exists and can be reused, otherwise false
 * */
bool validate_user_without_pass(AgentContext::SPtr context, const std::string& username);


/*!
 * @brief Verifies if iSCSI Target can be created from Endpoint
 * @param endpoint_uuid Target Endpoint UUID
 * @return True if can be created, otherwise false
 * */
bool is_endpoint_valid_for_iscsi(const Uuid& endpoint_uuid);


/*!
 * @brief Verifies if iSCSI Target can be created from Endpoint with its CHAP credentials
 * @param context Agent context object
 * @param endpoint_uuid Target Endpoint UUID
 * @return True if can be created, otherwise false
 * */
bool is_endpoint_valid_for_chap(AgentContext::SPtr context, const Uuid& endpoint_uuid);


/*!
 * @brief Verifies if given Endpoints can be added to a Zone
 * @param context Pointer to agent context
 * @param endpoint_uuids Array of Endpoint UUIDs
 * */
void check_endpoints_added_to_zone(AgentContext::SPtr context,
                                   const agent_framework::model::attribute::Array<Uuid>& endpoint_uuids);


/*!
 * @brief Creates iSCSI Targets from given Endpoints
 * @param context Pointer to agent context
 * @param endpoint_uuids Array of Endpoint UUIDs
 * */
void create_iscsi_targets_from_endpoints(AgentContext::SPtr context,
                                         const agent_framework::model::attribute::Array<Uuid>& endpoint_uuids);


/*!
 * @brief Removes iSCSI Targets from given Endpoints
 * @param context Pointer to agent context
 * @param endpoint_uuids Array of Endpoint UUIDs
 * */
void remove_iscsi_targets_from_endpoints(AgentContext::SPtr context,
                                         const agent_framework::model::attribute::Array<Uuid>& endpoint_uuids);


/*!
 * @brief Checks if any Endpoints within the array contains Connected Entity with given Entity role
 * @param endpoint_uuids Array of Endpoint UUIDs
 * @param role Entity role to be found
 * @return True if found, otherwise false
 * */
bool is_role_present(const agent_framework::model::attribute::Array<Uuid>& endpoint_uuids,
                     agent_framework::model::enums::EntityRole role);


/*!
 * @brief Removes CHAP credentials from given Endpoint in model and from persistent attributes database
 * @warning This function does not modify iSCSI TGT Target, it must be handled earlier
 * @param endpoint_uuid Endpoint UUID
 * */
void clean_chap_from_endpoint(const Uuid& endpoint_uuid);


/*!
 * @brief Removes CHAP credentials from given Endpoint from persistent attributes database
 * @param endpoint_uuid Endpoint UUID
 * */
void clean_chap_from_database(const Uuid& endpoint_uuid);


/*!
 * @brief Checks if given Endpoint represents iSCSI Target (is in a Zone with Initiator and Target Endpoint)
 * @param endpoint_uuid Endpoint UUID
 * @return True if represents, otherwise false
 * */
bool endpoint_represents_iscsi_target(const Uuid& endpoint_uuid);


/*!
 * @brief Checks if given Endpoint's Username is already in use in other Endpoints
 * @param username Endpoint's Username
 * @return True if is in use, otherwise false
 * */
bool is_endpoint_username_in_use(const std::string& username);

}
}
}
