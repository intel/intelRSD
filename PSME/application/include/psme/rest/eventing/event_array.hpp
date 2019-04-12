/*!
 * @brief Declaration of EventArray class carrying events for a subscriber
 *
 * If it's m_subscriber_id is empty, the EventArray has yet to be assigned to a subscriber.
 *
 * When a new EventArray is prepared for POST-ing, make sure that it's been assigned a new
 * Redfish id by calling the assign_new_id() method.
 *
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
 * @file rest/eventing/event_array.hpp
 * This class represents Event.Event metadata EntityType
 */

#include "event.hpp"
#include "agent-framework/module/utils/optional_field.hpp"

namespace psme {
namespace rest {
namespace eventing {

struct EventArray {
public:
    /*!
     * @brief Construct EventArray from events
     *
     * @param event_vec Vector of events
     */
    EventArray(const EventVec& event_vec) : m_events(event_vec) { }

    EventArray(const EventArray&) = default;
    EventArray(EventArray&&) = default;
    EventArray& operator=(const EventArray&) = default;
    EventArray& operator=(EventArray&&) = default;

    /*!
     * @brief Set subscriber id
     *
     * @param id Subscriber id
     */
    void set_subscriber_id(const OptionalField<uint64_t>& id) {
        m_subscriber_id = id;
    }

    /*!
     * @brief Get subscriber id
     *
     * @return Subscriber id
     */
    const OptionalField<uint64_t>& get_subscriber_id() const {
        return m_subscriber_id;
    }

    /*!
     * @brief Increment retry attempt
     *
     * @return Number of retry attempts after increment
     */
    unsigned int increment_retry_attempts() {
        return ++m_retry_attempts;
    }

    /*!
     * @brief Get retry attempt count
     *
     * @return Retry attempt count
     */
    unsigned int get_retry_attempts() const {
        return m_retry_attempts;
    }

    /*!
     * @brief Get events (non-const accessor)
     *
     * @return Vector of Event objects
     */
    EventVec& events() {
        return m_events;
    }

    /*!
     * @brief Get events (const accessor)
     *
     * @return Vector of Event objects
     */
    const EventVec& get_events() const {
        return m_events;
    }

    /*!
     * @brief Get context
     *
     * @return Context
     */
    const std::string& get_context() const {
        return m_context;
    }

    /*!
     * @brief Set context
     *
     * @param context Context
     */
    void set_context(const std::string& context) {
        m_context = context;
    }

    /*!
     * @brief Assign unique redfish Id to event array object
     */
    void assign_new_id();

    /*!
     * @brief Remove duplicate events from the Array.
     * Logs warning if two or more events for one resource have different types.
     */
    void remove_duplicates();

    /*!
     * @brief Get event array's id
     *
     * @return the Id of event array object
     */
    uint64_t get_id() const {
        return m_id;
    }

    /*!
     * @brief Convert Event to JSON representation
     *
     * @return JSON representation
     */
    json::Json to_json() const;

private:
    OptionalField<uint64_t> m_subscriber_id{};
    unsigned int m_retry_attempts{0};
    uint64_t m_id{};
    EventVec m_events;
    std::string m_context{};

    static uint64_t EVENT_ARRAY_ID;
};

using EventArrayUPtr = std::unique_ptr<EventArray>;

}
}
}
