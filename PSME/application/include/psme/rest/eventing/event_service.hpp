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
 * @file event_service.hpp
 *
 * @brief Declaration of EventService class
 * */
#pragma once
#include "event_queue.hpp"
#include "model/subscription.hpp"
#include "event.hpp"

#include <atomic>
#include <thread>
#include <vector>

namespace psme {
namespace rest {
namespace eventing {

class EventService;

using psme::rest::eventing::Event;
using psme::rest::eventing::EventUPtr;
using psme::rest::eventing::EventQueue;
using namespace psme::rest::eventing::model;

/*!
 * @brief EventService declaration
 */
class EventService {
public:
    /*!
     * @brief Delivery retry attempts property
     */
    static constexpr char DELIVERY_RETRY_ATTEMPTS_PROP[] = "delivery-retry-attempts";

    /*!
     * @brief Delivery retry interval property
     */
    static constexpr char DELIVERY_RETRY_INTERVAL_PROP[] = "delivery-retry-interval-seconds";

    /*!
     * @brief Default constructor
     */
    EventService();

    /*! @brief Start REST event service */
    void start();

    /*! @brief Stop REST event service */
    void stop();

    /*!
     * @brief Test if EventService is running
     *
     * @return EventService running flag
     */
    bool is_running() const {
        return m_running;
    }

    /*!
     * @brief Post event to subscribers
     *
     * @param event Event
     *
     * @param delay Event delivery delay (default no delay)
     */
    static void post_event(EventUPtr event,
        const steady_clock::duration& delay = steady_clock::duration::zero());

    /*!
     * @brief Get Event queue
     *
     * @return Event queue
     */
    static EventQueue& get_event_queue();

    /*!
     * @brief Get delivery retry interval
     *
     * @return Delivery retry interval
     */
    const std::chrono::seconds& get_delivery_retry_interval() const {
        return m_delivery_retry_interval;
    }

    /*!
     * @brief Get delivery retry attempt count
     *
     * @return Delivery retry attempt count
     */
    unsigned int get_delivery_retry_attempts() const {
        return m_delivery_retry_attempts;
    }

    /*! @brief Destructor */
    ~EventService();
private:
    void m_handle_events();
    SubscriptionVec select_subscribers(const Event& event);

    std::thread m_thread{};
    std::atomic<bool> m_running{false};
    std::chrono::seconds m_delivery_retry_interval{60};
    unsigned int m_delivery_retry_attempts{3};
};

}
}
}

