/*!
 * @copyright
 * Copyright (c) 2017 Intel Corporation
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

#include "agent-framework/module/storage_components.hpp"
#include "iscsi/errors.hpp"
#include "iscsi/manager.hpp"
#include "iscsi/response.hpp"
#include "iscsi/utils.hpp"


using namespace agent_framework;
using namespace agent_framework::model;
using namespace agent_framework::model::attribute;
using namespace agent_framework::module;
using namespace agent_framework::module::managers;
using Errors = agent::storage::iscsi::tgt::Errors;


namespace {

void unbind_chap_from_all(agent::storage::iscsi::tgt::Manager& manager,const std::string& username) {
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

}

namespace agent {
namespace storage {
namespace iscsi {
namespace tgt {

void add_chap_account_to_target(agent::storage::iscsi::tgt::Manager& manager, const std::int32_t target_id,
                                    const std::string& username_new, const std::string& password_new, bool mutual){
    create_chap_account(manager, username_new, password_new);
    bind_chap_account(manager, target_id, username_new, mutual);
}

void delete_chap_account(agent::storage::iscsi::tgt::Manager& manager, const std::string& username)
{
    /* TGT can crash if a bound CHAP account is deleted, so we make sure all instances are unbounded */
    unbind_chap_from_all(manager, username);
    auto response_old = manager.delete_chap_account(username);
    if (!response_old.is_valid()) {
        Errors::throw_exception(response_old.get_error(),
                                "Cannot unbind CHAP account from target: ");
    }
}

void replace_chap_account_in_target(agent::storage::iscsi::tgt::Manager& manager, const std::int32_t target_id,
                                     const std::string& username_old, const std::string& username_new,
                                     const std::string& password_new, bool mutual){
    create_chap_account(manager, username_new, password_new);
    delete_chap_account(manager, username_old);
    bind_chap_account(manager, target_id, username_new, mutual);
}

void update_password_for_chap_account(agent::storage::iscsi::tgt::Manager& manager, const std::int32_t target_id,
                                      const std::string& username, const std::string& password, bool mutual){
    delete_chap_account(manager, username);
    create_chap_account(manager, username, password);
    bind_chap_account(manager, target_id, username, mutual);
}


}
}
}
}


