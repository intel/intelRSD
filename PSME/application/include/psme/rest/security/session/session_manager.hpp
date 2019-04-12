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
 * @file session_manager.hpp
 *
 * @brief Declaration of SessionManager class.
 * */

#pragma once



#include "agent-framework/generic/singleton.hpp"
#include "psme/rest/security/session/session.hpp"
#include <mutex>
#include <map>



namespace psme {
namespace rest {
namespace security {
namespace session {

/*!
 * SessionManager class declaration.
 */
class SessionManager : public agent_framework::generic::Singleton<SessionManager> {
public:
    /*!
     * @brief Callback prototype for for_each()
     */
    using SessionCallback = std::function<void(const Session&)>;


    /*!
     * @brief Default constructor
     *
     * Session manager keeps tracking sessions. Sessions are not persistent.
     */
    SessionManager() = default;


    /*!
     * @brief Destructor
     */
    virtual ~SessionManager();


    /*!
     * @brief Visit all sessions kept by the manager
     * @param handle Callback to be called on each session
     * @warning Not allowed to use any SessionManager methods.
     */
    void for_each(const SessionCallback& handle) const;


    /*!
     * @brief Get session by session id
     *
     * @param session_id Session id
     * @return Session
     */
    const Session& get(uint64_t session_id) const;


    /*!
     * @brief Add session
     *
     * @param session Session
     */
    uint64_t add(Session session);


    /*!
     * @brief Removes session by session id
     *
     * @param session_id Session id
     */
    void del(uint64_t session_id);


    /*!
     * @brief Check if session given by token and http_origin is valid.
     *
     * @param token unique Session token exchanged between client and server.
     * @param http_origin content of the http Origin header provided with request.
     * @return true if session is valid, false otherwise.
     */
    bool is_session_valid(const std::string& token, const std::string& http_origin);


    /*!
     * Removes sessions which exceeded session timeout.
     */
    void remove_outdated_sessions(void);


private:
    using SessionMap = std::map<std::uint64_t, Session>;


    /*!
     * Update m_id to next available value.
     */
    void update_next_id(void);


    SessionMap m_sessions{};
    mutable std::mutex m_mutex{};

    /*! @brief Last assigned ID */
    std::uint64_t m_id{};
};

}
}
}
}
