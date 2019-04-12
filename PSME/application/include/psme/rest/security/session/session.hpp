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
 * @file session.hpp
 *
 * @brief Declaration of session class entity.
 * */

#pragma once



#include "json-wrapper/json-wrapper.hpp"
#include <string>
#include <vector>
#include <chrono>



namespace psme {
namespace rest {
namespace security {
namespace session {

/*! forward declarations */
class SessionManager;

/*!
 * @brief Session representation
 */
class Session final {
public:

    using Timepoint = std::chrono::steady_clock::time_point;

    friend class SessionManager;


    /*!
     * @brief Set Session id
     *
     * @param id Session id
     */
    void set_id(uint64_t id) {
        m_id = id;
    }


    /*!
     * @brief Get Session id
     *
     * @return Session id
     */
    uint64_t get_id() const {
        return m_id;
    }


    /*!
     * @brief Set user name of the session
     *
     * @param user_name Session user name
     */
    void set_user_name(const std::string& user_name) {
        m_user_name = user_name;
    }


    /*!
     * @brief Get user name of the session
     *
     * @return Session user name
     */
    const std::string& get_user_name() const {
        return m_user_name;
    }


    /*!
     * @brief Set session authentication token
     *
     * @param auth_token session authentication token
     */
    void set_session_auth_token(const std::string& auth_token) {
        m_session_auth_token = auth_token;
    }


    /*!
     * @brief Get session authentication token
     *
     * @return session authentication token
     */
    const std::string& get_session_auth_token() const {
        return m_session_auth_token;
    }


    /*!
    * @brief Set http origin header
    *
    * @param origin_header http origin header from session creation request
    */
    void set_origin_header(const std::string& origin_header) {
        m_origin_header = origin_header;
    }


    /*!
     * @brief Get http origin header from session creation request
     *
     * @return http origin header
     */
    const std::string& get_origin_header() const {
        return m_origin_header;
    }


    /*!
     * @brief Set timepoint of the last session usage
     * @param[in] timepoint Timepoint object representing last session usage time.
     */
    void set_last_used(const Timepoint& timepoint) {
        m_last_used = timepoint;
    }


    /*!
     * @brief Get last session usage timepoint.
     * @return last session usage timepoint.
     */
    const Timepoint get_last_used() const {
        return m_last_used;
    }


    /*!
     * @brief Creates json representation of session
     *
     * @return JSON representation of session
     */
    json::Json to_json() const;


    /*!
     * @brief Fills json with representation of session
     *
     * @param json JSON to be filled with representation of session
     */
    void fill_json(json::Json& json) const;


    /*!
     * @brief Creates model representation from Session JSON
     *
     * @param json JSON representation of session
     * @return Model representation of session
     */
    static Session from_json(const json::Json& json);


    /*!
     * Check if session is valid based on given timeout.
     * @param session_timeout SessionService's timeout for session validity.
     * @return true is session is valid, false if outdated.
     */
    bool is_session_valid(const std::chrono::seconds& session_timeout) const;


private:
    void update_last_used(void) {
        set_last_used(std::chrono::steady_clock::now());
    }


    uint64_t m_id{};
    std::string m_user_name{};
    std::string m_session_auth_token{};
    std::string m_origin_header{};
    Timepoint m_last_used{};
};


/*!
 * @return true if Sessions are equal (except id field)
 * @param s1 First Session
 * @param s2 Second Session
 */
bool operator==(const Session& s1, const Session& s2);

}
}
}
}
