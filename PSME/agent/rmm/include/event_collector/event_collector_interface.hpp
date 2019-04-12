/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file rmm/event_collector/event_collector_interface.hpp
 */

#pragma once

#include "agent-framework/eventing/events_queue.hpp"
#include "agent-framework/module/enum/common.hpp"

#include <memory>

namespace agent {
namespace rmm {
namespace event_collector {


/*! @brief Class responsible for collecting events and sending them when required */
class EventCollectorInterface {
public:

    EventCollectorInterface() = default;
    virtual ~EventCollectorInterface() = default;

    EventCollectorInterface(const EventCollectorInterface&) = default;
    EventCollectorInterface& operator=(const EventCollectorInterface&) = default;

    EventCollectorInterface(EventCollectorInterface&&) = default;
    EventCollectorInterface& operator=(EventCollectorInterface&&) = default;

    /*!
     * @brief Adds new resource added event to the pool
     * @param component Type of the component affected by the event
     * @param resource_uuid Uuid of the resource affected by the event
     * @param parent_uuid Uuid of the parent of the affected resource
     */
    virtual void poll_add_event(const agent_framework::model::enums::Component component,
        const std::string& resource_uuid,
        const std::string& parent_uuid) = 0;

    /*!
     * @brief Adds new resource removed event to the pool
     * @param component Type of the component affected by the event
     * @param resource_uuid Uuid of the resource affected by the event
     * @param parent_uuid Uuid of the parent of the affected resource
     */
     virtual void poll_remove_event(const agent_framework::model::enums::Component component,
         const std::string& resource_uuid,
         const std::string& parent_uuid) = 0;

    /*!
     * @brief Adds new resource updated event to the pool
     * @param component Type of the component affected by the event
     * @param resource_uuid Uuid of the resource affected by the event
     * @param parent_uuid Uuid of the parent of the affected resource
     */
    virtual void poll_update_event(const agent_framework::model::enums::Component component,
        const std::string& resource_uuid,
        const std::string& parent_uuid) = 0;

    /*!
     * @brief Sends all stored events and clears the event pool
     */
    virtual void send() = 0;

    /*!
     * @brief Clears all events stored by the collector
     */
    virtual void clear() = 0;

};

using EventCollectorInterfacePtr = std::shared_ptr<EventCollectorInterface>;

}
}
}
