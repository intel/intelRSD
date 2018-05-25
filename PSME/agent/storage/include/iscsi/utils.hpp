/*!
 * @copyright
 * Copyright (c) 2015-2018 Intel Corporation
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
 * @brief tgt utils
*/

#pragma once



#include "agent-framework/module/storage_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "iscsi/manager.hpp"



namespace agent {
namespace storage {
namespace iscsi {
namespace tgt {

/*! Request lld max size */
static constexpr unsigned LLD_SIZE = 64;

/*! TGT available operations */
enum class Operation {
    NEW,
    DELETE,
    SHOW,
    BIND,
    UNBIND,
    UPDATE,
    STATS,
    START,
    STOP
};

/*! TGT available modes */
enum class Mode {
    SYSTEM,
    TARGET,
    DEVICE,
    PORTAL,
    LLD,
    SESSION,
    CONNECTION,
    ACCOUNT
};


/*!
 * @brief Create a new CHAP account and bind to a target
 * @param manager TGT manager
 * @param target_id Target id
 * @param username_new CHAP username which will be created and bound
 * @param password_new CHAP password for username_new account
 * @param mutual True if CHAP account should be bound as mutual,otherwise one-way
 * @param only_bind True if CHAP account already exists and should be only bound to a target
 * */
void add_chap_account_to_target(agent::storage::iscsi::tgt::Manager& manager, const std::int32_t target_id,
                                const std::string& username_new, const std::string& password_new, bool mutual,
                                bool only_bind = false);


/*!
 * @brief Bind an exising CHAP account to a target
 * @param manager TGT manager
 * @param target_id Target id
 * @param username_existing CHAP username which will be bound
 * @param mutual True if CHAP account should be bound as mutual, otherwise one-way
 * */
void add_existing_chap_account_to_target(agent::storage::iscsi::tgt::Manager& manager, const std::int32_t target_id,
                                         const std::string& username_existing, bool mutual);


/*!
 * @brief Unbound a CHAP account from a target/targets and delete the account
 * @param manager TGT manager
 * @param username CHAP username which will be unbound and removed
 * */
void delete_chap_account(agent::storage::iscsi::tgt::Manager& manager, const std::string& username);


/*!
 * @brief Replace CHAP account in a target (unbinb/detele an old one, create/bind a new one to the same target)
 * @param manager TGT manager
 * @param target_id Target id
 * @param username_old CHAP username which will be unbound and removed
 * @param username_new CHAP username which will be created and bound
 * @param password_new CHAP password for username_new account
 * @param mutual True if CHAP account should be bound as mutual,otherwise one-way
 * @param only_bind True if CHAP account already exists and should be only bound to a target
 * */
void replace_chap_account_in_target(agent::storage::iscsi::tgt::Manager& manager, const std::int32_t target_id,
                                    const std::string& username_old, const std::string& username_new,
                                    const std::string& password_new, bool mutual, bool only_bind = false);


/*!
 * @brief Update CHAP account password (unbinb/detele an old one, create/bind a new one with the same username)
 * @param manager TGT manager
 * @param target_id Target id
 * @param username CHAP username of an account which password will be changed
 * @param password New CHAP password
 * @param mutual True if CHAP account should be bound as mutual,otherwise one-way
 * @param only_bind True if CHAP account already exists and should be only bound to a target
 * */
void update_password_for_chap_account(agent::storage::iscsi::tgt::Manager& manager, const std::int32_t target_id,
                                      const std::string& username, const std::string& password, bool mutual,
                                      bool only_bind = false);


/*!
 * @brief Remove iSCSI Target from tgt daemon
 * @param target_uuid Target UUID
 * */
void remove_iscsi_target(const Uuid& target_uuid);


/*!
 * @brief Add iSCSI Target to tgt daemon
 * @param target_endpoint_uuid Target Endpoint UUID
 * @param skip_validation If Endpoint was checked earlier the additional validation can be skipped
 * */
void add_iscsi_target(const Uuid& target_endpoint_uuid, bool skip_validation = false);


/*!
 * @brief When Storage Agent is restarted the password is lost, but if the username is currently used in one of tgt targets
 * then tgt remembers the password and the username can be reused.
 * @param username Username to be checked
 * @return True if username already exists and can be reused, otherwise false
 * */
bool validate_user_without_pass(const std::string& username);


/*!
 * @brief Verifies if iSCSI Target can be created from Endpoint
 * @param endpoint_uuid Target Endpoint UUID
 * @return True if can be created, otherwise false
 * */
bool is_endpoint_valid_for_iscsi(const Uuid& endpoint_uuid);


/*!
 * @brief Verifies if iSCSI Target can be created from Endpoint with its CHAP credentials
 * @param endpoint_uuid Target Endpoint UUID
 * @return True if can be created, otherwise false
 * */
bool is_endpoint_valid_for_chap(const Uuid& endpoint_uuid);


/*!
 * @brief Verifies if given Endpoints can be added to a Zone
 * @param endpoint_uuids Array of Endpoint UUIDs
 * */
void check_endpoints_added_to_zone(const agent_framework::model::attribute::Array<Uuid>& endpoint_uuids);


/*!
 * @brief Creates iSCSI Targets from given Endpoints
 * @param endpoint_uuids Array of Endpoint UUIDs
 * */
void create_iscsi_targets_from_endpoints(const agent_framework::model::attribute::Array<Uuid>& endpoint_uuids);


/*!
 * @brief Removes iSCSI Targets from given Endpoints
 * @param endpoint_uuids Array of Endpoint UUIDs
 * */
void remove_iscsi_targets_from_endpoints(const agent_framework::model::attribute::Array<Uuid>& endpoint_uuids);


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
}
