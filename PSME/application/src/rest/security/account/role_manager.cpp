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
 * @file role_manager.cpp
 * */

#include "psme/rest/security/account/role_manager.hpp"
#include "psme/rest/server/error/server_exception.hpp"
#include "configuration/configuration.hpp"
#include "psme/rest/constants/constants.hpp"

using namespace psme::rest;
using namespace psme::rest::security::role;

RoleManager::RoleManager() {
    Role role;
    role.set_id(constants::Common::ADMINISTRATOR);
    role.set_role_id(role.get_id());
    for (const auto assigned_privilege : PrivilegeType::get_values()) {
        role.add_privilege(assigned_privilege);
    }
    m_roles.insert(std::pair<std::string,Role>(constants::Common::ADMINISTRATOR,role));
}


RoleManager::~RoleManager() {}

const Role& RoleManager::get(const std::string& role_id) const {
    std::lock_guard<std::mutex> lock{m_mutex};
    auto it = m_roles.find(role_id);
    if (it == m_roles.end()) {
        throw agent_framework::exceptions::NotFound(std::string{"Role (ID: "}
                                                    + role_id + ") not found.");
    }
    return it->second;
}


void RoleManager::for_each(const RoleCallback& handle) const {
    std::lock_guard<std::mutex> lock{m_mutex};
    for (const auto& entry : m_roles) {
        handle(entry.second);
    }
}

