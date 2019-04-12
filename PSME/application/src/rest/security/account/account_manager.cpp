/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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
 * @file account_manager.cpp
 * */

#include "psme/rest/security/account/account_manager.hpp"
#include "psme/rest/server/error/server_exception.hpp"
#include "configuration/configuration.hpp"
#include "utils/crypt_utils.hpp"
#include "utils/conversion.hpp"
#include <algorithm>

using namespace psme::rest::security::account;

constexpr char AccountManager::USER_NAME[];
constexpr char AccountManager::PASSWORD_HASH[];

AccountManager::AccountManager() {
    const json::Json& config = configuration::Configuration::get_instance().to_json();
    const auto& authentication_config = config["authentication"];

    std::string user_name = authentication_config.value(USER_NAME, std::string());
    std::string password_hash = authentication_config.value(PASSWORD_HASH, std::string());

    if (!user_name.empty()) {
        Account account;
        account.set_user_name(user_name);
        account.set_password_hash(password_hash);
        add(account);
    }
}

AccountManager::~AccountManager() {}



bool AccountManager::validate_credentials(const std::string& user_name, const std::string& password) const {
    if (user_name.empty()) {
        return false;
    }

    std::lock_guard<std::mutex> lock{m_mutex};
    auto iter = std::find_if(std::begin(m_accounts), std::end(m_accounts),
                             [this, &user_name, &password](const auto& account) {
                                 // Password hash read from configuration is actually salt and hash concatenated together
                                 std::string salt_and_hash = account.second.get_password_hash();
                                 std::transform(salt_and_hash.begin(), salt_and_hash.end(), salt_and_hash.begin(), ::toupper);

                                 // Salt/hash are given in hex form, therefore 1B = 2 chars in string
                                 auto salt_size = utils::get_salt_size() * 2;
                                 auto hash_size = utils::get_kdf_key_size() * 2;
                                 std::string salt = salt_and_hash.substr(0, salt_size);
                                 std::string password_hash = salt_and_hash.substr(salt_size, hash_size);

                                 return account.second.get_user_name() == user_name &&
                                     password_hash == this->calculate_hash(password, utils::hex_string_to_string(salt));
                             });
    return iter != std::end(m_accounts);
}


std::string AccountManager::calculate_hash(const std::string& password, const std::string& salt) const {
    std::string password_digest = utils::salted_hash(password, salt);
    return utils::string_to_hex_string(password_digest);
}


const Account& AccountManager::get(uint64_t account_id) const {
    std::lock_guard<std::mutex> lock{m_mutex};
    auto it = m_accounts.find(account_id);
    if (it == m_accounts.end()) {
        throw agent_framework::exceptions::NotFound(std::string{"Account (ID: "}
                                                    + std::to_string(account_id) + ") not found.");
    }
    return it->second;
}


uint64_t AccountManager::add(Account account) {
    std::lock_guard<std::mutex> lock{m_mutex};

    /* find first not used account ID */
    update_next_id();
    account.set_id(m_id);
    m_accounts[m_id] = std::move(account);

    return m_id;
}


void AccountManager::for_each(const AccountCallback& handle) const {
    std::lock_guard<std::mutex> lock{m_mutex};
    for (const auto& entry : m_accounts) {
        handle(entry.second);
    }
}


void AccountManager::update_next_id(void) {
    std::uint64_t old_id = m_id;
    while (m_accounts.count(++m_id) != 0) {
        /* prevent from infinite loop when all id's are in use */
        if (m_id == old_id) {
            // TODO replace InvalidValue with more relevant exception type
            throw agent_framework::exceptions::InvalidValue(
                std::string("AccountManager error: number of accounts exceeded."));
        }
    }
}
