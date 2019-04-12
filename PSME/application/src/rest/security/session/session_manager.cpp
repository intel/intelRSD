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
 * @file session_manager.cpp
 * */

#include "psme/rest/security/session/session_manager.hpp"
#include "psme/rest/security/session/session_service_manager.hpp"
#include "psme/rest/server/error/server_exception.hpp"
#include "uuid/uuid.hpp"
#include <fstream>


namespace psme {
namespace rest {
namespace security {
namespace session {

SessionManager::~SessionManager() {}


const Session& SessionManager::get(uint64_t session_id) const {
    std::lock_guard<std::mutex> lock{m_mutex};
    auto it = m_sessions.find(session_id);
    if (it == m_sessions.end()) {
        throw agent_framework::exceptions::NotFound(std::string{"Session (ID: "}
                                                    + std::to_string(session_id) + ") not found.");
    }
    return it->second;
}


void SessionManager::for_each(const SessionCallback& handle) const {
    std::lock_guard<std::mutex> lock{m_mutex};
    for (const auto& entry : m_sessions) {
        handle(entry.second);
    }
}


void SessionManager::update_next_id(void) {
    std::uint64_t old_id = m_id;
    while (m_sessions.count(++m_id) != 0) {
        /* prevent from infinite loop when all id's are in use */
        if (m_id == old_id) {
            // TODO replace InvalidValue with more relevant exception type
            throw agent_framework::exceptions::InvalidValue(
                std::string("SessionManager error: number of sessions exceeded."));
        }
    }
}


uint64_t SessionManager::add(Session session) {
    std::lock_guard<std::mutex> lock{m_mutex};

    /* find first not used session ID */
    update_next_id();
    session.set_id(m_id);
    session.set_session_auth_token(make_v1_uuid());
    session.update_last_used();
    m_sessions[m_id] = std::move(session);

    return m_id;
}


void SessionManager::del(uint64_t subscription_id) {
    std::lock_guard<std::mutex> lock{m_mutex};
    auto it = m_sessions.find(subscription_id);
    if (it == m_sessions.end()) {
        throw agent_framework::exceptions::NotFound(std::string{"Subscription (ID: "}
                                                    + std::to_string(subscription_id) + ") not found.");
    }
    m_sessions.erase(it);
}


void SessionManager::remove_outdated_sessions(void) {
    std::lock_guard<std::mutex> lock{m_mutex};

    auto it = m_sessions.begin();
    auto endIter = m_sessions.end();

    while (it != endIter) {
        if (it->second.get_last_used() +
            std::chrono::seconds(SessionServiceManager::get_instance()->get_session_timeout()) <
            std::chrono::steady_clock::now()) {
            m_sessions.erase(it++);
        }
        else {
            ++it;
        }
    }
}


bool SessionManager::is_session_valid(const std::string& token, const std::string& http_origin) {
    std::lock_guard<std::mutex> lock{m_mutex};
    auto it = std::find_if(begin(m_sessions), end(m_sessions), [&token, &http_origin](auto& pair) {
        return pair.second.get_session_auth_token() == token && pair.second.get_origin_header() == http_origin;
    });

    if (it != std::end(m_sessions)) {
        if (it->second.is_session_valid(SessionServiceManager::get_instance()->get_session_timeout())) {
            m_sessions[it->first].update_last_used();
            return true;
        }
        else {
            m_sessions.erase(it);
        }
    }
    return false;
}

}
}
}
}
