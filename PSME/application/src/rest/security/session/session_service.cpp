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
 * @file session_service.cpp
 *
 * @brief SessionService implementation
 *
 * */

#include "psme/rest/security/session/session_service.hpp"
#include "psme/rest/security/session/session_manager.hpp"
#include "logger/logger.hpp"



using namespace psme::rest::security::session;
using std::chrono::steady_clock;


void SessionService::start() {
    log_info("rest", "Starting REST session service ...");
    if (!is_running()) {
        Thread::start();
        log_info("rest", "REST session service started.");
    }
}


void SessionService::stop() {
    log_info("rest", "Stopping REST session service ...");
    if (is_running()) {
        Thread::stop();
        log_info("rest", "REST session service stopped.");
    }
}


SessionService::~SessionService() {
    stop();
}


void SessionService::handle_sessions() {
    while (is_running()) {

        auto start_point = steady_clock::now();

        try {
            SessionManager::get_instance()->remove_outdated_sessions();
        }
        catch (const std::runtime_error& e) {
            log_error("rest",
                      " Exception occurred when cleaning the session container: " << e.what());
        }
        catch (...) {
            log_error("rest",
                      " Exception occurred when cleaning the session container.");
        }
        auto sleep_period = m_session_clenup_interval - (steady_clock::now() - start_point);

        std::this_thread::sleep_for(sleep_period);
    }
}
