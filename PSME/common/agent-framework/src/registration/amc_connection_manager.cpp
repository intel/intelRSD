/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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

#include "configuration/configuration.hpp"

using configuration::Configuration;

using namespace std;
using namespace agent_framework::generic;

/*! Default registration interval */
constexpr const AmcConnectionManager::Seconds
AmcConnectionManager::DEFAULT_REGISTRATION_INTERVAL;

AmcConnectionManager::AmcConnectionManager(EventDispatcher& event_dispatcher)
    :  m_event_dispatcher{event_dispatcher} {
    init();
}

void AmcConnectionManager::init() {
    const auto& configuration = Configuration::get_instance().to_json();
    const auto& m_ipv4 = configuration["registration"]["ipv4"].as_string();
    const auto& m_port = AmcConnectionManager::Port(configuration["registration"]["port"].as_uint());
    m_url = "http://" + m_ipv4 + ":" + std::to_string(unsigned(m_port));
    m_interval = Seconds(configuration["registration"]["interval"].as_uint());
    log_debug(GET_LOGGER("registration"), "Registration url: " << m_url
            << " interval: " << m_interval.count() << "s");

    wait_until_registered();
}

void AmcConnectionManager::wait_until_registered() {
    AmcClient client(m_url);
    while (State::REGISTER == m_state) {
        update_connection_state(client);
        this_thread::sleep_for(m_interval);
    }
}

void AmcConnectionManager::update_connection_state(const AmcClient& client) {
    try {
        switch (m_state) {
            case State::MONITOR:
            {
                auto heart_beat = client.heart_beat();
                if (m_interval < heart_beat.get_min_delay()) {
                    m_interval = heart_beat.get_min_delay();
                }
                if (heart_beat.get_timestamp() < m_heart_beat) {
                    m_state = State::REGISTER;
                    m_event_dispatcher.disable_send_notifications();
                }
            }
            break;
            case State::REGISTER:
            default:
            {
                auto attach_result = client.attach();
                log_info(GET_LOGGER("registration"),
                        "Agent has been registered to " << m_url);

                auto heart_beat = client.heart_beat();
                m_heart_beat = heart_beat.get_timestamp();
                if (m_interval < heart_beat.get_min_delay()) {
                    m_interval = heart_beat.get_min_delay();
                }
                m_state = State::MONITOR;
                m_event_dispatcher.enable_send_notifications(attach_result.get_amc_event_listener_url());
            }
            break;
        }
    } catch (std::exception& e) {
        m_state = State::REGISTER;
        if (m_event_dispatcher.send_notifications_enabled()) {
            m_event_dispatcher.disable_send_notifications();
        }
        log_error(GET_LOGGER("registration"),
                "Cannot register to server " << m_url << "...");
        log_debug(GET_LOGGER("registration"), e.what());
    }
}

void AmcConnectionManager::execute() {

    AmcClient client(m_url);

    do {
        this_thread::sleep_for(m_interval);
        update_connection_state(client);
    } while (is_running());
}
