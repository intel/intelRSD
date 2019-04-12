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
 * @file account.hpp
 *
 * @brief Declaration of account class entity.
 * */

#pragma once



#include "json-wrapper/json-wrapper.hpp"
#include <string>
#include <vector>



namespace psme {
namespace rest {
namespace security {
namespace account {

/*! forward declarations */
class AccountManager;

/*!
 * @brief Account representation
 */
class Account final {
public:

    friend class AccountManager;


    /*!
     * @brief Set account id
     *
     * @param id account id
     */
    void set_id(const uint64_t id) {
        m_id = id;
    }


    /*!
     * @brief Get account id
     *
     * @return account id
     */
    uint64_t get_id() const {
        return m_id;
    }


    /*!
     * @brief Set user name of the account
     *
     * @param user_name account user name
     */
    void set_user_name(const std::string& user_name) {
        m_user_name = user_name;
    }


    /*!
     * @brief Get user name of the account
     *
     * @return account user name
     */
    const std::string& get_user_name() const {
        return m_user_name;
    }


    /*!
     * @brief Check if account is enabled
     *
     * @return true is account is enabled, false otherwise
     */
    bool is_enabled() const {
        return m_enabled;
    }


    /*!
     * @brief Set account enabled property
     *
     * @param enabled flag which indicates this account is enabled
     */
    void set_enabled(bool enabled) {
        m_enabled = enabled;
    }


    /*!
     * @brief Get password hash
     *
     * @return password hash for this account
     */
    const std::string& get_password_hash() const {
        return m_password_hash;
    }


    /*!
     * @brief Set password hash
     *
     * @param password_hash hash of the password for this account
     */
    void set_password_hash(const std::string& password_hash) {
        m_password_hash = password_hash;
    }


    /*!
     * @brief Get role id
     *
     * @return id of the role for this account
     */
    const std::string& get_role_id() const {
        return m_role_id;
    }


    /*!
     * @brief Set role id
     *
     * @param role_id id of the role for this account
     */
    void set_role_id(const std::string& role_id) {
        m_role_id = role_id;
    }


    /*!
     * @brief Check if this account is locked
     *
     * @return true if account is locked, false otherwise
     */
    bool is_locked() const {
        return m_locked;
    }


    /*!
     * @brief Set account locked property
     *
     * @param locked flag which indicates if this account is locked
     */
    void set_locked(bool locked) {
        m_locked = locked;
    }


    /*!
     * @brief Creates json representation of account
     *
     * @return JSON representation of account
     */
    json::Json to_json() const;


    /*!
     * @brief Fills json with representation of account
     *
     * @param json JSON to be filled with representation of account
     */
    void fill_json(json::Json& json) const;


private:

    bool m_enabled{true};
    uint64_t m_id{};
    std::string m_user_name{"Administrator"};
    std::string m_password_hash{""};
    std::string m_role_id{"Administrator"};
    bool m_locked{false};
};

}
}
}
}
