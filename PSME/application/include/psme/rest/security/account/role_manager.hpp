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
 * @file role_manager.hpp
 *
 * @brief Declaration of RoleManager class.
 * */

#pragma once



#include "agent-framework/generic/singleton.hpp"
#include "psme/rest/security/account/role.hpp"



namespace psme {
namespace rest {
namespace security {
namespace role {

/*!
 * RoleManager class declaration.
 */
class RoleManager : public agent_framework::generic::Singleton<RoleManager> {

public:
    /*!
     * @brief Callback prototype for for_each()
     */
    using RoleCallback = std::function<void(const Role&)>;


    /*!
     * @brief Default constructor
     */
    RoleManager();


    /*!
     * @brief Destructor
     */
    virtual ~RoleManager();


    /*!
     * @brief Get role by role id
     *
     * @param role_id Role id
     * @return Role object
     */
    const Role& get(const std::string& role_id) const;


    /*!
     * @brief Visit all roles kept by the manager
     * @param handle Callback to be called on each role
     * @warning Not allowed to use any RoleManager methods.
     */
    void for_each(const RoleCallback& handle) const;



private:

    using RoleMap = std::map<std::string, Role>;


    RoleMap m_roles{};
    mutable std::mutex m_mutex{};
};

}
}
}
}