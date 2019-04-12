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
 * @file account.cpp
 *
 * @brief Implementation of account entity
 * */


#include "psme/rest/security/account/account.hpp"
#include "psme/rest/constants/constants.hpp"



using namespace psme::rest::constants;

namespace psme {
namespace rest {
namespace security {
namespace account {

json::Json Account::to_json() const {
    json::Json j = json::Json();
    fill_json(j);
    return j;
}


void Account::fill_json(json::Json& json) const {
    json[constants::Common::NAME] = json::Json::value_t::null;
    json[constants::Common::DESCRIPTION] = json::Json::value_t::null;
    json[constants::Common::ID] = m_id;
    json[constants::Account::ENABLED] = m_enabled;
    json[constants::Account::PASSWORD] = json::Json::value_t::null;
    json[constants::Account::USER_NAME] = m_user_name;
    json[constants::Account::ROLE_ID] = m_role_id;
    json[constants::Account::LOCKED] = m_locked;
}

}
}
}
}
