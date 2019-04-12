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
 * @file session_service_manager.cpp
 * */


#include "psme/rest/security/session/session_service_manager.hpp"



using namespace psme::rest::security::session;

constexpr char SessionServiceManager::SESSION_TIMEOUT[];
constexpr char SessionServiceManager::SERVICE_ENABLED[];


SessionServiceManager::SessionServiceManager() {
    const json::Json& config = configuration::Configuration::get_instance().to_json();
    const auto& session_service_config = config["session-service"];
    m_session_timeout = std::chrono::seconds(
        session_service_config.value(SESSION_TIMEOUT, std::uint64_t{m_default_session_timeout}));
    m_service_enabled = session_service_config.value(SERVICE_ENABLED, m_default_service_enabled);
}


const std::chrono::seconds& SessionServiceManager::get_session_timeout() const {
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_session_timeout;
}


void SessionServiceManager::set_session_timeout(const std::chrono::seconds& session_timeout) {
    std::lock_guard<std::mutex> lock{m_mutex};
    m_session_timeout = session_timeout;
}


bool SessionServiceManager::is_service_enabled() const {
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_service_enabled;
}


void SessionServiceManager::set_service_enabled(bool service_enabled) {
    std::lock_guard<std::mutex> lock{m_mutex};
    m_service_enabled = service_enabled;
}
