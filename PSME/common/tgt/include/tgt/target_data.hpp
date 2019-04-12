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
 * @file target_data.hpp
 * @brief Target data interface
 */

#pragma once



#include "lun_data.hpp"

#include <vector>
#include <memory>
#include <string>



namespace tgt {

/*! Class represents tgt target model data */
class TargetData {
public:
    using Id = std::int32_t;


    /*!
     * @brief Set target id
     * @param target_id Target id
     */
    void set_target_id(const Id target_id);


    /*!
     * @brief Get target id
     * @return Target id
     */
    Id get_target_id() const;


    /*!
     * @brief Set target iqn
     * @param target_iqn Target iqn
     */
    void set_target_iqn(const std::string& target_iqn);


    /*!
     * @brief Get target iqn
     * @return Target iqn
     */
    const std::string& get_target_iqn() const;


    /*!
     * @brief Set target initiator IQN
     * @param initiator Target initiator IQN
     */
    void set_target_initiator(const std::string& initiator);


    /*!
     * @brief Get target initiator IQN
     * @return Target initiator IQN
     */
    const std::string& get_target_initiator() const;


    /*!
     * @brief Set authentication method
     * @param authentication_method Authentication method
     */
    void set_authentication_method(const std::string& authentication_method);


    /*!
     * @brief Get authentication method
     * @return Authentication method
     */
    const std::string& get_authentication_method() const;


    /*!
     * @brief Set chap username
     * @param username Chap username
     */
    void set_chap_username(const std::string& username);


    /*!
     * @brief Get chap username
     * @return Chap username
     */
    const std::string& get_chap_username() const;


    /*!
     * @brief Set mutual chap username
     * @param username Mutual chap username
     */
    void set_mutual_chap_username(const std::string& username);


    /*!
     * @brief Get mutual chap username
     * @return Mutual chap username
     */
    const std::string& get_mutual_chap_username() const;


    /*!
     * @brief Set chap password
     * @param password Chap password
     */
    void set_chap_password(const std::string& password);


    /*!
     * @brief Set mutual chap password
     * @param password Mutual chap password
     */
    void set_mutual_chap_password(const std::string& password);


    /*!
     * @brief Add lun data object to list
     * @param lun LunData object
     */
    void add_lun_data(std::shared_ptr<LunData> lun);


    /*!
     * @brief Get luns list
     * @return Luns data list
     */
    const LunDataCollection& get_luns() const;


private:
    Id m_target_id{};
    std::string m_target_iqn{};
    std::string m_target_initiator{};
    std::string m_authentication_method{};
    std::string m_chap_username{};
    std::string m_chap_password{};
    std::string m_mutual_chap_username{};
    std::string m_mutual_chap_password{};
    LunDataCollection m_luns{};
};

using TargetDataCollection = std::vector<TargetData>;

}
