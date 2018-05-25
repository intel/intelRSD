/*!
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation.
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
 * @header{Filesystem}
 * @file rmm/event_collector/event_colelctor.hpp
 */

#pragma once

#include "event_collector/event_collector_interface.hpp"

#include <mutex>

namespace agent {
namespace rmm {
namespace event_collector {

/*! @brief Class responsible for collecting events and sending them when required */
class EventCollector : public EventCollectorInterface {
public:

    EventCollector() = default;
    virtual ~EventCollector() = default;

    EventCollector(const EventCollector&) = default;
    EventCollector& operator=(const EventCollector&) = default;

    EventCollector(EventCollector&&) = default;
    EventCollector& operator=(EventCollector&&) = default;

    /*!
     * @brief Adds new resource added event to the pool
     * @param component Type of the component affected by the event
     * @param resource_uuid Uuid of the resource affected by the event
     * @param parent_uuid Uuid of the parent of the affected resource
     */
    virtual void poll_add_event(const enums::Component component, const std::string& resource_uuid,
        const std::string& parent_uuid) override;

    /*!
     * @brief Adds new resource removed event to the pool
     * @param component Type of the component affected by the event
     * @param resource_uuid Uuid of the resource affected by the event
     * @param parent_uuid Uuid of the parent of the affected resource
     */
    virtual void poll_remove_event(const enums::Component component, const std::string& resource_uuid,
        const std::string& parent_uuid) override;

    /*!
     * @brief Adds new resource updated event to the pool
     * @param component Type of the component affected by the event
     * @param resource_uuid Uuid of the resource affected by the event
     * @param parent_uuid Uuid of the parent of the affected resource
     */
    virtual void poll_update_event(const enums::Component component, const std::string& resource_uuid,
        const std::string& parent_uuid) override;

    /*!
     * @brief Sends all stored events and clears the event pool
     */
    virtual void send() override;

    /*!
     * @brief Clears all events stored by the collector
     */
    virtual void clear() override;



private:

    /*! Generic poll event method for any event types */
    void poll_event(const enums::Component component, const std::string& resource_uuid,
        const std::string& parent_uuid, agent_framework::eventing::Notification event_type);

    /*! Returns a function used in searches */
    static std::function<bool (const agent_framework::eventing::EventData&)> has_same_uuid(const std::string& uuid) {
        return [&uuid] (const agent_framework::eventing::EventData& ed) { return ed.get_component() == uuid; };
    };

    std::list<agent_framework::eventing::EventData> m_event_pool{};
};

}
}
}
