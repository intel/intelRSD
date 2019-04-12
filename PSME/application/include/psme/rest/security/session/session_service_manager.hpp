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
 * @file session_service_manager.hpp
 *
 * @brief Declaration of SessionServiceManager class.
 * */

#pragma once



#include "agent-framework/generic/singleton.hpp"
#include "configuration/configuration.hpp"

#include <chrono>
#include <mutex>



namespace psme {
namespace rest {
namespace security {
namespace session {

class SessionServiceManager : public agent_framework::generic::Singleton<SessionServiceManager> {
public:

    /*!
     * @brief Session timeout property
     */
    static constexpr char SESSION_TIMEOUT[] = "session-timeout";

    /*!
     * @brief Session service enabled property
     */
    static constexpr char SERVICE_ENABLED[] = "service-enabled";


    /*!
     * SessionServiceManager constructor.
     */
    SessionServiceManager();


    /*!
     * @return number of seconds which is session timeout for SessionService.
     */
    const std::chrono::seconds& get_session_timeout() const;


    /*!
     * Setter for session timeout property in SessionService.
     * @param session_timeout number of seconds for SessionService session timeout.
     */
    void set_session_timeout(const std::chrono::seconds& session_timeout);


    /*!
     * Checks if SessionService is enabled.
     * @return true if SessionService enabled, false otherwise.
     */
    bool is_service_enabled() const;


    /*!
     * Setter for service enabled property.
     * @param service_enabled service enabled flag.
     */
    void set_service_enabled(bool service_enabled);


    const uint64_t m_default_session_timeout = 30;
    const bool m_default_service_enabled = false;

private:
    std::chrono::seconds m_session_timeout{};
    bool m_service_enabled{};

    mutable std::mutex m_mutex{};
};

}
}
}
}
