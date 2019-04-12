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
 * @file event_client.hpp
 *
 * @brief Declaration of Event Dispatcher class
 * */

#pragma once

#include "agent-framework/threading/thread.hpp"
#include "agent-framework/eventing/event_sender.hpp"

#include <memory>

/*! AGENT_FRAMEWORK namespace */
namespace agent_framework {
/*! Generic namespace */
namespace generic {

/*!
 * @brief Event Dispatcher pops events from EventsQueue
 * and dispatches them to EventSender.
 */
class EventDispatcher final: public agent_framework::threading::Thread {
public:
    using UPtr = std::unique_ptr<EventDispatcher>;

    /*!
     * @brief Constructor.
     * */
    explicit EventDispatcher();

    /*!
     * @brief Destructor.
     * */
    virtual ~EventDispatcher();

    /*!
     * @brief Checks status of ACM notification service
     * @return true if notifications to ACM are enabled, false otherwise.
     */
    bool send_notifications_enabled() const;

    /*!
     * @brief Enables sending notifications to ACM application.
     * @param url Url of event destination (ACM application)
     */
    void enable_send_notifications(const std::string& url);

    /*!
     * @brief Disables sending event notifications to ACM application.
     */
    void disable_send_notifications();

private:
    std::unique_ptr<EventSender> m_event_sender{};

    void execute();
};

}
}
