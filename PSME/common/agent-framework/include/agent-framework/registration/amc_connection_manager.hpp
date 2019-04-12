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
 *
 * @file amc_connection_manager.hpp
 * @brief AMC application connection manager
 * */

#pragma once



#include "agent-framework/registration/registration_data.hpp"
#include "agent-framework/eventing/event_dispatcher.hpp"
#include "agent-framework/threading/thread.hpp"

#include <chrono>

/*! Agent framework namespace */
namespace agent_framework {
/*! Generic namespace */
namespace generic {

class AmcClient;

/*!
 * @brief Registers agent to AMC (Asset Management Core) application
 * and monitors connection status periodically sending heart-beats.
 *
 * In registration response receives ipv4 address and port number
 * on which AMC application listens for events.
 */
class AmcConnectionManager final : public agent_framework::threading::Thread {
public:
    using Seconds = std::chrono::seconds;

    /*!
     * @brief Defines enumerations for connection status with AMC application
     */
    enum class State {
        MONITOR,
        REGISTER
    };

    static constexpr const Seconds DEFAULT_REGISTRATION_INTERVAL{3};

    /*!
     * @brief Constructor.
     *
     * @param event_dispatcher Event dispatcher sending events to AMC application.
     * @param registration_data AMC application data for registration.
     */
    AmcConnectionManager(EventDispatcher& event_dispatcher,
                         const RegistrationData& registration_data);


private:
    EventDispatcher& m_event_dispatcher;

    std::string m_url{};
    Seconds m_interval{DEFAULT_REGISTRATION_INTERVAL};
    Seconds m_heart_beat{0};
    State m_state{State::REGISTER};

    void execute() override;


    void update_connection_state(const AmcClient& client);


    void wait_until_registered();
};

}
}

