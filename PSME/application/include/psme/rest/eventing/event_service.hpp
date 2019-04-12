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
 * @file event_service.hpp
 *
 * @brief Declaration of EventService class
 * */
#pragma once
#include "event_array_queue.hpp"

#include <atomic>
#include <thread>
#include <vector>

namespace psme {
namespace rest {
namespace eventing {

class EventService;

using psme::rest::eventing::Event;
using psme::rest::eventing::EventArray;
using psme::rest::eventing::EventArrayUPtr;
using psme::rest::eventing::EventArrayQueue;

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
     * @brief Enqueue events array to be POST'ed to the subscribers
     *
     * @param event_array Container with Events
     *
     * @param delay Events delivery delay (default no delay)
     */
    static void post_events_array(EventArrayUPtr event_array,
        const steady_clock::duration& delay = steady_clock::duration::zero());

    /*!
     * @brief Get Event array queue
     *
     * @return Event array queue
     */
    static EventArrayQueue& get_event_array_queue();

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
    std::vector<EventArray> select_events_for_subscribers(const EventArray& event);
    void send_event_array(const EventArray& event_array);

    std::thread m_thread{};
    std::atomic<bool> m_running{false};
    std::chrono::seconds m_delivery_retry_interval{60};
    unsigned int m_delivery_retry_attempts{3};
};

}
}
}

