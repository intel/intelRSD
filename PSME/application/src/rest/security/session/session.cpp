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
 * @file session.cpp
 *
 * @brief Implementation of session resource
 * */


#include "psme/rest/security/session/session.hpp"
#include "psme/rest/constants/constants.hpp"



using namespace psme::rest::constants;

namespace psme {
namespace rest {
namespace security {
namespace session {

json::Json Session::to_json() const {
    json::Json j = json::Json();
    fill_json(j);
    return j;
}


void Session::fill_json(json::Json& json) const {
    json[constants::Common::NAME] = json::Json::value_t::null;
    json[constants::Common::DESCRIPTION] = json::Json::value_t::null;
    json[constants::Common::ID] = m_id;
    json[constants::Session::USER_NAME] = m_user_name;
    json[constants::Session::PASSWORD] = json::Json::value_t::null;
    json[constants::Session::OEM] = json::Json::value_t::array;
}


Session Session::from_json(const json::Json& json) {
    const auto& user_name = json[constants::Session::USER_NAME].get<std::string>();

    Session session{};
    session.set_user_name(user_name);

    return session;
}


bool Session::is_session_valid(const std::chrono::seconds& session_timeout) const {
    return get_last_used() + session_timeout >= std::chrono::steady_clock::now();
}


bool operator==(const Session& s1, const Session& s2) {
    return s1.get_user_name() == s2.get_user_name() &&
           s1.get_origin_header() == s2.get_origin_header();
}

}
}
}
}
