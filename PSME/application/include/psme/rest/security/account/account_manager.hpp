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
 * @file account_manager.hpp
 *
 * @brief Declaration of AccountManager class.
 * */

#pragma once



#include "agent-framework/generic/singleton.hpp"
#include "psme/rest/security/account/account.hpp"



namespace psme {
namespace rest {
namespace security {
namespace account {

/*!
 * AccountManager class declaration.
 */
class AccountManager : public agent_framework::generic::Singleton<AccountManager> {

public:
    /*!
     * @brief Callback prototype for for_each()
     */
    using AccountCallback = std::function<void(const Account&)>;


    /*!
     * @brief Default constructor
     */
    AccountManager();


    /*!
     * @brief Destructor
     */
    virtual ~AccountManager();


    /*!
     * @brief Get account by account id
     *
     * @param account_id Account id
     * @return Account object
     */
    const Account& get(uint64_t account_id) const;


    /*!
     * @brief Add account
     *
     * @param account new Account object to add
     */
    uint64_t add(Account account);


    /*!
     * @brief Calculate hash from password
     *
     * @param password from which hash will be calculated
     * @param salt random data used for hash generation
     * @return SHA512 based PBKDF2 hash of the given password
     */
    std::string calculate_hash(const std::string& password, const std::string& salt) const;


    /*!
     * @brief Visit all accounts kept by the manager
     * @param handle Callback to be called on each account
     * @warning Not allowed to use any AccountManager methods.
     */
    void for_each(const AccountCallback& handle) const;


    /*!
     * @brief Validates credentials given as arguments.
     * @param user_name name of the user
     * @param password user password
     * @return true if credentials are valid, false otherwise.
     */
    bool validate_credentials(const std::string& user_name, const std::string& password) const;


private:
    /*!
     * @brief Authentication user name property name
     */
    static constexpr char USER_NAME[] = "username";

    /*!
     * @brief Authentication password hash property name
     */
    static constexpr char PASSWORD_HASH[] = "password";

    using AccountMap = std::map<std::uint64_t, Account>;


    /*!
     * Update m_id to next available value.
     */
    void update_next_id(void);


    AccountMap m_accounts{};
    mutable std::mutex m_mutex{};

    /*! @brief Last assigned ID */
    std::uint64_t m_id{0};
};

}
}
}
}