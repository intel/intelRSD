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
 * */

#pragma once
#include "agent-framework/module/enum/enum_builder.hpp"
#include "psme/rest/eventing/event.hpp"
#include <string>
#include <vector>

namespace psme {
namespace rest {
namespace eventing {
namespace model {

/*!
 * @brief EventTypeVec EventType Vector type
 */
using EventTypeVec = std::vector<EventType>;

/*!
 * @brief EventTypes class
 */
class EventTypes {
public:
    /*!
     * @brief Add event type
     *
     * @param event_type Event type
     */
    void add(EventType event_type);

    /*!
     * @brief Get event type collection
     */
    EventTypeVec get() const;

private:
    EventTypeVec m_event_types{};
};

/*!
 * @brief Subscription representation
 */
class Subscription final {
public:
    /*!
     * @brief Set subscription id
     *
     * @param id Subscription id
     */
    void set_id(uint64_t id) {
        m_id = id;
    }

    /*!
     * @brief Get subscription id
     *
     * @return subscription id
     */
    uint64_t get_id() const {
        return m_id;
    }

    /*!
     * @brief Set subscription name
     *
     * @param name Subscription name
     */
    void set_name(const std::string& name) {
        m_name = name;
    }

    /*!
     * @brief Get subscription name
     *
     * @return subscription name
     */
    const std::string& get_name() const {
        return m_name;
    }

    /*!
     * @brief Set subscription destination
     *
     * @param destination Subscription destination
     */
    void set_destination(const std::string& destination) {
        m_destination = destination;
    }

    /*!
     * @brief Get subscription destination
     *
     * @return Subscription destination
     */
    const std::string& get_destination() const {
        return m_destination;
    }

    /*!
     * @brief Set subscription event types
     *
     * @param event_types Subscription event types
     */
    void set_event_types(const EventTypes& event_types) {
        m_event_types = event_types;
    }

    /*!
     * @brief Get subscription event types
     *
     * @return Subscription event types
     */
    const EventTypes& get_event_types() const {
        return m_event_types;
    }

    /*!
     * @brief Set subscription context
     *
     * @param context Subscription context
     */
    void set_context(const std::string& context) {
        m_context= context;
    }

    /*!
     * @brief Get subscription context
     *
     * @return Subscription context
     */
    const std::string& get_context() const {
        return m_context;
    }

    /*!
     * @brief Set subscription protocol
     *
     * @param protocol Subscription protocol
     */
    void set_protocol(const std::string& protocol) {
        m_protocol = protocol;
    }

    /*!
     * @brief Get subscription protocol
     *
     * @return Subscription protocol
     */
    const std::string& get_protocol() const {
        return m_protocol;
    }

    /*!
     * @brief Creates json representation of subscription
     *
     * @return JSON representation of subscription
     */
    json::Value to_json() const;

    /*!
     * @brief Creates model representation from subscription JSON
     *
     * @param json JSON representation of subscription
     * @return Model representation of subscription
     */
    static Subscription from_json(const json::Value& json);

private:
    uint64_t m_id{};
    std::string m_name{};
    std::string m_destination{};
    EventTypes m_event_types{};
    std::string m_context{};
    std::string m_protocol{};
};

/*! Subscription vector type */
using SubscriptionVec = std::vector<Subscription>;

}
}
}
}
