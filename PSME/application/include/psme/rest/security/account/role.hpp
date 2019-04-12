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
 * @file role.hpp
 *
 * @brief Declaration of role class entity.
 * */

#pragma once



#include "json-wrapper/json-wrapper.hpp"
#include <string>
#include <vector>
#include "agent-framework/module/enum/enum_builder.hpp"



namespace psme {
namespace rest {
namespace security {
namespace role {

/*!
 * @brief PrivilegeType Type of privilege
 */
ENUM(PrivilegeType, uint32_t,
    Login,
    ConfigureManager,
    ConfigureUsers,
    ConfigureSelf,
    ConfigureComponents
);


/*! forward declarations */
class RoleManager;

/*!
 * @brief Role representation
 */
class Role final {
public:

    friend class RoleManager;


    /*!
     * @brief Set role id
     *
     * @param id role id
     */
    void set_id(const std::string& id) {
        m_id = id;
    }


    /*!
     * @brief Get role id
     *
     * @return role id
     */
    const std::string& get_id() const {
        return m_id;
    }

    /*!
     * @brief Check if role is predefined
     *
     * @return true is role is predefined, false otherwise
     */
    bool is_predefined() const {
        return m_is_predefined;
    }


    /*!
     * @brief Set role predefined property
     *
     * @param enabled flag which indicates this role is predefined
     */
    void set_is_predefined(bool is_predefined) {
        m_is_predefined = is_predefined;
    }


    /*!
     * @brief Get role id
     *
     * @return name of the role for this role
     */
    const std::string& get_role_id() const {
        return m_role_id;
    }


    /*!
     * @brief Set role id
     *
     * @param role_id name of the role for this role
     */
    void set_role_id(const std::string& role_id) {
        m_role_id = role_id;
    }


    /*!
     * @brief Get privileges list
     *
     * @return list of the privileges for this role
     */
    const std::vector<std::string>& get_privileges() const {
        return m_privileges;
    }


    /*!
     * @brief Add privilege to list
     *
     * @param privilege name to add
     */
    void add_privilege(const std::string& privilege) {
        m_privileges.push_back(privilege);
    }


    /*!
     * @brief Creates json representation of role
     *
     * @return JSON representation of role
     */
    json::Json to_json() const;


    /*!
     * @brief Fills json with representation of role
     *
     * @param json JSON to be filled with representation of role
     */
    void fill_json(json::Json& json) const;


private:

    bool m_is_predefined{true};
    std::string m_id{};
    std::string m_role_id{};

    std::vector<std::string> m_privileges{};


};

}
}
}
}