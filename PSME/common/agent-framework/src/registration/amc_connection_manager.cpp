/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 */

#include "agent-framework/registration/amc_connection_manager.hpp"
#include "agent-framework/registration/amc_client.hpp"

#include "logger/logger.hpp"



using namespace agent_framework::generic;

/*! Default registration interval */
constexpr const AmcConnectionManager::Seconds AmcConnectionManager::DEFAULT_REGISTRATION_INTERVAL;


AmcConnectionManager::AmcConnectionManager(EventDispatcher& event_dispatcher,
                                           const RegistrationData& registration_data) :
    m_event_dispatcher{event_dispatcher}, m_interval(Seconds(registration_data.get_interval())) {

    m_url = "http://" + registration_data.get_ipv4_address()
            + ":" + std::to_string(unsigned(registration_data.get_port()));
    log_debug("registration", "Registration url: " << m_url << " interval: " << m_interval.count() << "s");
    wait_until_registered();
}


void AmcConnectionManager::wait_until_registered() {
    AmcClient client(m_url);
    while (State::REGISTER == m_state) {
        update_connection_state(client);
        std::this_thread::sleep_for(m_interval);
    }
}


void AmcConnectionManager::update_connection_state(const AmcClient& client) {
    try {
        switch (m_state) {
            case State::MONITOR: {
                auto heart_beat = client.heart_beat();
                m_interval = heart_beat.get_min_delay();
                if (heart_beat.get_timestamp() < m_heart_beat) {
                    log_info("registration", "Rest has restarted " << heart_beat.get_timestamp().count()
                                          << "s ago, connect in " << m_interval.count() << "s");
                    m_state = State::REGISTER;
                    m_event_dispatcher.disable_send_notifications();
                }
                else {
                    m_heart_beat = heart_beat.get_timestamp();
                    log_debug("registration", "Rest runs for " << m_heart_beat.count() << "s");
                }
                break;
            }
            case State::REGISTER:
            default: {
                auto attach_result = client.attach();

                auto heart_beat = client.heart_beat();
                m_interval = heart_beat.get_min_delay();
                m_heart_beat = heart_beat.get_timestamp();

                log_info("registration", "Agent has been registered to " << m_url
                                      << ", interval " << m_interval.count() << "s"
                                      << ", rest runs for " << m_heart_beat.count() << "s");

                m_state = State::MONITOR;
                m_event_dispatcher.enable_send_notifications(attach_result.get_amc_event_listener_url());
                break;
            }
        }
    }
    catch (const std::exception& e) {
        log_error("registration", "Cannot register to server " << m_url << "...");
        log_debug("registration", "Cannot register to server " << m_url << ": " << e.what());

        m_state = State::REGISTER;
        if (m_event_dispatcher.send_notifications_enabled()) {
            m_event_dispatcher.disable_send_notifications();
        }
    }
}


void AmcConnectionManager::execute() {
    AmcClient client(m_url);
    do {
        std::this_thread::sleep_for(m_interval);
        update_connection_state(client);
    }
    while (is_running());
}
