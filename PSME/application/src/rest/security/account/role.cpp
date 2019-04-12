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
 * @file role.cpp
 *
 * @brief Implementation of role entity
 * */


#include "psme/rest/security/account/role.hpp"
#include "psme/rest/constants/constants.hpp"



using namespace psme::rest::constants;

namespace psme {
namespace rest {
namespace security {
namespace role {

json::Json Role::to_json() const {
    json::Json j = json::Json();
    fill_json(j);
    return j;
}


void Role::fill_json(json::Json& json) const {
    json[constants::Common::NAME] = json::Json::value_t::null;
    json[constants::Common::DESCRIPTION] = json::Json::value_t::null;
    json[constants::Common::ID] = m_id;
    json[constants::Role::ROLE_ID] = m_role_id;
    json[constants::Role::IS_PREDEFINED] = m_is_predefined;
    json[constants::Role::ASSIGNED_PRIVILEGES] = m_privileges;
}

}
}
}
}