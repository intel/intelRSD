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
* @param manager Tgt manager
* @param target_id Target id
* @param username_new CHAP username which will be created and bound
* @param password_new CHAP password for username_new account
* @param mutual True if CHAP account should be bound as mutual,otherwise one-way
*/
void add_chap_account_to_target(agent::storage::iscsi::tgt::Manager& manager, const std::int32_t target_id,
                                const std::string& username_new, const std::string& password_new, bool mutual);


/*!
* @brief Unbound a CHAP account from a target/targets and delete the account
* @param manager Tgt manager
* @param username CHAP username which will be unbound and removed
*/
void delete_chap_account(agent::storage::iscsi::tgt::Manager& manager, const std::string& username);


/*!
* @brief Replace CHAP account in a target (unbinb/detele an old one, create/bind a new one to the same target)
* @param manager Tgt manager
* @param target_id Target id
* @param username_old CHAP username which will be unbound and removed
* @param username_new CHAP username which will be created and bound
* @param password_new CHAP password for username_new account
* @param mutual True if CHAP account should be bound as mutual,otherwise one-way
*/
void replace_chap_account_in_target(agent::storage::iscsi::tgt::Manager& manager, const std::int32_t target_id,
                                    const std::string& username_old, const std::string& username_new,
                                    const std::string& password_new, bool mutual);


/*!
* @brief Update CHAP account password (unbinb/detele an old one, create/bind a new one with the same username)
* @param manager Tgt manager
* @param target_id Target id
* @param username CHAP username of an account which password will be changed
* @param password New CHAP password
* @param mutual True if CHAP account should be bound as mutual,otherwise one-way
*/
void update_password_for_chap_account(agent::storage::iscsi::tgt::Manager& manager, const std::int32_t target_id,
                                      const std::string& username, const std::string& password, bool mutual);

}
}
}
}
